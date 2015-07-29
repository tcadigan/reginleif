#ifndef IO_H_
#define IO_H_

#include <stdio.h>

void cursors();
void lprcat(char *str);
FILE *lcreat(char *str);
FILE *lopen(char *str);
FILE *lappend(char *str);
int larnint();
long lgetc();
void lwrite(char *buf, int len);
void lflush();
void lrfill(char *adr, int num);
void lwclose();
void lprintf(char *format, ...);
void lprint(long x);
void lrclose();
char *lgetl();
void setupvt100();
void newgame();
void cursor(int x, int y);
void scbr();
void sncbr();
void set_score_output();
void cl_line(int x, int y);
void cl_up(int x, int y);
void cl_dn(int x, int y);

#endif
