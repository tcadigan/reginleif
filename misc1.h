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

#endif
