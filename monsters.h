#ifndef MONSTERS_H_
#define MONSTERS_H_

int seeawakemonster(char *monster);
int seemonster(char *monster);
void wakemonster(int dir);
void sleepmonster();
void dumpmonster();
void holdmonsters();
void addmonster(char ch, int r, int c, int quiescence);
void deletemonster(int r, int c);
char *monname(char m);
int monsternum(char *monster);
void rampage();
int isholder(char *monster);
void newmonsterlevel();

#endif
