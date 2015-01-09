/**
********************************************************************************
@internal
Copyright(c) 2013 Cyberonics Inc.  All Rights Reserved.

This software is proprietary and confidential.  By using this software
You agree with the terms of the associated Cyberonics Inc. License Agreement
This file is documented using Doxygen annotations for extraction of detail
design description items.
@endinternal

@file  RingBuf.c

@brief  @b Description: @n
   This file includes functions for ring buffer implementation

@internal
* Change Log: Major releases will be captured here, minor releases will use
*             SVN check-in/history log for details.
@endinternal
*******************************************************************************/

#include "qep_port.h"
#include "RingBuf.h"

/**
********************************************************************************
@internal
   Fuction Name: FlushBuf
@endinternal

@b Parameter: @n
@b   Input:   **rPtr - pointer to read pointer   @n
@b            **wPtr - pointer to write pointer  @n
@b            *buf -  buffer pointer @n
@b   Returns: none  @n

@b Description: @n
    Reset all ring buffer's pointers.

*******************************************************************************/
void FlushBuf(int16_t **rPtr, int16_t **wPtr, int16_t *buf)
{
   *rPtr = *wPtr = buf;
}

/**
********************************************************************************
@internal
   Fuction Name: ReadBuf
@endinternal

@b Parameter: @n
@b   Input:   **bufPtr - pointer to read pointer   @n
@b            *buf - buffer pointer                @n
@b            size - size of buffer                @n
@b   Returns: read data  @n

@b Description: @n
    Read data from a ring buffer.

*******************************************************************************/
int16_t ReadBuf(int16_t **bufPtr, int16_t *buf, uint16_t size)
{
   int16_t data;

   data = **bufPtr;
   (*bufPtr)++;
   if (*bufPtr > buf+size-1)
   {
      *bufPtr = buf;
   }
   return (data);
}

/**
********************************************************************************
@internal
   Fuction Name: ReadBufR
@endinternal

@b Parameter: @n
@b   Input:   **bufPtr - pointer to read pointer   @n
@b            *buf - buffer pointer                @n
@b            size - size of buffer                @n
@b   Returns: read data  @n

@b Description: @n
    Read data from ring buffer in reverse order.

*******************************************************************************/
int16_t ReadBufR(int16_t **bufPtr, int16_t *buf, uint16_t size)
{
   uint16_t data;

   data = **bufPtr;
   (*bufPtr)--;
   if (*bufPtr < buf)
   {
      *bufPtr = buf+size-1;
   }
   return (data);
}

/**
********************************************************************************
@internal
   Fuction Name: WriteBuf
@endinternal

@b Parameter: @n
@b   Input:   **bufPtr - pointer to write pointer  @n
@b            *buf - buffer pointer                @n
@b            data - data to be written            @n
@b            size - size of buffer                @n
@b   Returns: none  @n

@b Description: @n
    Write data in a ring buffer.

*******************************************************************************/
void WriteBuf(int16_t **bufPtr, int16_t *buf, int16_t data, uint16_t size)
{
    **bufPtr = data;
    (*bufPtr)++;
    if (*bufPtr > buf+size-1)
    {
        *bufPtr = buf;
    }
}

/**
********************************************************************************
@internal
   Fuction Name: WriteBufR
@endinternal

@b Parameter: @n
@b   Input:   **bufPtr - pointer to write pointer  @n
@b            *buf - buffer pointer                @n
@b            data - data to be written            @n
@b            size - size of buffer                @n
@b   Returns: none  @n

@b Description: @n
    Write data in a ring buffer in reverse order.

*******************************************************************************/
void WriteBufR(int16_t **bufPtr, int16_t *buf, int16_t data, uint16_t size)
{
   **bufPtr = data;
   (*bufPtr)--;
   if (*bufPtr < buf)
   {
      *bufPtr = buf+size-1;
   }
}

