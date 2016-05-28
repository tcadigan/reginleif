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
void drawscreen();
void redraw();
void bufferprint();
void clear_if_necessary();
int move_slot(int oldslot, int newslot, int maxslot);
int getnumber(int range);
void mnumprint(int n);
void clearmsg1();
void clearmsg3();
void display_pack();
void display_possessions();
void display_win();
void display_bigwin();
void display_death(char *source);
void display_option_slot(int slot);
void display_options();
void display_quit();
void display_inventory_slot(int slotnum, int topline);
void deathprint();
void plotmon(struct monster *m);
void draw_explosion(char pyx, int x, int y);
void checkclear();
void erase_monster(struct monster *m);
void plotspot(int x, int y, int showmonster);
int stillonblock();

#endif
