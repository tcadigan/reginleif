#ifndef HACK_END_H_
#define HACK_END_H_

#include "def.monst.h" /* monst */

#define NAMSZ 8
#define DTHSZ 40

struct toptenentry {
    struct toptenentry *tt_next;
    long int points;
    int level;
    int maxlvl;
    int hp;
    int maxhp;
    char plchar;
    char str[NAMSZ + 1];
    char death[DTHSZ + 1];
};

void done_in_by(struct monst *mtmp);
void done(char *st1);
void done1(int sig);
int done2();
char *eos(char *s);
char *itoa(int a);
void clearlocks();
int outentry(int rank, struct toptenentry *t1, int so);
void topten();
void outheader();
char *ordin(int n);
void prscore(int argc, char **argv);
void charcat(char *s, char c);

#endif
