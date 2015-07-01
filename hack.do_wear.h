#ifndef HACK_DO_WEAR_H_
#define HACK_DO_WEAR_H_

#include "def.obj.h" /* obj */

void corrode_armor();
int dowearring();
int doremring();
int doremarm();
int doweararm();
void ringoff(struct obj *otmp);
int armoroff(struct obj *otmp);
int cursed(struct obj *otmp);
int dorr(struct obj *otmp);
void find_ac();
void glibr();

#endif
