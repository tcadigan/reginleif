#ifndef OPRIEST_H_
#define OPRIEST_H_

#include "odefs.h"

void l_altar();
void make_hp(pob o);
void hp_req_print();
void hp_req_test();
void answer_prayer();
void increase_priest_rank(int deity);
int check_sacrilege(int deity);

#endif
