/**
********************************************************************************
@internal
Copyright(c) 2013 Cyberonics Inc.  All Rights Reserved.

This software is proprietary and confidential.  By using this software
You agree with the terms of the associated Cyberonics Inc. License Agreement
This file is documented using Doxygen annotations for extraction of detail
design description items.
@endinternal

@file  MathFix.c

@brief  @b Description: @n
   This file includes fix point math functions

   Q format numbers are (notionally) fixed point numbers (but not actually a number itself).
   That is, they are stored and operated upon as regular binary numbers (i.e. signed integers),
   thus allowing standard integer hardware/ALU to perform rational number calculations.
   The number of integer bits, fractional bits and the underlying word size are to be chosen
   by the programmer on an application-specific basis — the programmer's choices
   of the foregoing will depend on the range and resolution needed for the numbers.
   The machine itself remains oblivious to the notional fixed point representation
   being employed — it merely performs integer arithmetic the way it knows how.
   Ensuring that the computational results are valid in the Q format representation
   is the responsibility of the programmer.

   The Q notation is written as Qm.n, where:
   m is the number of bits set aside to designate the two's complement integer portion of
   the number, exclusive of the sign bit (therefore if m is not specified it is taken as zero).
   n is the number of bits used to designate the fractional portion of the number, i.e.
   the number of bits to the right of the binary point. (If n = 0, the Q numbers are
   integers — the degenerate case).

   Note that the most significant bit is always designated as the sign bit (the number is stored
   as a two's complement number) in order to allow standard arithmetic-logic hardware to manipulate
   Q numbers. Representing a signed fixed-point data type in Q format therefore always requires
   m+n+1 bits to account for the sign bit. Hence the smallest machine word size required
   to accommodate a Qm.n number is m+n+1, with the Q number left justified in the machine word.

   Checking the conditions of the inpit parameters these math functions have to be controlled outside
   for more speed efficient calculation (some algorithms calculation results has to be obtained before
   next sample occures, for example this is 20 ms time period for accelerometer.
@internal
* Change Log: Major releases will be captured here, minor releases will use
*             SVN check-in/history log for details.
@endinternal
*******************************************************************************/

#include <Math.h>
#include "MathFix.h"
#include "qep_port.h"

// rounding array, support rounding and right shift up to 25 bits
uint32_t rnd[25] = {0x0000,0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
                           0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000,
                           0x10000,0x20000,0x40000,0x80000,0x100000,0x200000,0x400000,0x800000};


/**
********************************************************************************
@internal
   Fuction Name: DivFx
@endinternal

@b Parameter: @n
@b   Input:   dividend - signed 16-bit dividend       @n
@b            divisor - signed 16-bit divisor         @n
@b            shift - scaling factor                  @n
@b   Returns: result - 16-bit signed rounded result   @n

@b Description: @n
    Signed 16-bit by signed 16-bit fix point division with rounding
    of 16-bit signed result and adjusting of fix point.
    Division by zero has to be controlled outside.

*******************************************************************************/
int16_t DivFx(int16_t dividend, int16_t divisor, int16_t shift)
{
   int16_t result; int32_t temp;

   temp = ((int32_t) dividend)<<N16;
   temp /= divisor;

   if (temp >= 0)
   {
      temp += rnd[N16-shift]; // round positive number
   }
   else
   {
      temp -= rnd[N16-shift]; // round negative number
   }

   result = (int16_t) (temp >> (N16-shift)); // adjust fix point after rounding

   return result;
}

/**
********************************************************************************
@internal
   Fuction Name: DivFxL
@endinternal

@b Parameter: @n
@b   Input:   dividend - signed 32-bit dividend       @n
@b            divisor - signed 16-bit divisor         @n
@b            shift - scaling factor                  @n
@b   Returns: result - 16-bit signed rounded result   @n

@b Description: @n
    Signed 32-bit by signed 16-bit fix point division with rounding
    of 16-bit signed result and adjusting of fix point.
    Division by zero has to be controlled outside.

*******************************************************************************/
int16_t DivFxL(int32_t dividend, int16_t divisor, int16_t shift)
{
   int16_t result;

   dividend /= divisor;

   if (dividend >= 0)
   {
      dividend += rnd[N16-shift]; // round positive number
   }
   else
   {
      dividend -= rnd[N16-shift]; // round negative number
   }

   result = (int16_t) (dividend >> (N16-shift)); // scaling

   return result;
}

