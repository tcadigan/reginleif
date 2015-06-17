#ifndef ROOMS_H_
#define ROOMS_H_

void markmissingrooms();
int markexplored(int row, int col);
int nextto(int type, int r, int c);
int mazedoor(int row, int col);
int canbedoor(int deadr, int deadc);
int whichroom(int r, int c);
int darkroom();
int isexplored(int row, int col);
int nexttowall(int r, int c);

#endif
