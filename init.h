#ifndef INIT_H_
#define INIT_H_

#include "rogue.h"

int init_player();
int init_things();
int init_colors();
int init_names();
int init_stones();
int init_materials();
int badcheck(char *name, struct object *magic, int bound);

#endif
