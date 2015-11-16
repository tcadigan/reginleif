#ifndef MISC2_H_
#define MISC2_H_

#include "types.h"

int todis_adj();
int todam_adj();
int tohit_adj();
int toac_adj();
int con_adj();
int wis_adj();
int int_adj();
void prt_cmana();
void prt_wisdom();
void prt_intelligence();
void prt_constitution();
void prt_dexterity();
void prt_charisma();
void prt_experience();
void prt_stat_block();
void prt_poisoned();
void prt_afraid();
void prt_confused();
void prt_blind();
void prt_hunger();
void prt_level();
void prt_title();
void prt_mhp();
void prt_pac();
void prt_depth();
void prt_gold();
void prt_strength();
void prt_chp();
void get_name();
void put_misc1();
void put_misc2();
void put_misc3();
void put_stats();
void put_character();
int moria_move(int dir, int *y, int *x);
void inven_destroy(int item_val);
int player_saves(int adjust);
void insert_num(char *object_str, char *mtc_str, int number, int show_sign);
void insert_str(char *object_str, char *mtc_str, char *insert);
void alloc_object(int (*alloc_set)(), int typ, int num);
void change_name();
int check_paswd();
void draw_cave();
void cnv_stat(byteint stat, char *out_val);
int get_obj_num(int level);
void place_stairs(int typ, int num, int walls);
void place_door(int y, int x);
void place_object(int y, int x);
void place_down_stairs(int y, int x);
void place_up_stairs(int y, int x);
void place_locked_door(int y, int x);
void place_secret_door(int y, int x);
void place_trap(int y, int x, int typ, int subval);
void place_gold(int y, int x);
void random_object(int y, int x, int num);
byteint de_statt();
int learn_prayer();
int learn_spell(int *redraw);
void gain_mana(int amount);
void char_inven_init();
void bpswd();

#endif
