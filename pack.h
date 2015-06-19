#ifndef PACK_H_
#define PACK_H_

#include <stdio.h>

void dumpinv(FILE *f);
void deleteinv(int pos);
void removeinv(int pos);
void resetinv();
void doresetinv();
int inventory(char *msgstart, char *msgsend);
void killed(char *monster);
void washit(char *monster);
void wasmissed(char *monster);

#endif
