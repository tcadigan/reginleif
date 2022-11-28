#ifndef THINGS_H_
#define THINGS_H_

#include "rogue.h"

char *inv_name(struct object *obj, bool drop);
int money();
int drop();
int dropcheck(struct object *op);
struct linked_list *new_thing();
int pick_one(struct object *magic, int nitems);

#endif
