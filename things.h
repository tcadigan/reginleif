#ifndef THINGS_H_
#define THINGS_H_

#include "types.h"

int willrust(int obj);
int wearing(char *name);
int wielding(stuff otype);
int reads(int obj);
int unknown(stuff otype);
int unidentified(stuff obj);
int point(int obj, int dir);
int hungry();
int takeoff();
int puton(int obj);
int drop(int obj);
int quaff(int obj);
int fainting();
int throw(int obj, int dir);
int wield(int obj);
int wear(int obj);
int have(stuff otype);
int havemult(stuff otype, char *name, int count);
int havefood(int n);
int havenamed(stuff otype, char *name);
int havewand(char *name);
int haveuseless();
int haveminus();
void addstuff(char ch , int row, int col);
void dumpstuff();
void deletestuff(int row, int col);
void initstufflist();
void display(char *s);
int pickident();
int prepareident(int obj, int iscroll);
int removering(int obj);

#endif
