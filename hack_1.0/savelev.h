/* Copyright (c) Stichting Mathematisch centrum, Amsterdam, 1984. */

#ifndef SAVELEV_H_
#define SAVELEV_H_

#include "def.gen.h" /* gen */
#include "def.monst.h" /* monst */
#include "def.obj.h" /* obj */

void bwrite(int fd, char *loc, unsigned int num);
void savegenchn(int fd, struct gen *gtmp);
void savemonchn(int fd, struct monst *mtmp);
void saveobjchn(int fd, struct obj *otmp);

#ifdef MKLEV
void savelev();
#else
void savelev(int fd);
#endif

#endif
