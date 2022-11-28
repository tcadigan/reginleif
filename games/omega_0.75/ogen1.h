#ifndef OGEN1_H_
#define OGEN1_H_

#include "odefs.h"

void change_level(char fromlevel, char tolevel, char rewrite_level);
void clear_level(struct level *dungeon_level);
void free_dungeon();
char *roomname(int index);
void lset(int x, int y, int stat);
void lreset(int x, int y, int stat);
int loc_statusp(int x, int y, int stat);
void install_specials();
void sewer_corridor(int x, int y, int dx, int dy, char locchar);
void sewer_level();
void cavern_level();
void install_traps();
void find_stairs(char fromlevel, char tolevel);
void corridor_crawl(int *fx, int *fy, int sx, int sy, int n, char loc, char rsi);
plv findlevel(struct level *dungeon, char levelnum);
void makedoor(int x, int y);
void build_room(int x, int y, int l, char rsi, int baux);
void straggle_corridor(int fx, int fy, int tx, int ty, char loc, char rsi);

#endif
