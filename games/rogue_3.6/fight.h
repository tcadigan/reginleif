#ifndef FIGHT_H_
#define FIGHT_H_

#include "rogue.h"

int fight(struct coord *mp, char mn, struct object *weap, bool thrown);
int attack(struct thing *mp);
int swing(int at_lvl, int op_arm, int wplus);
int check_level();
int roll_em(struct stats *att, struct stats *def, struct object *weap, bool hurl);
char *prname(char *who, bool upper);
int hit(char *er, char *ee);
int miss(char *er, char *ee);
int save_throw(int which, struct thing *tp);
int save(int which);
int str_plus(struct stats *stats);
int add_dam(struct stats *stats);
int raise_level();
int thunk(struct object *weap, char *mname);
int bounce(struct object *weap, char *mname);
int remove_monster(struct coord *mp, struct linked_list *item);
int is_magic(struct object *obj);
int killed(struct linked_list *item, bool pr);

#endif
