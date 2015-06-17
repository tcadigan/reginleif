#ifndef IO_H_
#define IO_H_

#include <stdarg.h>
#include <stdio.h>

void dosnapshot();
void printsnap(FILE *f);
void getoldcommand(char *s);
void say(char *f, va_list args);
void saynow(char *f, ...);
void send(char *f, ...);
void dumpwalls();
void quitrogue(char *reason, int gld, int terminationtype);

#endif
