#ifndef CREATE_H_
#define CREATE_H_

void checkgen();
void eat(int xx, int yy);
void sethp(int flg);
void makeobject(int j);
void makemaze(int k);
void makeplayer();
void newcavelevel(int x);
int cannedlevel(int k);
void treasureroom(int lv);
void froom(int n, int arg, char itm);
void troom(int lv, int xsize, int ysize, int tx, int ty, int glyph);
void fillmroom(int n, int arf, char what);
int fillmonst(char what);

#endif
