#ifndef SPELLS_H_
#define SPELLS_H_

void breath(int type, int y, int x, int dam_hp, char *ddesc);
void teleport_to(int ny, int nx);
int aggravate_monster(int dis_affect);
void lose_exp(int amount);
void teleport_away(int monptr, int dis);
int mass_genocide();
int ident_spell();
int remove_fear();
int cure_confusion();
int cure_blindness();
int cure_poison();
int remove_curse();

#endif
