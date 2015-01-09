#include <stdint.h>
#include "qep_port.h"
#include "AlgMbsda.h"
#include "AlgMbsdaPrivate.h"
#include "RingBuf.h"
#include "MathFix.h"


//====================================================================
// Global signals
//
enum SystemSignals
{
   XL_DATA_SIG = Q_USER_SIG,  // 4,
   TEST_SIG
};

//====================================================================
// Event structure for collected accelerometer data
//
typedef struct XlDataEvtTag
{
   // QP event structure for all events
   QEvt super;

   uint32_t timeStamp;
   int16_t  x;
   int16_t  y;
   int16_t  z;

} XlDataEvt;

#define MBSDA_STARTUP_DELAY 3000

// Protected State function prototypes
//
static QState Mbsda_initial     (Mbsda * const me, QEvt const * const e);
static QState Mbsda_startUp     (Mbsda * const me, QEvt const * const e);
static QState Mbsda_idle        (Mbsda * const me, QEvt const * const e);

// Local objects
Mbsda l_mbsda;     // Single instance of the Mbsda class

QFsm * const FSM_Mbsda = (QFsm *)&l_mbsda;  // "opaque" QHsm pointer


/**
********************************************************************************
@internal
   Fuction Name: Mbsda_ctor
@endinternal

@b Parameter: @n
@b   Input:   parent   - pointer to parent Active Object  @n
@b   Returns: (QHsm *) - pointer to "this" module for dispatching events  @n

@b Description: @n
    MBSDA constructor that is used to call other QP constructors and initialize
    any necessary variables

*******************************************************************************/
QFsm * Mbsda_ctor(void)
{
   uint16_t orderIdx;
   uint16_t widthIdx;

   Mbsda *me       = &l_mbsda;

   // Call QP related constructors
   //
   QFsm_ctor(&me->super, (QStateHandler)&Mbsda_initial);

   // Initializing data members used in the Motion-Based Algorithm
   //
/*
   me->pgm.alpha         = MBSDA_DEF_ALPHA;
   me->pgm.stdaThLow     = MBSDA_DEF_ACT_THL;
   me->pgm.stdaThHigh    = MBSDA_DEF_ACT_THH;
   me->pgm.activityTime  = MBSDA_DEF_ACT_TIME;
   me->pgm.mTpkThLow     = MBSDA_DEF_MTPK_THL;
   me->pgm.mTpkThHigh    = MBSDA_DEF_MTPK_THH;
   me->pgm.mTpkRThLow    = MBSDA_DEF_MTPKR_THL;
   me->pgm.mTpkRThHigh   = MBSDA_DEF_MTPKR_THH;
   me->pgm.freqPendTime  = MBSDA_DEF_FREQ_TIME;
   me->pgm.freqDelayTime = MBSDA_DEF_FREQDLY_TIME;
   me->pgm.zTh           = MBSDA_DEF_Z_TH;
   me->pgm.zStdTh        = MBSDA_DEF_ZSTD_TH;
   me->pgm.expTime       = MBSDA_DEF_EXP_TIME;
   me->pgm.freqHys       = MBSDA_DEF_FREQ_HYS;
   me->pgm.freqPkCnt     = MBSDA_DEF_FREQ_PK_CNT;
*/
   me->xlSampleCnt = 0;

   me->xQ.in = me->xQ.rIn = me->xQ.wIn = me->xBufSto;
   me->yQ.in = me->yQ.rIn = me->yQ.wIn = me->yBufSto;
   me->zQ.in = me->zQ.rIn = me->zQ.wIn = me->zBufSto;

   me->lastXyzFilt[0] = 0;
   me->lastXyzFilt[1] = 0;
   me->lastXyzFilt[2] = 0;

   me->lastTimestamp = 0;
   me->startTick     = 0;

   //
   // Initialize parameters related to low frequency magnitude filtration
   //
   me->lfFiltOutput = 0;
   for(orderIdx = 0; orderIdx < MBSDA_LFMAG_ORDR_SZ; orderIdx++)
   {
      me->lowFreqMag[orderIdx].aggregate = 0;

      me->lowFreqMag[orderIdx].queue.in     =
         me->lowFreqMag[orderIdx].queue.rIn =
         me->lowFreqMag[orderIdx].queue.wIn =
                                   (me->lfBufSto+MBSDA_LFMAG_WDTH_SZ*orderIdx);

      for(widthIdx = 0; widthIdx < MBSDA_LFMAG_WDTH_SZ; widthIdx++)
      {
         WriteBuf(&me->lowFreqMag[orderIdx].queue.wIn,
                   me->lowFreqMag[orderIdx].queue.in, 0, MBSDA_LFMAG_WDTH_SZ);
      }
   }

   //
   // Initialize parameters related to scaled derivative calculations
   //
   me->sclDerCurr = 0;
   me->sclDerPrev = 0;
   me->sclDer.queue.in     =
      me->sclDer.queue.rIn =
      me->sclDer.queue.wIn = me->sclDerBufSto;
   for(widthIdx = 0; widthIdx < MBSDA_DER_WDTH_SZ; widthIdx++)
   {
      WriteBuf(&me->sclDer.queue.wIn, me->sclDer.queue.in,
                0, MBSDA_DER_WDTH_SZ);
   }

   //
   // Initialize parameters related to peak-peak period calculations
   //
   me->mTpkFiltOutput  = 0;
   me->mdTpkFiltOutput = 0;
   for(orderIdx = 0; orderIdx < MBSDA_INT_ORDR_SZ; orderIdx++)
   {
      me->mTpk[orderIdx].aggregate  = 0;
      me->mdTpk[orderIdx].aggregate = 0;

      me->mTpk[orderIdx].queue.in     =
         me->mTpk[orderIdx].queue.rIn =
         me->mTpk[orderIdx].queue.wIn =
                                   (me->mTpkBufSto+MBSDA_INT_WDTH_SZ*orderIdx);

      me->mdTpk[orderIdx].queue.in     =
         me->mdTpk[orderIdx].queue.rIn =
         me->mdTpk[orderIdx].queue.wIn =
                                   (me->mdTpkBufSto+MBSDA_INT_WDTH_SZ*orderIdx);
      for(widthIdx = 0; widthIdx < MBSDA_INT_WDTH_SZ; widthIdx++)
      {
         WriteBuf(&me->mTpk[orderIdx].queue.wIn,
                   me->mTpk[orderIdx].queue.in,  0, MBSDA_LFMAG_WDTH_SZ);
         WriteBuf(&me->mdTpk[orderIdx].queue.wIn,
                   me->mdTpk[orderIdx].queue.in, 0, MBSDA_LFMAG_WDTH_SZ);
      }
   }

   me->freqNumPk[MBSDA_PKNEG_IDX] = 0;
   me->freqNumPk[MBSDA_PKPOS_IDX] = 0;

   me->szFlags     = 0;
   me->freqPkCntr  = 0;

   me->pgmSsValue = 0;  // Set to '0' to indicate using default values for
                        //  a possible failed setting attempt

   return (QFsm *)me;
}

