#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>

int dwait(int msgtype, char *f, ...);
void promptforflags();
void dumpflags(int r, int c);
void toggledebug();
int getscrpos(char *msg, int *r, int *c);
void timehistory(FILE *f, char sep);

#endif
