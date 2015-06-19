#ifndef EXPLORE_H_
#define EXPLORE_H_

int avoid();
void pinavoid();
int expinit();
void avoidmonsters();
int setpsd(int print);
int zigzagvalue(int r, int c, int depth, int *val, int *avd, int *cont);
void caddycorner(int r, int c, int d1, int d2, char ch);

#endif
