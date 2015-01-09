/**
********************************************************************************
@internal
Copyright(c) 2013 Cyberonics Inc.  All Rights Reserved.

This software is proprietary and confidential.  By using this software
You agree with the terms of the associated Cyberonics Inc. License Agreement
This file is documented using Doxygen annotations for extraction of detail
design description items.
@endinternal

@file  RingBuf.h

@brief  @b Description: @n
   This file includes the prototypes for the ring buffer operations

@internal
* Change Log: Major releases will be captured here, minor releases will use
*             SVN check-in/history log for details.
@endinternal
*******************************************************************************/

#ifndef _RINGBUF_H_
#define _RINGBUF_H_

#include "qep_port.h"

#ifdef WIN32
   #define INLINE __inline
#else
   #define INLINE inline
#endif

extern INLINE void FlushBuf(int16_t **rPtr, int16_t **wPtr, int16_t *buf);
extern INLINE void WriteBuf(int16_t **bufPtr, int16_t *buf, int16_t data,
                            uint16_t bufSize);
extern INLINE void WriteBufR(int16_t **bufPtr, int16_t *buf, int16_t data,
                             uint16_t bufSize);
extern INLINE int16_t ReadBuf(int16_t **bufPtr, int16_t *buf,
                              uint16_t bufSize);
extern INLINE int16_t ReadBufR(int16_t **bufPtr, int16_t *buf,
                               uint16_t bufSize);

#endif /* _RINGBUF_H_ */
