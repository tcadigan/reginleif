/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robery P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * makelist.c
 *
 * Galactic Bloodshed Planet List Consolidator.
 * makelist oldlist addlist > newlist
 * Errors are to stderr. Normal output to stdout.
 * This program takes a list of planets and alphabetizes them. Then takes the
 * addlist and tries to add unique planet names up to NUM_CHARS, if it can.
 * Otherwise it informs which name is being omitted and what names caused the
 * clash.
 *
 * Tue Apr 16 00:02:36 MDT 1991 Evan Koffler (ekoffler@du.edu)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * #include <strings.h>
 */

#define NUM_CHARS 4

typedef struct list {
    char name[257];
    struct list *next;
    struct list *prev;
} LIST;

LIST *list_head;

void add_to_list(char *s);
int print_list(void);

int main(int argc, char *argv[])
{
    FILE *input;
    FILE *add;
    char *c;
    char buf[BUFSIZE];
    int names;

    if (argc != 3) {
        printf("Usage: %s oldlist newlist > outputlist\n", argv[0]);

        exit(1);
    }

    input = fopen(argv[1], "r");

    if (input == NULL) {
        printf("Can not open %s for reading.\n", argv[1]);

        exit(1);
    }

    add = fopen(argv[2], "r");

    if (add == NULL) {
        printf("Can not open %s for reading.\n", argv[2]);

        exit(1);
    }

    if (fgets(buf, BUFSIZE, input)) {
        list_head = (LIST *)malloc(sizeof(LIS));
        list_head->prev = NULL;
        list_head->next = NULL;

        c = index(buf, '\n');

        if (c != NULL) {
            *c = '\0';
        }

        strcpy(list_head->name, buf);
    }

    while (fgets(buf, BUFSIZ, input)) {
        add_to_list(buf);
    }

    while (fgets(buf, BUFSIZ, add)) {
        add_to_list(buf);
    }

    names = print_list();
    fprintf(stderr, "Done with makelist. %d names total\n", names);

    return 0;
}

void add_to_list(char *s)
{
    LIST *p;
    LIST *q;
    int val;
    char *c;

    c = index(s, '\n');

    if (c) {
        *c = '\0';
    }

    if (*s == '\0') {
        return;
    }

    for (p = list_head; p; p = p->next) {
        val = strncmp(p->name, s, NUM_CHARS);

        if (val > 0) {
            q = (LIST *)malloc(sizeof(LIST));
            strcpy(q->name, s);

            if (p == list_head) {
                list_head = q;
                p->prev = q;
                q->next = p;
            } else {
                q->prev = p->prev;
                p->prev->next = q;
                q->next = p;
                p->prev = q;
            }

            return;
        } else if (val == 0) {
            fprintf(stderr,
                    "Duplicate name. In list %s. Omitting %s\n",
                    p->name,
                    s);

            return;
        }
    }

    if (!p) {
        p = list_head;

        while (p->next) {
            p = p->next;
        }

        p->next = (LIST *)malloc(sizeof(LIST));
        strcpy(p->name, s);
        p->next->prev = p;
        p->next->next = NULL;
    }
}

int print_list(void)
{
    LIST *p;
    int i = 0;

    for (p = list_head; p; p = p->next) {
        printf("%s\n", p->name);
        ++i;
    }

    return i;
}
