#ifndef OSCR_H_
#define OSCR_H_

void mprint(char *s);
void locprint(char *s);
void putspot(int x, int y, char c);
char getspot(int x, int y, int showmonster);
char mcigetc();
char mgetc();
char lgetc();
void nprint1(char *s);
void nprint2(char *s);
char ynq2();
void levelrefresh();
void spreadroomlight(int x, int y, int roomno);
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

#endif
