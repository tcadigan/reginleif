#ifndef MORIA2_H_
#define MORIA2_H_

void multiply_monster(int y, int x, int z, int slp);
void check_mon_lite(int y, int x);
void delete_monster(int j);
int delete_object(int y, int x);
int find_range(int item1, int item2, int *j, int *k);
void openobject();
void look();
void jamdoor();
void throw_object();
void drop();
void closeobject();
void examine_book();
void tunnel(int y, int x);
void refill_lamp();
void disarm_trap();
void bash();
void go_down();
void go_up();
void teleport(int dis);
void move_char(int dir);
void desc_remain(int item_val);
void add_food(int num);
int cast_spell(char *prompt, int item_val, int *sn, int *sc, int *redraw);

#endif