/**
********************************************************************************
@internal
   Fuction Name: MpyFxss32R
@endinternal

@b Parameter: @n
@b   Input:   multiplicand - signed 16-bit multiplier   @n
@b            multiplier- signed 16-bit multiplier   @n
@b            shift - scaling factor             @n
@b   Returns: result - 16-bit signed rounded result  @n

@b Description: @n
    Signed 16-bit by signed 16-bit fix point multiplication with
    rounding of 32-bit signed result and adjusting of fix point

*******************************************************************************/
int32_t MpyFxss32R(int16_t multiplicand, int16_t multiplier, int16_t shift)
{
   int32_t result;

   result = (int32_t) multiplicand*multiplier;

   if (result >= 0)
   {
      result += rnd[shift]; // round positive number
   }
   else
   {
      result -= rnd[shift]; // round negative number
   }

   result >>= shift; // scaling

   return (result);
}

/**
********************************************************************************
@internal
   Fuction Name: MpyFxus32R
@endinternal

@b Parameter: @n
@b   Input:   multiplicand - unsigned 16-bit multiplier  @n
@b            multiplier - signed 16-bit multiplier    @n
@b            shift - scaling factor              @n
@b   Returns: c - 32-bit signed rounded result  @n

@b Description: @n
    Unsigned 16-bit by signed 16-bit fix point multiplication with
    rounding of 32-bit signed result and adjusting of fix point

*******************************************************************************/
int32_t MpyFxus32R(uint16_t multiplicand, int16_t multiplier, int16_t shift)
{
   int32_t result;

   result = (int32_t)  multiplicand*multiplier;

   if (result >= 0)
   {
      result += rnd[shift]; // round positive number
   }
   else
   {
      result -= rnd[shift]; // round negative number
   }

   result >>= shift; // scaling

   return (result);
}

/**
********************************************************************************
@internal
   Fuction Name: MpyFxuu32R
@endinternal

@b Parameter: @n
@b   Input:   multiplicand - unsigned 16-bit multiplier   @n
@b            b - unsigned 16-bit multiplier   @n
@b            n - scaling factor               @n
@b   Returns: c - 32-bit unsigned rounded result  @n

@b Description: @n
    Unsigned 16-bit by unsigned 16-bit fix point multiplication with
    rounding of 32-bit unsigned result and adjusting of fix point

*******************************************************************************/
uint32_t MpyFxuu32R(uint16_t multiplicand, uint16_t multiplier, int16_t shift)
{
   uint32_t result;

   result = (uint32_t) multiplicand*multiplier;
   result += rnd[shift]; // round positive number
   result >>= shift; // scaling

   return (result);
}

/**
********************************************************************************
@internal
   Fuction Name: MpyFxuu64R
@endinternal

@b Parameter: @n
@b   Input:   a - unsigned 32-bit multiplier   @n
@b            b - unsigned 32-bit multiplier   @n
@b            n - scaling factor               @n
@b   Returns: c - 32-bit unsigned rounded result  @n

@b Description: @n
    Unsigned 32-bit by unsigned 32-bit fix point multiplication with
    rounding of 32-bit unsigned result and adjusting of fix point

*******************************************************************************/
uint32_t MpyFxuu64R(uint32_t multiplicand, uint32_t multiplier, uint16_t shift)
{
   uint64_t result;

   result = (uint64_t) multiplicand*multiplier;
   result += rnd[shift]; // round positive number
   result >>= shift; // scaling

   return ((uint32_t) result);
}

/**
********************************************************************************
@internal
   Fuction Name: ShiftQ14
@endinternal

@b Parameter: @n
@b   Input:   data - signed 32-bit   @n
@b   Returns: shift - fix point position  @n

@b Description: @n
    Calculate fix point position

*******************************************************************************/
int16_t ShiftQ14(int32_t data)
{
   uint16_t shift = 0;
   int32_t tmp;

   tmp = labs(data)>>N14;
   while (tmp > 0)
   {
      tmp >>= 1;
      shift++;
   }

   return (shift);
}