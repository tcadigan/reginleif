/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#ifndef DEF_GEN_H_
#define DEF_GEN_H_

#include "config.h" /* xchar */

struct gen {
    struct gen *ngen;
    xchar gx;
    xchar gy;
    unsigned int gflag; /* 037: trap type; 040: SEEN flag */
#define SEEN 040
};

extern struct gen *fgold;
extern struct gen *ftrap;

#define newgen() (struct gen *)alloc(sizeof(struct gen))

#endif
