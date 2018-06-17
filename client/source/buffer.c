/*
 * buffer.c: Handles the buffering of output on a linked list
 *
 * Written by Evan D. Kofflet <evank@netcom.com>
 *
 * Copyright (c) 1993
 *
 * See the COPYRIGHT file
 */
#include "buffer.h"

#include "str.h"
#include "vars.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void add_buffer(BufferInfo *infoptr, char *str, int partial)
{
    char *q;
    Buffer *p;
    int len;

    /* Partial lines so append incoming data */
    if(infoptr->partial) {
        if(partial) {
            debug(2, "add_buffer (partial): adding to line. This line is: partial");
        }
        else {
            debug(2, "add_buffer (partial): adding to line. This line is: done");
        }

        len = strlen(infoptr->tail->buf) + strlen(str);
        q = (char *)malloc(len + 1);

        if(!q) {
            msg("-- Malloc: Could not allocate socket output!");

            return;
        }

        strcpy(q, infoptr->tail->buf);
        strcat(q, str);
        q[len] = '\0';
        strfree(infoptr->tail->buf);
        infoptr->tail->buf = q;

        /* Are we newline terminated yet? */
        infoptr->partial = partial;

        return;
    }

    /* New data coming in, set up linked list */
    p = (Buffer *)malloc(sizeof(Buffer));

    if(!p) {
        msg("-- Malloc: Could not allocate socket output!");

        return;
    }

    if(!infoptr->tail) {
        infoptr->tail = p;
        infoptr->head = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else {
        p->next = infoptr->tail;
        infoptr->tail->prev = p;
        p->prev = NULL;
        infoptr->tail = p;
    }

    ++info.lines_read;
    p->buf = maxstring(str);
    infoptr->partial = partial;

    if(partial) {
        debug(2, "add_buffer (newline): adding to line. This line is: partial");
    }
    else {
        debug(2, "add_buffer (newline): adding to line. This line is: done");
    }
}

char *remove_buffer(BufferInfo *infoptr)
{
    Buffer *p = infoptr->head;
    char *str;

    if(!infoptr->head) {
        return "";
    }

    /* Lop off the head node */
    if(p->prev) {
        p->prev->next = NULL;
    }

    /* One node, head/tail point to it */
    if(infoptr->head == infoptr->tail) {
        infoptr->tail = NULL;
        infoptr->head = infoptr->tail;
        infoptr->partial = false;
    }
    else {
        /* Else move head back one node */
        infoptr->head = p->prev;
    }

    debug(3, "remove_buffer: returning %s", p->buf);
    str = p->buf;
    free(p);

    return str;
}

/*
 * Determines if there is any nodes on the buffer that can be taken for
 * processing. A partial node is not valid since all output is newline
 * terminated.
 */
int have_buffer(BufferInfo *infoptr)
{
    static long last_check = -1;

    if(!infoptr) {
        return false;
    }

    if((infoptr->tail == infoptr->head) && infoptr->partial) {
        if(GET_BIT(options, PARTIAL_LINES)) {
            if(last_check == -1) {
                last_check = time(0);
            }
            else if(last_check < (time(0) /* - 1 */)) {
                infoptr->partial = false;
                infoptr->is_partial = true;
                last_check = -1;

                return true;
            }
        }

        return false;
    }

    if(infoptr->head) {
        last_check = -1;

        return true;
    }

    return false;
}
