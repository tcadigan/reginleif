#ifndef OEFFECT1_H_
#define OEFFECT1_H_

void manastorm(int x, int y, int dmg);
void bless(int blessing);
void enchant(int delta);
void acquire(int blessing);
void identify(int blessing);
void wish(int blessing);
void heal(int amount);
void fball(int fx, int fy, int tx, int ty, int dmg);
void lball(int fx, int fy, int tx, int ty, int dmg);
void snowball(int fx, int fy, int tx, int ty, int dmg);
void ball(int fx, int fy, int tx, int ty, int dmg, int dtype);
void fbolt(int fx, int fy, int tx, int ty, int hit, int dmg);
void lbolt(int fx, int fy, int tx, int ty, int hit, int dmg);
void nbolt(int fx, int fy, int tx, int ty, int hit, int dmg);
void bolt(int fx, int fy, int tx, int ty, int hit, int dmg, int dtype);
int random_item();
void objdet(int blessing);
void mondet(int blessing);
void apport(int blessing);

#endif
