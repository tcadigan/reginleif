#ifndef WEAPONS_H_
#define WEAPONS_H_

#include "rogue.h"

int missile(int ydelta, int xdelta);
int do_motion(struct object *obj, int ydelta, int xdelta);
int fall(struct linked_list *item, bool pr);
int init_weapon(struct object *weap, char type);
int hit_monster(int y, int x, struct object *obj);
char *num(int n1, int n2);
int wield();
int fallpos(struct coord *pos, struct coord *newpos, bool passages);

#endif
