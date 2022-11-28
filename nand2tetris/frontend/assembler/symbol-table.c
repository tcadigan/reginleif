/* Keeps a correspondence between symbolic labels and numeric addresses. */
#include "symbol-table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    char *symbol;
    int address;
    struct node *prev;
    struct node *next;
};

struct node *head;
struct node *tail;

/* Creates a new empty symbol table */
void construct_symbol_table()
{
    head = NULL;
    tail = NULL;
}

void destroy_symbol_table()
{
    struct node *cur = head;
    struct node *next = NULL;

    while(cur != NULL) {
        if(cur->symbol) {
            free(cur->symbol);
        }

        next = cur->next;

        free(cur);

        cur = next;
    }
}

/* Adds the pair (symbol, address to the table). */
void add_entry(char *symbol, int address)
{
    struct node *n = (struct node *)malloc(sizeof(struct node));
    if(n == NULL) {
        fprintf(stderr, "Unable to allocate node");

        return;
    }

    n->symbol = (char *)malloc(strlen(symbol) + 1);

    if(n->symbol == NULL) {
        fprintf(stderr, "Unable to allocate space for symbol.");
        free(n);
        
        return;
    }

    strncpy(n->symbol, symbol, strlen(symbol));
    n->symbol[strlen(symbol)] = '\0';
    n->address = address;
    n->prev = NULL;
    n->next = NULL;
    
    if((head == NULL) && (tail == NULL)) {
        head = n;
        tail = n;
    }
    else {
        tail->next = n;
        n->prev = tail;
        tail = n;
    }
}

/* Does the symbol table contain the given symbol? */
int contains(char *symbol)
{
    struct node *n = head;

    while(n) {
        if(n->symbol) {
            if((strlen(n->symbol) == strlen(symbol))
               && (strncmp(n->symbol, symbol, strlen(symbol)) == 0)) {
                return 1;
            }
        }

        n = n->next;
    }

    return 0;
}

/* Returns the address associated with the symbol */
int get_address(char *symbol)
{
    struct node *n = head;
    
    while(n) {
        if(n->symbol) {
            if((strlen(n->symbol) == strlen(symbol))
               && (strncmp(n->symbol, symbol, strlen(symbol)) == 0)) {
                return n->address;
            }
        }

        n = n->next;
    }

    return -1;
}
