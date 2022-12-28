#ifndef TELE_H_
#define TELE_H_

#include "vars.h"

void purge(int, int, int, int, orbitinfo *);
void post(char const *, int);
void push_telegram_race(int, char const *);
void push_telegram(int, int, char const *);
void teleg_read(int, int);
void news_read(int, int, int);
void purge_telegrams(int, int);

#endif /* TELE_H_ */
