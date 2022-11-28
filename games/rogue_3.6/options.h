#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "rogue.h"

int option();
int put_bool(bool *b);
int put_str(char *str);
int get_bool(bool *bp, WINDOW *win);
int get_str(char *opt, WINDOW *win);
int parse_opts(char *str);
int strucpy(char *s1, char *s2, int len);

#endif
