#ifndef IO_H_
#define IO_H_

#include <ncurses.h>

int msg(char *fmt, void *args);
int addmsg(char *fmt, void *args);
int endmsg();
int doadd(char *fmt, void *args);
int step_ok(char ch);
char readchar();
int status();
int wait_for(char ch);
int show_win(WINDOW *scr, char *message);

#endif
