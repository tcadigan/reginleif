#ifndef MONSTERS_H_
#define MONSTERS_H_

int seemonster(char *monster);
void wakemonster(int dir);
void sleepmonster();
void dumpmonster();
void holdmonsters();
void deletemonster(int r, int c);
char *monname(char m);
int monsternum(char *monster);
void rampage();
int isholder(char *monster);

#endif
