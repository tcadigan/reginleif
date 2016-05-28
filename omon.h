#ifndef OMON_H_
#define OMON_H_

#include "odefs.h"

void m_dropstuff(struct monster *m);
void m_death(struct monster *m);
void m_damage(struct monster *m, int dmg, int dtype);
void m_status_reset(struct monster *m, int s);
void m_status_set(struct monster *m, int s);
int m_statusp(struct monster *m, int s);
int m_immunityp(struct monster *m, int s);
void m_pickup(struct monster *m, struct object *o);
void m_no_op(struct monster *m);
void m_altar(struct monster *m);
void m_trap_abyss(struct monster *m);
void m_trap_acid(struct monster *m);
void m_trap_sleepgas(struct monster *m);
void m_trap_manadrain(struct monster *m);
void m_trap_disintegrate(struct monster *m);
void m_trap_teleport(struct monster *m);
void m_trap_fire(struct monster *m);
void m_trap_blade(struct monster *m);
void m_trap_snare(struct monster *m);
void m_trap_door(struct monster *m);
void m_trap_pit(struct monster *m);
void m_trap_dart(struct monster *m);
void m_abyss(struct monster *m);
void m_fire(struct monster *m);
void m_lava(struct monster *m);
void m_water(struct monster *m);
void make_hiscore_npc(pmt npc, int npcid);
void make_log_npc(struct monster *npc);
void monster_talk(struct monster *m);
char *mantype();
void movemonster(struct monster *m, int newx, int newy);
void strengthen_death(struct monster *m);
void determine_npc_behavior(pmt npc, int level, int behavior);
void monster_action(struct monster *m, int action);
void monster_special(struct monster *m);
void monster_strike(struct monster *m);
void monster_move(struct monster *m);

#endif
