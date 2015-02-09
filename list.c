// Functions for dealing with linked lists of goodies
//
// @(#)list.c 3.3 (Berkeley) 6/15/81

#include "list.h"

#include "io.h"
#include "main.h"

// _detach:
//     Takes an item out of whatever linked list it might be in
int _detach(struct linked_list **list, struct linked_list *item)
{
    if(*list == item) {
	*list = item->l_next;
    }
    if(item->l_prev != NULL) {
        item->l_prev->l_next = item->l_next;
    }
    if(item->l_next != NULL) {
        item->l_next->l_prev = item->l_prev;
    }
    item->l_next = NULL;
    item->l_prev = NULL;

    return 0;
}

// _attach:
//     Add an item to the head of a list
int _attach(struct linked_list **list, struct linked_list *item)
{
    if(*list != NULL) {
	item->l_next = *list;
	(*list)->l_prev = item;
	item->l_prev = NULL;
    }
    else {
	item->l_next = NULL;
	item->l_prev = NULL;
    }

    *list = item;

    return 0;
}

// _free_list:
//     Throw the whole blamed thing away
int _free_list(struct linked_list **ptr)
{
    struct linked_list *item;

    while(*ptr != NULL) {
	item = *ptr;
	*ptr = item->l_next;
	discard(item);
    }

    return 0;
}

// discard:
//     Free up an item
int discard(struct linked_list *item)
{
    total -= 2;
    free(item->l_data);
    free(item);

    return 0;
}

// new_item:
//     Get a new item with the specified size
struct linked_list *new_item(int size)
{
    struct linked_list *item;

    item = (struct linked_list *)new(sizeof(*item));
    if(item == NULL) {
	msg("Ran out of memory for header after %d items", total);
    }

    item->l_data = new(size);
    if(item->l_data == NULL) {
	msg("Ran out of memory for data after %d items", total);
    }
    item->l_next = item->l_prev = NULL;
    
    return item;
}

// new:
//     Something...
char *new(int size)
{
    char *space = malloc((unsigned int)size);

    if(space == NULL) {
	sprintf(prbuf, "Rogue ran out of memory (%p).  Fatal error!", sbrk(0));
        fatal(prbuf);
    }
    
    ++total;
    
    return space;
}
