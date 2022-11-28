#ifndef MESS_H_
#define MESS_H_

#include <stdio.h>

void parsemsg(char *mess, char *mend);
void summary(FILE *f, char sep);
void terpmes();
void versiondep();
int smatch(char *dat, char *pat, char **res);
void infer(char *objname);
void curseditem();
void mshit(char *monster);
void msmiss(char *monster);
void didhit();
void didmiss();
int getmonhist(char *monster, int hitormiss);
void countgold(char *amount);
void readident(char *name);

#endif
