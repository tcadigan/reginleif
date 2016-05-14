#ifndef OMON_H_
#define OMON_H_

#include "odefs.h"

void m_dropstuff(struct monster *m);
void m_death(struct monster *m);
void m_damage(struct monster *m, int dmg, int dtype);
void m_status_reset(struct monster *m, int s);
void m_status_set(struct monster *m, int s);
int m_statusp(struct monster *m, int s);
void make_hiscore_npc(pmt npc, int npcid);
void monster_talk(struct monster *m);

#endif
