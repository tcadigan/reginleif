#ifndef GLOBAL_H_
#define GLOBAL_H_

void positionplayer();
int makemonst(int lev);
void recalc();
void raiseexperience(long x);
void raisehp(int x);
void raisemhp(int x);
void raisemspells(int x);
void raiselevel();
void loseexperience(long x);
void losemspells(int x);
void losehp(int x);
void losemhp(int x);
void more();
int take(int itm, int arg);
void adjustcvalues(int itm, int arg);
int drop_object(int k);
int packweight();
void quit();
int getpassword();
int stealsomething();
int emptyhanded();
void loselevel();
void creategem();
void enchantarmor();
void enchweapon();
int nearbymonst();

#endif
