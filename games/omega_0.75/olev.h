#ifndef OLEV_H_
#define OLEV_H_

#include "odefs.h"

int difficulty();
void wandercheck();
pmt m_create(int x, int y, int kind, int level);
void make_country_monsters(char terrain);
void make_site_monster(int i, int j, int mid);
void make_site_treasure(int i, int j, int itemlevel);
void make_specific_treasure(int i, int j, int itemid);
pmt make_creature(int mid);
void stock_level();
void populate_level(int monstertype);

#endif
