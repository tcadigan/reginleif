#ifndef EXPLORE_H_
#define EXPLORE_H_

int avoid();
void pinavoid();
int expinit();
void avoidmonsters();
int setpsd(int print);
int zigzagvalue(int r, int c, int depth, int *val, int *avd, int *cont);
void caddycorner(int r, int c, int d1, int d2, char ch);
void unrest();
int gotowards(int r, int c, int running);
int archmonster(int m, int trns);
int doorexplore();
int exploreroom();
int movetorest();
int downvalue(int r, int c, int depth, int *val, int *avd, int *cont);
int sleepvalue(int r, int c, int depth, int *val, int *avd, int *cont);
int archeryvalue(int r, int c, int depth, int *val, int *avd, int *cont);
int archeryinit();
int genericinit();
int findsafe();

#endif
