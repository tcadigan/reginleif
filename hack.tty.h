#ifndef HACK_TTY_H_
#define HACK_TTY_H_

#include "config.h" /* boolean */

char readchar();
void getret();
void setctty();
void setftty();
void setty(char *s);
char *parse();
void getlin(char *bufp);
void getty();
void cgetret();
void xwaitforspace(boolean spaceflag);

#endif
