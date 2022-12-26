#ifndef BUG_H_
#define BUG_H_

#include "../server/vars.h"

void bug(int, int, int, int, orbitinfo *);
int send_bug_email(char *);
void read_bug_report(int, int);

#endif /* BUG_H_ */
