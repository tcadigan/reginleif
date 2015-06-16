#ifndef IO_H_
#define IO_H_

#include <stdarg.h>

void getoldcommand(char *s);
void say(char *f, va_list args);
void saynow(char *f, ...);
void send(char *f, ...);

#endif
