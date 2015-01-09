/**
********************************************************************************
@internal
Copyright(c) 2014 Cyberonics Inc.  All Rights Reserved.

This software is proprietary and confidential.  By using this software
You agree with the terms of the associated Cyberonics Inc. License Agreement
This file is documented using Doxygen annotations for extraction of detail
design description items.
@endinternal

@file  AlgMbsdaPrivate.h

@brief  @b Description: @n
   This file includes the structure that defines the MBSDA component. Most
    cases will have this structure defined in the source file, but for testing
    purposes this structure needs to be exposed to test related source files.

@internal
* Change Log: Major releases will be captured here, minor releases will use
*             SVN check-in/history log for details.
@endinternal
*******************************************************************************/
#ifndef ALGMBSDA_PRIVATE_H
#define ALGMBSDA_PRIVATE_H

#include "qep_port.h"

//==============================================================================
// Mbsda state machine structure definition dependencies
//==============================================================================
#define MBSDA_ORD_SIZE      39  // Order value for 50 Hz sampling rate

#define MBSDA_LFMAG_WDTH_SZ  4  // Width of integrator kernel
#define MBSDA_LFMAG_ORDR_SZ  4  // Order of integrator kernel
#define MBSDA_DER_WDTH_SZ    2  // Width size for derivative calculations
#define MBSDA_DER_ORDR_SZ    1  // Order size for derivative calculations
#define MBSDA_INT_WDTH_SZ   24  // Smoothing width (peaks)
#define MBSDA_INT_ORDR_SZ    2  // Smoothing order

#define MBSDA_PKPOS_IDX      0  // Index for negative peak parameters
#define MBSDA_PKNEG_IDX      1  // Index for positive peak parameters
#define MBSDA_SIGN_IDX_SZ    2

#define XL_NUM_AXIS 3
#define XL_X_AXIS   0
#define XL_Y_AXIS   1
#define XL_Z_AXIS   2

// ===================================================================
/// struct @b XlQueue - queue structure for ring buffer functions
// ===================================================================
typedef struct XlQueueTag
{
  int16_t *in;  // Specific buffer storage pointer
  int16_t *rIn; // read pointer
  int16_t *wIn; // write pointer

} XlQueue;

// ===================================================================
/// struct @b XlFilter - common structure for filter calculations
// ===================================================================
typedef struct XlFilterTag
{
   int32_t           output;    // filtered output value
   int32_t           aggregate; // Cumulative Sum value
   struct XlQueueTag queue;     // Queue for this specific filter calculations

} XlFilter;

//==============================================================================
/// struct @b Mbsda - State machine class/structure for the MBSDA module.
//==============================================================================
typedef struct MbsdaTag
{
   QFsm super; // QP required "inherited" structure for component-based state
               //   machine
               // THIS MUST BE THE FIRST ITEM IN THIS STRUCTURE

//   MbsdaPgmParms pgm;         // Programmable parameters structure
   uint8_t       pgmSsValue;  // Current sensitivity setting used

   // ======================================================
   // Queue buffer storage allocation
   // X,Y,Z buffer storage
   int16_t xBufSto[MBSDA_ORD_SIZE];
   int16_t yBufSto[MBSDA_ORD_SIZE];
   int16_t zBufSto[MBSDA_ORD_SIZE];

   // Frequency module related filter buffer storage
   int16_t lfBufSto    [MBSDA_LFMAG_WDTH_SZ*MBSDA_LFMAG_ORDR_SZ];
   int16_t sclDerBufSto[MBSDA_DER_WDTH_SZ];
   int16_t mTpkBufSto  [MBSDA_INT_WDTH_SZ*MBSDA_INT_ORDR_SZ];
   int16_t mdTpkBufSto [MBSDA_INT_WDTH_SZ*MBSDA_INT_ORDR_SZ];

   // ======================================================
   // Activity based filter queues for x, y, & z axis
   XlQueue xQ;
   XlQueue yQ;
   XlQueue zQ;

   uint16_t stdaXyz;       // Short Term Dynamic Activity (stda) measure
   uint32_t xlSampleCnt;   // Accumulated count of XL samples

   uint32_t lastXyzFilt[XL_NUM_AXIS]; // last filtered value storage

   uint32_t lastTimestamp; // Capture the last timestamp from last sample
   uint32_t startTick;     // Capture specific timestamp for delta calculations

   //
   // FREQUENCY module based parameters
   //
   int32_t  lfFiltOutput; // low frequency filtered output value
   XlFilter lowFreqMag[MBSDA_LFMAG_ORDR_SZ]; // Boxcar filter LF struct calcs
   int32_t  sclDerCurr;   // Current scaled-derivative value
   int32_t  sclDerPrev;   // Previous scaled-derivative value
   XlFilter sclDer;       // Scaled-derivative filter struct for calculations
   int32_t  mTpkFiltOutput; // peak-peak period value filtered output
   XlFilter mTpk[MBSDA_INT_ORDR_SZ]; // peak-peak period filter struct
   int32_t  mdTpkFiltOutput;// peak-peak deviation filtered output
   XlFilter mdTpk[MBSDA_INT_ORDR_SZ];// peak-peak deviation filter struct

   int16_t mTpkR;   // Ratio of filtered mdTpk and mTpk
   int16_t zAct;    // Captured Z value when activity starts
   int16_t zStdFreq;// Captured Z filtered value in freqPending

   // Parameters for calculations in getPkPrStat function
   //
   int32_t freqNumPk[MBSDA_SIGN_IDX_SZ];
   int32_t freqPkV[MBSDA_SIGN_IDX_SZ];
   int32_t freqPkIdxCurr[MBSDA_SIGN_IDX_SZ];
   int32_t freqPkIdxPrev[MBSDA_SIGN_IDX_SZ];

   // pkStat specific parameters
   int32_t freqTpkCurr;
   int32_t freqTpkPrev;
   int16_t freqDpk;

   uint8_t szFlags;    // Status flag value to provide state information

   uint8_t freqPkCntr; // Number of peaks counter

} Mbsda;

#endif // ALGMBSDA_PRIVATE_H
