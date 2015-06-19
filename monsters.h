#ifndef MONSTERS_H_
#define MONSTERS_H_

int seemonster(char *monster);
void wakemonster(int dir);
void sleepmonster();
void dumpmonster();
void holdmonsters();
char *monname(char m);
int monsternum(char *monster);
void rampage();

#endif
