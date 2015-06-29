#ifndef HACK_EAT_H_
#define HACK_EAT_H_

#include "def.obj.h" /* obj */

void gethungry();
void lesshungry(int num);
int doeat();
void Meatdone();
int eatcorpse(struct obj *otmp);

#endif
