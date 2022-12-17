/* Copyright (c) Stichting Mathematisch centrum, Amsterdam, 1984. */

#ifndef MKLV_SAVELEV_H_
#define MKLV_SAVELEV_H_

#include "def.gen.h" /* gen */
#include "mklv.def.monst.h" /* monst */
#include "def.obj.h" /* obj */

void bwrite(int fd, char *loc, unsigned int num);
void savegenchn(int fd, struct gen *gtmp);
void savemonchn(int fd, struct monst *mtmp);
void saveobjchn(int fd, struct obj *otmp);

void savelev();

#endif
