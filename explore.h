#ifndef EXPLORE_H_
#define EXPLORE_H_

int avoid();
void pinavoid();
int expinit();
int gotoinit();
int restinit();
int expruninit();
int archeryinit();
int genericinit();
int runinit();
int unpininit();
int expunpininit();
int rundoorinit();
void avoidmonsters();
int setpsd(int print);
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
int runvalue(int r, int c, int depth, int *val, int *avd, int *cont);
int gotovalue(int r, int c, int depth, int *val, int *avd, int *cont);
int restvalue(int r, int c, int depth, int *val, int *avd, int *cont);
int zigzagvalue(int r, int c, int depth, int *val, int *avd, int *cont);
int exprunvalue(int r, int c, int depth, int *val, int *avd, int *cont);
int expunpinvalue(int r, int c, int depth, int *val, int *avd, int *cont);
int rundoorvalue(int r, int c,int depth, int *val, int *avd, int *cont);
int findsafe();

#endif
