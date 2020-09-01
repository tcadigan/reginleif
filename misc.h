#ifndef MISC_H_
#define MISC_H_

#include "rogue.h"

char *tr_name(char ch);
int look(bool wakeup);
char secretdoor(int y, int x);
struct linked_list *find_obj(int y, int x);
int eat();
int chg_str(int amt);
int add_haste(bool potion);
int aggravate();
char *vowelstr(char *str);
int is_current(struct object *obj);
int get_dir();

#endif
