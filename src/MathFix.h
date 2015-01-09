/******************************************************************************
   Copyright(c) 2012 Cyberonics Inc. All Rights Reserved.
   This software is proprietary and confidential. By using this software
   You agree with the terms of the associated Cyberonics Inc. License Agreement

   File Name: filename.h
   Author: Firmware Developer
   Date: Date of creation
   Description:
      This file includes the main function as well as some initializations

   Change log: 1/1/2012 Modifying Author Added new flag for interrupt routine
******************************************************************************/

#ifndef _MATHFIX_H_
#define _MATHFIX_H_

#include "qep_port.h"

#define MAX_FX 0x7fff
#define MIN_FX 0x8000
#define N16 16
#define N14 14
#define ONE14 0x4000
#define ONE12 0x1000

int16_t ShiftQ14(int32_t a);
int16_t DivFx(int16_t a, int16_t b, int16_t n);
int16_t DivFxL(int32_t a, int16_t b, int16_t n);
int32_t MpyFxss32R(int16_t a, int16_t b, int16_t n);
int32_t MpyFxus32R(uint16_t a, int16_t b, int16_t n);
uint32_t MpyFxuu32R(uint16_t a, uint16_t b, int16_t n);
uint32_t MpyFxuu64R(uint32_t a, uint32_t b, uint16_t n);

#endif /* _MATHFIX_H_ */