/**
********************************************************************************
@internal
   Fuction Name: Mbsda_initial
@endinternal

@b Parameter: @n
@b   Input:   none  @n
@b   Returns: none  @n

@b Description: @n
     ...

*******************************************************************************/
QState Mbsda_initial(Mbsda * const me, QEvt const * const e)
{
   (void)e; // suppress the compiler warning about unused parameter

   return Q_TRAN(&Mbsda_startUp);
}

/**
********************************************************************************
@internal
   Fuction Name: Mbsda_startUp
@endinternal

@b Parameter: Standard QP State inputs/returns. Defined in qpc\include\qep.h @n

@b Description: @n
    This state is used to let the filter settle before starting to check
      for activity levels.
@n
@b Constraints: @n
    This state and others that have guard conditions based on a timeout & will
    calculate a delta time from the timestamp provided in the accelerometer
    data event. This is used instead of a QP timer to allow the processor to
    enter a lower power state while the time period is getting checked. The
    QP timer currently requires the high frequency clock which does not run
    in the lower power state.
    @n
    Another reason for using a delta time calculation is to support running
    this state machine in a simulated environment where the data is provided
    from a file instead of the accelerometer directly. This allows running
    more data in a shorter period of time.

*******************************************************************************/
QState Mbsda_startUp(Mbsda * const me, QEvt const * const e)
{
   switch (e->sig)
   {
      case Q_ENTRY_SIG:
      {
         // Capture the current timestamp, will check against the timestamp
         //  received in the XL_DATA for the timeout condition.
         //
         me->startTick = me->lastTimestamp;

         return Q_HANDLED();
      }
      case Q_EXIT_SIG:
      {
         return Q_HANDLED();
      }

      case XL_DATA_SIG:
      {
//         Mbsda_process(me, e);

         if((me->lastTimestamp - me->startTick) > MBSDA_STARTUP_DELAY)
         {
            return Q_TRAN(&Mbsda_idle);
         }

         return Q_HANDLED();
      }
   }
   return Q_IGNORED();
}

/**
********************************************************************************
@internal
   Fuction Name: Mbsda_idle
@endinternal

@b Parameter: Standard QP State inputs/returns. Defined in qpc\include\qep.h @n

@b Description: @n
    Process incoming accelerometer data using the motion-based seizure
    detection algorithm. After processing the data, the activity will be
    checked and if lower than the programmed low threshold will transition
    to the low activity state.

*******************************************************************************/
QState Mbsda_idle(Mbsda * const me, QEvt const * const e)
{
   switch (e->sig)
   {
      case Q_ENTRY_SIG:
      {
         return Q_HANDLED();
      }
      case Q_EXIT_SIG:
      {
         return Q_HANDLED();
      }

      case XL_DATA_SIG:
      {
/*
         Mbsda_process(me, e);

         if(me->stdaXyz < me->pgm.stdaThLow)
         {
            return Q_TRAN(&Mbsda_lowActivity);
         }
*/
         return Q_HANDLED();
      }
   }
   return Q_IGNORED();
}





