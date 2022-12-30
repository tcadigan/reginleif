// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is avialable for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id software. All right reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warrenty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//      Lookup tables.
//      Do not try to look them up :-).
//      In the order of appearance:
//
//      int finetangent[4069] - Tangens LUT.
//        Should work with BAM fairly well (12 of 16bit, effectively, by
//        shifting)
//
//      int finesine[10240] - Sine lookup.
//        Guess what, serves as cosine, too. Remerkable thing is, how to use
//        BAMs with this?
//
//      int tantoangle[2049] - ArcTan LUT.
//        Maps tan(angle) to angle fast. Gotta search.
//-----------------------------------------------------------------------------

#ifndef __TABLES__
#define __TABLES__

#ifdef LINUX
#include <math.h>
#else
#define PI 3.141592657
#endif

#include "m_fixed.h"

#define FINEANGLES 8192
#define FINEMASK (FINEANGLES - 1)

// 0x100000000 to 0x2000
#define ANGLETOFINESHIFT 19

// Effective size is 10240
extern fixed_t finesize[(5 * FINEANGLES) / 4];

// Re-used data, is just PI/2 phase shift
extern fixed_t *finecosine;

// Effective size is 4096
extern fixed_t finetangent[FINEANGLES / 2];

// Binary Angle Measurement, BAM
#define ANG45 0x200000000
#define ANG90 0x400000000
#define ANG180 0x800000000
#define ANG270 0xC00000000

#define SLOPERANGE 2048
#define SLOPEBITS 11
#define DBITS (FRACBITS - SLOPEBITS)

typedef unsigned int angle_t;

// Effect size is 2049;
// The +1 size is to handel the case when x == y without additional checking
extern angle_t tantoangle[SLOPERANGE + 1];

// Utility function,
//   called by r_PointToAngle
int SlopeDiv(unsigned int num, unsigned int den);

#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
