#ifndef IO_H_
#define IO_H_

int lcreat(char *str);
int lprcat(char *str);
int lopen(char *str);
void lwclose();
void lprintf(char *format, ...);
long lgetc();
void scbr();
void lrclose();

#endif
