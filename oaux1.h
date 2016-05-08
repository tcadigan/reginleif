#ifndef OAUX1_H_
#define OAUX1_H_

#include "odefs.h"

void gain_experience(int amount);
void p_death(char *fromstring);
void p_damage(int dmg, int dtype, char *fromstring);
char *levelname(int level);
void fight_monster(struct monster *m);
int optionp(int o);
void calc_melee();
void damage_item(pob o);

#endif
