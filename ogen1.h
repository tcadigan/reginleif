#ifndef OGEN1_H_
#define OGEN1_H_

#include "odefs.h"

void change_level(char fromlevel, char tolevel, char rewrite_level);
void clear_level(struct level *dungeon_level);
void free_dungeon();
char *roomname(int index);
void lset(int x, int y, int stat);
void lreset(int x, int y, int stat);
int loc_statusp(int x, int y, int stat);

#endif
