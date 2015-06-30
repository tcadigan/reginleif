#ifndef HACK_TTY_H_
#define HACK_TTY_H_

char readchar();
void getret();
void setctty();
void setty(char *s);
char *parse();
void getlin(char *bufp);
void cgetret();
void echo(int n);

#endif
