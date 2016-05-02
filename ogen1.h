#ifndef OGEN1_H_
#define OGEN1_H_

#include "odefs.h"

void clear_level(struct level *dungeon_level);
char *roomname(int index);
void lset(int x, int y, int stat);
void lreset(int x, int y, int stat);
int loc_statusp(int x, int y, int stat);

#endif
