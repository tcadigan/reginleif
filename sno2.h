#ifndef SNO2_H
#define SNO2_H

#include "sno.h"

struct node *compon(void);
struct node *nscomp(void);
struct node *push(struct node *stack);
struct node *pop(struct node *stack);
struct node *expr(struct node *start, int eof, struct node *e);
struct node *match(struct node *start, struct node *m);
struct node *compile(void);

#endif /* SNO2_H_ */
