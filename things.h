#ifndef THINGS_H_
#define THINGS_H_

#include "types.h"

int willrust(int obj);
int wearing(char *name);
int wielding(stuff otype);
int have(stuff otype);
int havemult(stuff otype, char *name, int count);
int havefood(int n);
int havenamed(stuff otype, char *name);
void dumpstuff();
void initstuff();
void deletestuff(int row, int col);

#endif
