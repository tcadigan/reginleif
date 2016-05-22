#ifndef OITEM_H_
#define OITEM_H_

#include "odefs.h"

void item_use(struct object *o);
pob create_object(int itemlevel);
char *ringname(int reset, int id);
char *cloakname(int reset, int id);
char *bootname(int reset, int id);
char *stickname(int reset, int id);
char *potionname(int reset, int id);
char *scrollname(int reset, int id);
char *grotname();
void make_artifact(pob new, int id);
void make_thing(pob new, int id);
void make_food(pob new, int id);
void make_cloak(pob new, int id);
void make_boots(pob new, int id);
void make_weapon(pob new, int id);
void make_shield(pob new, int id);
void make_armor(pob new, int id);
void make_stick(pob new, int id);
void make_ring(pob new, int id);
void make_scroll(pob new, int id);
void make_potion(pob new, int id);
void make_cash(pob new, int level);
int twohandedp(int id);
int itemcharge();
int itemplus();
int itemblessing();

#endif
