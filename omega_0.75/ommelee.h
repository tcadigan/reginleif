#ifndef OMMELEE_H_
#define OMMELEE_H_

#include "odefs.h"

void transcribe_monster_actions(struct monster *m);
char random_loc();
int monster_hit(struct monster *m, char hitloc, int bonus);
void monster_melee(struct monster *m, char hitloc, int bonus);
void tacmonster(struct monster *m);

#endif
