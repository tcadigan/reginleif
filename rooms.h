#ifndef ROOMS_H_
#define ROOMS_H_

#include "rogue.h"

int do_rooms();
int draw_room(struct room *rp);
int horiz(int cnt);
int vert(int cnt);
int rnd_pos(struct room *rp, coord *cp);

#endif
