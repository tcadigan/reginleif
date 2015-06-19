#ifndef ROOMS_H_
#define ROOMS_H_

void markmissingrooms();
int markexplored(int row, int col);
int nextto(int type, int r, int c);
int mazedoor(int row, int col);
void dumpmazedoor();
int canbedoor(int deadr, int deadc);
int whichroom(int r, int c);
int darkroom();
int isexplored(int row, int col);
void printexplored();
int nexttowall(int r, int c);
void currentrectangle();
void newlevel();
void updatepos(char ch, int row, int col);
void updateat();
void mapinfer();
void inferhall();
void nametrap(int traptype, int standingonit);
void findstairs(int notr, int notc);

#endif
