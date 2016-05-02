#ifndef OINV_H_
#define OINV_H_

#include "odefs.h"

void conform_lost_object(pob obj);
void givemonster(struct monster *m, struct object *o);
char *itemid(pob obj);
void dispose_lost_objects(int n, pob obj);

#endif
