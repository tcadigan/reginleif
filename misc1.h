#ifndef MISC1_H_
#define MISC1_H_

int randint(int maxval);
int randnor(int mean, int stand);
int test_light(int y, int x);
int distance(int y1, int x1, int y2, int x2);
int in_bounds(int y, int x);
int summon_undead(int *y, int *x);
int summon_monster(int *y, int *x, int slp);
int bit_pos(unsigned int *test);
int damroll(char *dice);
int los(int y1, int x1, int y2, int x2);
int randint(int maxval);
void set_seed(char *state, int seed);
void reset_seed();
int check_time();
void prt_map();
void regenmana(double percent);
void regenhp(double percent);
void alloc_monster(int (*alloc_set)(), int num, int dis, int slp);
void pusht(int x);
void popt(int *x);
void magic_treasure(int x, int level);
void loc_symbol(int y, int x, char *sym);
void mlink();
void tlink();
void place_win_monster();
int next_to8(int y, int x, int elem_a, int elem_b, int elem_c);
void price_adjust();
void init_t_level();
void init_m_level();
void init_seeds();
void sort_objects();

#endif
