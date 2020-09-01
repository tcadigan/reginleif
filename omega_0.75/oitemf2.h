#ifndef OITEMF2_H_
#define OITEMF2_H_

#include "odefs.h"

void weapon_normal_hit(int dmgmod, pob o, struct monster *m);
void weapon_scythe(int dmgmod, pob o, struct monster *m);
void weapon_victrix(int dmgmod, pob o, struct monster *m);
void weapon_defend(int dmgmod, pob o, struct monster *m);
void weapon_firestar(int dmgmod, pob o, struct monster *m);
void weapon_desecrate(int dmgmod, pob o, struct monster *m);
void weapon_vorpal(int dmgmod, pob o, struct monster *m);
void weapon_mace_disrupt(int dmgmod, pob o, struct monster *m);
void weapon_lightsabre(int dmgmod, pob o, struct monster *m);
void weapon_demonblade(int dmgmod, pob o, struct monster *m);
void weapon_bolt(int dmgmod, pob o, struct monster *m);
void weapon_arrow(int dmgmod, pob o, struct monster *m);
void weapon_tangle(int dmgmod, pob o, struct monster *m);
void weapon_acidwhip(int dmgmod, pob o, struct monster *m);
void weapon_bare_hands(int dmgmod, struct monster *m);
void i_perm_deflect(pob o);
void i_normal_shield(pob o);
void i_victrix(pob o);
void i_defend(pob o);
void i_mace_disrupt(pob o);
void i_desecrate(pob o);
void i_demonblade(pob o);
void i_lightsabre(pob o);
void i_normal_weapon(pob o);
void i_normal_armor(pob o);
void i_perm_breathing(pob o);
void i_perm_energy_resist(pob o);
void i_perm_fear_resist(pob o);
void i_perm_regenerate(pob o);
void i_perm_knowledge(pob o);
void i_perm_poison_resist(pob o);
void i_perm_fire_resist(pob o);
void i_perm_gaze_immune(pob o);
void i_perm_strength(pob o);
void i_perm_burden(pob o);

#endif
