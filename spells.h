#ifndef SPELLS_H_
#define SPELLS_H_

#include "types.h"

void breath(int type, int y, int x, int dam_hp, char *ddesc);
void teleport_to(int ny, int nx);
int aggravate_monster(int dis_affect);
void lose_exp(int amount);
void teleport_away(int monptr, int dis);
int genocide();
int mass_genocide();
int ident_spell();
int remove_fear();
int cure_confusion();
int cure_blindness();
int cure_poison();
int remove_curse();
int hp_player(int num, char *kind);
int lose_chr();
int lose_dex();
int lose_wis();
int lose_int();
int lose_con();
int lose_str();
int light_area(int y, int x);
int destroy_area(int y, int x);
int recharge(int num);
int create_food();
int wall_to_mud(int dir, int y, int x);
int teleport_monster(int dir, int y, int x);
int speed_monster(int dir, int y, int x, int spd);
int poly_monster(int dir, int y, int x);
int confuse_monster(int dir, int y, int x);
int sleep_monster(int dir, int y, int x);
int sleep_monsters1(int y, int x);
int sleep_monsters2();
int detect_monsters();
int td_destroy();
int fire_ball(int typ, int dir, int y, int x, int dam_hp, ctype descrip);
int fire_bolt(int typ, int dir, int y, int x, int dam, ctype bolt_typ);
int detect_trap();
int detect_sdoor();

#endif
