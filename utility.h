#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdio.h>

int stlmatch(char *small, char *big);
int filelength(char *f);
int fexists(char *fn);
void critical();
void uncritical();
void int_exit(void(*exitproc)());
void quit(int code, char *fmt, ...);
void reset_int();
int lock_file(char *lockfil, int maxtime);
void unlock_file(char *lokfil);
FILE *wopen(char *fname, char *mode);
char *getname();

#endif
