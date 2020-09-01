#ifndef LTM_H_
#define LTM_H_

#include "types.h"

void parsemonster(char *monster);
int findmonster(char *monster);
int addmonhist(char *monster);
void clearltm(ltmrec *ltmarr);
void readltm();
void saveltm(int score);
void analyzeltm();
void restoreltm();
void dumpmonstertable();
void mapcharacter(char ch, char *str);

#endif
