#ifndef OINV_H_
#define OINV_H_

#include "odefs.h"

void conform_lost_object(pob obj);
void conform_lost_objects(int n, pob obj);
void conform_unused_object(pob obj);
void givemonster(struct monster *m, struct object *o);
char *itemid(pob obj);
void dispose_lost_objects(int n, pob obj);
int find_item(pob *o, int id, int chargeval);
void gain_item(struct object *o);
void lose_all_items();
void p_drop_at(int x, int y, int n, pob o);
void drop_at(int x, int y, pob o);
void inventory_control();
void top_inventory_control();
void give_money(struct monster *m);
int cursed(pob obj);
void drop_money();
void pickup_at(int x, int y);
int getitem(char itype);

#endif
