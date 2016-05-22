#ifndef OEFFECT3_H_
#define OEFFECT3_H_

void annihilate(int blessing);
void level_return();
void level_drain(int levels, char *source);
void strategic_teleport(int blessing);
void summon(int blessing, int id);
void dispel(int blessing);
void cleanse(int blessing);
int itemlist(int itemindex, int num);
void sleep_player(int amount);
void disease(int amount);
int monsterlist();
void learnspell(int blessing);
void illuminate(int blessing);
void levitate(int blessing);
void hero(int blessing);
void cure(int blessing);
void p_poison(int toxicity);
void p_teleport(int type);
void deflection(int blessing);
void clairvoyance(int vision);
void polymorph(int blessing);
void sleep_monster(int blessing);
void amnesia();
void hide(int x, int y);
void disintegrate(int x, int y);
void inflict_fear(int x, int y);
void disrupt(int x, int y, int amount);
void drain_life(int amount);

#endif
