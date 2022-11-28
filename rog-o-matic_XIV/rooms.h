#ifndef ROOMS_H_
#define ROOMS_H_

void markmissingrooms();
void markexplored(int row, int col);
void unmarkexplored(int row, int col);
int isexplored(int row, int col);
void printexplored();
int haveexplored(int n);
int nextto(int type, int r, int c);
int mazedoor(int row, int col);
void dumpmazedoor();
int canbedoor(int deadr, int deadc);
void connectdoors(int r1, int c1, int r2, int c2);
int whichroom(int r, int c);
int darkroom();
int lightroom();
int nexttowall(int r, int c);
void currentrectangle();
void newlevel();
void updatepos(char ch, int row, int col);
void updateat();
void mapinfer();
void inferhall();
void nametrap(int traptype, int standingonit);
void findstairs(int notr, int notc);
void teleport();
void foundnew();
int downright(int *drow, int *dcol);

#endif
