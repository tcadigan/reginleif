#ifndef CHASE_H_
#define CHASE_H_

#include "rogue.h"

int runners();
int do_chase(struct thing *th);
int runto(coord *runner, coord *spot);
int chase(struct thing *tp, coord *ee);
struct room *roomin(coord *cp);
struct linked_list *find_mons(int y, int x);
int diag_ok(coord *sp, coord *ep);
int cansee(int y, int x);

#endif
