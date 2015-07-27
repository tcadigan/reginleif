#ifndef IO_H_
#define IO_H_

void cursors();
int lcreat(char *str);
int lprcat(char *str);
int lopen(char *str);
int lappend(char *str);
int larnint();
long lgetc();
void lwrite(char *buf, int len);
void lflush();
void lrfill(char *adr, int num);
void lwclose();
void lprintf(char *format, ...);
void lprint(long x);
void lrclose();
void cursor(int x, int y);
void scbr();
void set_score_output();
void cl_line(int x, int y);
void cl_up(int x, int y);

#endif
