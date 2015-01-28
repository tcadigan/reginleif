#ifndef SNO4_H_
#define SNO4_H_

#include "sno.h"

struct node *and(struct node *ptr);
struct node *eval(struct node *e, int t);
struct node *doop(int op, struct node *arg1, struct node *arg2);
struct node *execute(struct node *e);
int assign(struct node *adr, struct node *val);

#endif /* SNO4_H_ */
