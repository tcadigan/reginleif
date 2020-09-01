#ifndef OGEN2_H_
#define OGEN2_H_

void make_country_screen(char terrain);
void make_stairs(int fromlevel);
void make_swamp();
void make_mountains();
void make_river();
void make_jungle();
void make_forest();
void make_road();
void make_plains();
void room_level();
void maze_level();
void maze_corridor(int fx, int fy, int tx, int ty, char rsi, char num);
void room_corridor(int fx, int fy, int tx, int ty, int baux);

#endif
