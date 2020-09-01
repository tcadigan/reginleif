#ifndef HACK_TERMCAP_H_
#define HACK_TERMCAP_H_

void curs(int x, int y);
void standoutbeg();
void standoutend();
void home();
void cl_end();
void startup();
void hack_clear_screen();
void xputs(char *s);
void cmov(int x, int y);
void nocmov(int x, int y);
void backsp();
void hack_bell();
void hack_clear_screen();

#endif
