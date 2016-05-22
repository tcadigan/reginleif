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
void merge_item(int slot);
pob split_item(int num, pob item);
int find_and_remove_item(int id, int chargeval);
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
void fixpack();
void pack_extra_items(pob item);
int objequal(struct object *o, struct object *p);
void switch_to_slot(int slot);
void drop_from_slot(int slot);
void put_to_pack(int slot);
int get_to_pack(pob o);
int get_inventory_slot();
int item_useable(pob o, int slot);
int slottable(pob o, int slot);
int badobject(char slotchar);
void setchargestr(pob obj, char *cstr);
void setnumstr(pob obj, char *nstr);
void setplustr(pob obj, char *pstr);
int get_money(int limit);
pob detach_money();
char *cashstr();

#endif
