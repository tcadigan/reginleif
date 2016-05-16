#ifndef OAUX1_H_
#define OAUX1_H_

#include "odefs.h"

void gain_experience(int amount);
void p_death(char *fromstring);
void p_damage(int dmg, int dtype, char *fromstring);
int p_country_moveable(int x, int y);
int p_moveable(int x, int y);
char *levelname(int level);
void fight_monster(struct monster *m);
int optionp(int o);
void calc_melee();
void damage_item(pob o);
void foodcheck();
void roomcheck();
void optionset(int o);
void optionreset(int o);
int goberserk();
void threaten(struct monster *m);
void surrender(struct monster *m);
int getdir();
void searchat(int x, int y);
void tunnelcheck();
char *trapid(int trapno);
void describe_player();
char *mstatus_string(struct monster *m);
void setspot(int *x, int *y);

#endif
