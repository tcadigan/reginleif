#ifndef MONSTERS_H_
#define MONSTERS_H_

#include "rogue.h"

char randmonster(bool wander);
int new_monster(struct linked_list *item, char type, struct coord *cp);
int wanderer();
struct linked_list *wake_monster(int y, int x);
int genocide();

#endif
