#ifndef CHASE_H_
#define CHASE_H_

#include "rogue.h"

int runners();
int do_chase(struct thing *th);
int runto(struct coord *runner, struct coord *spot);
int chase(struct thing *tp, struct coord *ee);
struct room *roomin(struct coord *cp);
struct linked_list *find_mons(int y, int x);
int diag_ok(struct coord *sp, struct coord *ep);
int cansee(int y, int x);

#endif
