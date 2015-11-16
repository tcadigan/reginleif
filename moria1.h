#ifndef MORIA1_H_
#define MORIA1_H_

#include "types.h"

int movement_rate(int speed);
void move_rec(int y1, int x1, int y2, int x2);
void corrode_gas(char *kb_str);
void light_dam(int dam, char *kb_str);
void cold_dam(int dam, char *kb_str);
void acid_dam(int dam, char *kb_str);
void fire_dam(int dam, char *kb_str);
void py_bonuses(treasure_type tobj, int factor);
void take_hit(int damage, char *hit_from);
int test_hit(int bth, int level, int pth, int ac);
int panel_contains(int y, int x);
void search_on();
void search_off();
void rest();
void rest_off();
int no_light();
void lite_spot(int y, int x);
void unlite_spot(int y, int x);
void move_light(int y1, int x1, int y2, int x2);
void search(int y, int x, int chance);
int inven_command(char command, int r1, int r2);
int get_dir(char *prompt, int *dir, int *com_val, int *y, int *x);
int get_item(int *com_val, char *pmt, int *redraw, int i, int j);
void change_speed(int num);
void new_spot(int *y, int *x);

#endif
