#ifndef MONSTER_H_
#define MONSTER_H_

void genmonst();
void sphboom(int x, int y);
int rmsphere(int x, int y);
int newsphere(int x, int y, int dir, int life);
void checkloss();
void dropgold(int amount);
void dropsomething(int monst);
void something(int level);
void createmonster(int mon);
void createitem(int it, int arg);
void hitmonster(int x, int y);
int newobject(int lev, int *i);
void cast();
int spattack(int x, int xx, int yy);
void omnidirect(int spnum, int dam, char *str);
void tdirect(int spnum);
void godirect(int spnum, int dam, char *str, int delay, char cshow);
void direct(int spnum, int dam, char *str, int arg);
void loseint();
int hitm(int x, int y, int amt);
void ifblind(int x, int y);
void dirpoly(int spnum);
int vxy(int *x, int *y);
void speldamage(int x);
int fullhit(int xx);
int cgood(int x, int y, int itm, int monst);
int newsphere(int x, int y, int dir, int life);
void hitplayer(int x, int y);
int annihilate();

#endif
