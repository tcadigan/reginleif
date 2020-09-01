#ifndef OSAVE_H_
#define OSAVE_H_

#include <stdio.h>

#include "odefs.h"

int save_game(int compress, char *savestr);
void save_country(FILE *fd);
void save_itemlist(FILE *fd, pol ol);
void save_item(FILE *fd, pob o);
void save_monsters(FILE *fd, pml ml);
void save_level(FILE *fd, plv level);
void save_player(FILE *fd);
int restore_game(char *savestr);
void restore_country(FILE *id);
void restore_monsters(FILE *id, plv level);
void restore_level(FILE *fd);
pol restore_itemlist(FILE *fd);
pob restore_item(FILE *fd);
void restore_player(FILE *fd);
void signalsave(int signum);

#endif
