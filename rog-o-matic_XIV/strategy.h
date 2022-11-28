#ifndef STRATEGY_H_
#define STRATEGY_H_

int quitforhonors();
int strategize();
int wanttowake(char c);
int battlestations(int m,
                   char *monster, 
                   int mbad,
                   int danger,
                   int mdir,
                   int mdist,
                   int alert,
                   int adj);
int pickupafter();
int archery();
int tostuff();
int aftermelee();
int dropjunk();
int fightinvisible();
int fightmonster();

#endif
