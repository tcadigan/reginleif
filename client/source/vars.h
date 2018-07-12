/*
 * vars.h: Contains a few generic changes for various systems and how they
 *         like to handle functions. (I.e. names and parameter listing).
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1991-1993
 *
 * See the COPYRIGHT file.
 */
#ifndef VARS_H_
#define VARS_H_

#include "ansi.h"
#include "types.h"

#include <stdbool.h>
#include <string.h>

#ifdef SYSV
#define TERMIO
#endif

typedef struct node {
    char *line;
    int type; /* For queueing. Do we wait for prompt */
    int indx;
    struct node *next;
    struct node *prev;
} Node;

#ifdef RWHO
extern struct rwhostruct rwho;
#endif

extern int detached;
extern struct statusstruct status;
extern int hide_prompt;
extern int racegen;
extern long boot_time;
extern long connect_time;
extern char gbrc_path[];
extern char *progname;
extern char *shell;
extern char race_name[];
extern char govn_name[];
extern char race_pass[];
extern char govn_pass[];
extern int password_failed;

#endif // VARS_H_
