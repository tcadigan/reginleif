#ifndef IO_H_
#define IO_H_

#include <ncurses.h>
#include <stdarg.h>

int msg(char *fmt, ...);
int addmsg(char *fmt, ...);
int endmsg();
int doadd(char *fmt, va_list args);
int step_ok(char ch);
char readchar();
int status();
int wait_for(char ch);
int show_win(WINDOW *scr, char *message);

#endif
