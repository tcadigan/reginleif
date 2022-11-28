#ifndef LIST_H_
#define LIST_H_

#include "rogue.h"

int _detach(struct linked_list **list, struct linked_list *item);
int _attach(struct linked_list **list, struct linked_list *item);
int _free_list(struct linked_list **ptr);
int discard(struct linked_list *item);
struct linked_list *new_item(int size);
char *new(int size);

#endif
