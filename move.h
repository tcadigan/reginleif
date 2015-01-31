#ifndef MOVE_H_
#define MOVE_H_

#include "rogue.h"

int do_run(char ch);
int do_move(int dy, int dx);
int light(coord *cp);
char show(int y, int x);
int be_trapped(coord *tc);
struct trap *trap_at(int y, int x);
coord *rndmove(struct thing *who);

#endif
