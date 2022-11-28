#ifndef OCHAR_H_
#define OCHAR_H_

#include <stdio.h>

int calcmana();
void initplayer();
void initstats();
void omegan_character_stats();
void user_character_stats();
int competence_check(int attack);
void save_omegarc();
FILE *omegarc_check();
int fixnpc(int status);

#endif
