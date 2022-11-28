#ifndef REPLAY_H_
#define REPLAY_H_

#include <stdio.h>

struct levstruct {
    long pos;
    int level;
    int gold;
    int hp;
    int hpmax;
    int str;
    int strmax;
    int ac;
    int explev;
    int exp;
};

void positionreplay();
int findlevel(FILE *f, struct levstruct *lvpos, int *nmlev, int maxnum);
int findmatch(FILE *f, char *s);
void fillstruct(FILE *f, struct levstruct *lev);

#endif
