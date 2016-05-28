#ifndef OAUX2_H_
#define OAUX2_H_

#include "odefs.h"

int item_value(pob item);
int true_item_value(pob item);
void gain_level();
void movecursor(int *x, int *y, int dx, int dy);
int p_immune(int dtype);
void p_fumble(int dtype);
void p_hit(struct monster *m, int dmg, int dtype);
void p_drown();
void tacplayer(struct monster *m);
int statmod(int stat);
void change_environment(char new_environment);
int player_hit(int hitmod, char hitloc, struct monster *m);
int expval(int plevel);
void break_weapon();
void drop_weapon();
void torch_check();
void moon_check();
void minute_status_check();
void weapon_use(int dmgmod, pob weapon, struct monster *m);
char *actionlocstr(char dir);
void toggle_item_use(int on);

#endif
