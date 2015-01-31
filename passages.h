#ifndef PASSAGES_H_
#define PASSAGES_H_

#include "rogue.h"

int do_passages();
int conn(int r1, int r2);
int door(struct room *rm, coord *cp);
int add_pass();

#endif
