#ifndef ARMS_H_
#define ARMS_H_

int havearmor(int k, int print, int rustproof);
int havebow(int k, int print);
int havering(int k, int print);
int haveweapon(int k, int print);
int armorclass(int i);
int bowclass(int i);
int weaponclass(int i);
int ringclass(int i);
int hitbonus(int strength);
int damagebonus(int strength);
void setbonuses();
void plusweapon();

#endif
