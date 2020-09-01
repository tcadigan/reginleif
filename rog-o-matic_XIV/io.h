#ifndef IO_H_
#define IO_H_

#include <stdarg.h>
#include <stdio.h>

void at(int r, int c);
void dosnapshot();
void printsnap(FILE *f);
void getoldcommand(char *s);
void say(char *f, va_list args);
void saynow(char *f, ...);
void send(char *f, ...);
void sendnow(char *f, ...);
void sendcnow(char c);
void terpbot();
void toggleecho();
void clearscreen();
void clearsendqueue();
void waitfor(char *mess);
void waitforspace();
void dumpwalls();
void quitrogue(char *reason, int gld, int terminationtype);
void pauserogue();
void deadrogue();
void redrawscreen();
void startreplay(FILE **logf, char *logfname);
void getrogver();
void getrogue(char *waitstr, int onat);
int resend();
void givehelp();
int charsavail();
char *statusline();
char getroguetoken();
char getlogtoken();
int pending();

#endif
