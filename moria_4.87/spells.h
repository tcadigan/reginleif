#ifndef SPELLS_H_
#define SPELLS_H_

#include "types.h"

void breath(int typ, int y, int x, int dam_hp, char *ddesc);
void teleport_to(int ny, int nx);
int aggravate_monster(int dis_affect);
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
void lose_exp(int amount);
int lose_con();
int lose_chr();
int lose_dex();
int lose_wis();
int lose_int();
int lose_str();
int light_area(int y, int x);
int unlight_area(int y, int x);
int destroy_area(int y, int x);
int recharge(int num);
int create_food();
int wall_to_mud(int dir, int y, int x);
int teleport_monster(int dir, int y, int x);
int speed_monster(int dir, int y, int x, int spd);
int speed_monsters(int spd);
int poly_monster(int dir, int y, int x);
int confuse_monster(int dir, int y, int x);
int sleep_monster(int dir, int y, int x);
int sleep_monsters1(int y, int x);
int sleep_monsters2();
int detect_monsters();
int td_destroy();
void fire_ball(int typ, int dir, int y, int x, int dam_hp, ctype descrip);
void fire_bolt(int typ, int dir, int y, int x, int dam, ctype bolt_typ);
int detect_trap();
int detect_sdoor();
int detect_evil();
int detect_invisible();
int detect_object();
int detect_treasure();
int slow_poison();
void detect_inv2(int amount);
int restore_level();
int warding_glyph();
int dispell_creature(int cflag, int damage);
int turn_undead();
int map_area();
int earthquake();
int protect_evil();
int bless(int amount);
int door_creation();
int trap_creation();
int enchant(worlint *plusses);
int mass_poly();
int starlite(int y, int x);
int disarm_all(int dir, int y, int x);
int clone_monster(int dir, int y, int x);
int build_wall(int dir, int y, int x);
int drain_life(int dir, int y, int x);
int td_destroy2(int dir, int y, int x);
int hp_monster(int dir, int y, int x, int dam);
void light_line(int dir, int y, int x);

#endif
