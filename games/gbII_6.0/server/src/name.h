#ifndef NAME_H_
#define NAME_H_

#include "vars.h"

void personal(int, int, char *);
void bless(int, int, int, int, orbitinfo *);
void insurgency(int, int, int, int, orbitinfo *);
void pay(int, int, int, int, orbitinfo *);
void give(int, int, int, int, orbitinfo *);
void page(int, int, int, int, orbitinfo *);
void send_message(int, int, int, int, orbitinfo *);
void read_messages(int, int, int, int, orbitinfo *);
void purge_messages(int, int, int, int, orbitinfo *);
void motto(int, int, int, char *);
void name(int, int, int, int, orbitinfo *);
int MostAPs(int, startype *);
void announce(int, int, char *, int, int);
char *garble_msg(char *, int, int, int);
void garble_chat(int, int, int, int, orbitinfo *);

#endif /* NAME_H_ */
