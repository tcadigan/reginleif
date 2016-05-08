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

#endif
