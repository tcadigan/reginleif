#ifndef PACK_H_
#define PACK_H_

#include "rogue.h"

int add_pack(struct linked_list *item, bool silent);
int inventory(struct linked_list *list, int type);
int pick_up(char ch);
int picky_inven();
struct linked_list *get_item(char *purpose, int type);
char pack_char(struct object *obj);

#endif
