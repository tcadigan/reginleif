#ifndef OAUX2_H_
#define OAUX2_H_

#include "odefs.h"

int true_item_value(pob item);
void gain_level();
void movecursor(int *x, int *y, int dx, int dy);
int p_immune(int dtype);
void p_fumble(int dtype);
void tacplayer(struct monster *m);
int statmod(int stat);
void change_environment(char new_environment);
int player_hit(int hitmod, char hitloc, struct monster *m);
int expval(int plevel);
void break_weapon();
void drop_weapon();
void torch_check();
void moon_check();

#endif
