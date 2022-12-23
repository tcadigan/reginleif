#ifndef DISPATCH_H_
#define DISPATCH_H_

#include "vars.h"

int count_dispatch(int, int);
int open_dispatch(int, int, mdbtype ***);
void read_dispatch(int, int, int);
void write_dispatch(int, int, int, mdbtype ***);
void send_dispatch(int, int, int, int, int, int, char const *);
void send_race_dispatch(int, int, int, int, int, char const *);
void delete_dispatch(int, int, int);
void purge_dispatch(int, int, int);
void check_dispatch(int, int);

#endif /* DISPATCH_H_ */
