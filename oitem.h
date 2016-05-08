#ifndef OITEM_H_
#define OITEM_H_

#include "odefs.h"

void item_use(struct object *o);
void make_artifact(pob new, int id);
pob create_object(int itemlevel);
char *ringname(int reset, int id);
char *cloakname(int reset, int id);
char *bootname(int reset, int id);
char *stickname(int reset, int id);
char *potionname(int reset, int id);
char *scrollname(int reset, int id);

#endif
