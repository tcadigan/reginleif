/* Copyright (c) Stichting Mathematisch centrum, Amsterdam, 1984. */

#ifndef SAVELEV_H_
#define SAVELEV_H_

void bwrite(int fd, char *loc, unsigned int num);

#ifdef MKLEV
void savelev();
#else
void savelev(int fd);
#endif

#endif
