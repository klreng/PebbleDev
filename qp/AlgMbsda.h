/**
********************************************************************************
@internal
Copyright(c) 2013 Cyberonics Inc.  All Rights Reserved.

This software is proprietary and confidential.  By using this software
You agree with the terms of the associated Cyberonics Inc. License Agreement
This file is documented using Doxygen annotations for extraction of detail
design description items.
@endinternal

@file  AlgMbsda.h

@brief  @b Description: @n
   This file includes the function prototypes for the Motion Based Seizure
   Detection Algorithm (MBSDA).

@internal
* Change Log: Major releases will be captured here, minor releases will use
*             SVN check-in/history log for details.
@endinternal
*******************************************************************************/
#ifndef ALGMBSDA_H
#define ALGMBSDA_H

//
// PUBLIC FUNCTION PROTOTYPE
//
QFsm * Mbsda_ctor(void);

extern QFsm * const FSM_Mbsda;

#endif // ALGMBSDA_H
