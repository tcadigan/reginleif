#ifndef OSCR_H_
#define OSCR_H_

#include "odefs.h"

void mprint(char *s);
void locprint(char *s);
void putspot(int x, int y, char c);
char getspot(int x, int y, int showmonster);
void drawspot(int x, int y);
void dodrawspot(int x, int y);
char mcigetc();
char mgetc();
char lgetc();
void nprint1(char *s);
void nprint2(char *s);
void nprint3(char *s);
char ynq();
char ynq1();
char ynq2();
void levelrefresh();
void spreadroomlight(int x, int y, int roomno);
void spreadroomdark(int x, int y, int roomno);
void screencheck(int y);
void showcursor(int x, int y);
void endgraf();
void display_death(char *source);
void clearmsg();
void morewait();
void print1(char *s);
void print2(char *s);
void print3(char *s);
void dataprint();
void showflags();
void comwinprint();
void drawvision(int x, int y);
void show_screen();
void erase_level();
void timeprint();
void phaseprint();
void display_win();
void xredraw();
void dobackspace();
void maddch(char c);
void menuclear();
void menuprint(char *s);
void menunumprint(int n);
char menugetc();
void menuaddch(char c);
void initgraf();
char *msgscanstring();
int parsenum();
void commanderror();
void display_possessions();
void drawscreen();
void redraw();
void bufferprint();
void display_pack();
void clear_if_necessary();
void display_option_slot(int slot);
int move_slot(int oldslot, int newslot, int maxslot);
void display_options();
int getnumber(int range);
void mnumprint(int n);
void clearmsg1();
void display_bigwin();
void display_quit();
void deathprint();
void plotmon(struct monster *m);
void draw_explosion(char pyx, int x, int y);

#endif
