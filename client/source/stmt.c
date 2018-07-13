/*
 * stmt.c: Handles the more advanced command statements of the client
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1992-1993
 *
 * See the COPYRIGHT file.
 */
#include "stmt.h"

#include "args.h"
#include "gb.h"
#include "key.h"
#include "str.h"
#include "util.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct loopheadstruct {
    long low_time; /* Lowest time in stack */
    long last_time; /* Last time we were checked and done */
    Loop *head;
    Loop *tail;
} LoopHead;

LoopHead loop_info = { 1000000, 0, (Loop *)NULL, (Loop *)NULL };

Loop *find_loop(char *name);
int add_loop(char *cmd, long int timel, int user_defn);
int analyze_cndtl(char *s);
void cmd_listloop(char *args);
void loop_update_index(void);
void loop_update_time(void);
int remove_loop(char *cmd_name);

void cmd_loop(char *args)
{
    char timebuf[SMABUF];
    char restbuf[MAXSIZ];
    long timel;
    int resp;

    if (!*args || !strcmp(args, "-")) {
        cmd_listloop(args);

        return;
    }

    if (*args == '-') {
        resp = remove_loop(args + 1);

        if (resp == true) {
            msg("-- loop: %s remove.", args + 1);
        } else if (resp == 0) {
            msg("-- loop: %s not found.", args + 1);
        }

        return;
    }

    split(args, timebuf, restbuf);
    timel = atoi(timebuf);

    if ((timel == 0) && (*args == '0')) {
        msg("-- loop: please input an integer greater than 0");

        return;
    }

    if (timel <= 0) {
        msg("-- loop: time value must be greater than 0 seconds.");

        return;
    }

    resp = add_loop(restbuf, timel, true);

    if (resp == ERROR) {
        msg("-- loop: could not malloc the memory. loop not added.");
    } else if (resp >= 0) {
        msg("-- loop(#%d): added %s with delay %d seconds", resp, restbuf, timel);
        sprintf(restbuf, "#%d", resp);
        add_assign("pid", restbuf);
    }
}

int add_loop(char *cmd, long int timel, int user_defn)
{
    Loop *p;

    p = find_loop(cmd);

    if (p) {
        strfree(p->cmd);
        loop_update_time();
    } else {
        p = (Loop *)malloc(sizeof(Loop));

        if (!p) {
            return ERROR;
        }

        /* Empty list */
        if (loop_info.head == (Loop *)NULL) {
            loop_info.head = p;
            loop_info.tail = p;
            p->next = (Loop *)NULL;
            p->prev = (Loop *)NULL;
        } else {
            p->prev = loop_info.tail;
            loop_info.tail->next = p;
            p->next = NULL;
            loop_info.tail = p;
        }
    }

    p->time = timel;

    if (timel < loop_info.low_time) {
        loop_info.low_time = timel;
    }

    p->last_time = time(0);
    p->user_defn = user_defn;
    p->cmd = string(cmd);
    loop_update_index();

    return p->indx;
}

void cmd_removeloop(char *args)
{
    int resp;

    resp = remove_loop(args);

    if (resp == true) {
        msg("-- loop: %s removed.", args);
    } else if (resp == false) {
        msg("-- loop: %s not found.", args);
    }
}

/* Return 0 if not found, 1 if found and removed */
int remove_loop(char *cmd_name)
{
    Loop *p;
    int val;

    if (*cmd_name == '#') {
        val = atoi(cmd_name + 1);
        p = loop_info.head;

        while (p && (p->indx != val)) {
            p = p->next;
        }
    } else {
        p = find_loop(cmd_name);
    }

    if (!p) {
        return false;
    }

    /* Head of list? */
    if (!p->prev) {
        /* Not sole node */
        if (p->next) {
            loop_info.head = p->next;
            loop_info.head->prev = (Loop *)NULL;
        } else {
            /* Sole node */
            loop_info.head = (Loop *)NULL;
            loop_info.tail = (Loop *)NULL;
        }
    } else if (!p->next) {
        /* End of list */
        loop_info.tail = p->prev;
        p->prev->next = (Loop *)NULL;
    } else {
        /* Middle of list */
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }

    if (p->time == loop_info.low_time) {
        loop_update_time();
    }

    strfree(p->cmd);
    free(p);
    loop_update_index();

    return true;
}

void cmd_listloop(char *args)
{
    Loop *p = loop_info.head;
    int show_all = false;
    int cnt = 1;

    if (!strcmp(args, "-")) {
        show_all = true;
    }

    if (!loop_info.head) {
        msg("-- Loop list is empty.");

        return;
    }

    msg("-- Loop list:");

    while (p) {
        if (p->user_defn) {
            msg("%3d) \'$s\' (%d secs) %s",
                cnt,
                p->cmd,
                p->time,
                (p->user_defn ? "" : "client defined"));
        } else if (show_all)  {
            msg("%3d) \'%s\' (%d secs) %s",
                cnt,
                p->cmd,
                p->time,
                (p->user_defn ? "" : "client defined"));
        }

        p->indx = cnt++;
        p = p->next;
    }

    msg("-- loop list done.");
}

void loop_update_index(void)
{
    Loop *p;
    int cnt = 1;

    for (p = loop_info.head; p; p = p->next) {
        p->indx = cnt++;
    }
}

Loop *find_loop(char *name)
{
    Loop *p = loop_info.head;

    while (p && strcmp(name, p->cmd)) {
        p = p->next;
    }

    return p;
}

void handle_loop(void)
{
    long cur_time;
    Loop *p = loop_info.head;
    char temp[MAXSIZ];

    cur_time = time(0);

    if ((loop_info.last_time + (long)loop_info.low_time) > cur_time) {
        return;
    }

    loop_info.last_time = cur_time;

    while (p) {
        if ((p->last_time + (long)p->time) < cur_time) {
            strcpy(temp, p->cmd);
            process_key(temp, 0);
            p->last_time = cur_time;
        }

        p = p->next;
    }
}

void loop_update_time(void)
{
    Loop *q;
    long newlow = 10000000;

    for (q = loop_info.head; q; q = q->next) {
        if (q->time < newlow) {
            newlow = q->time;
        }
    }

    loop_info.low_time = newlow;
}

/* FOR stuff */
void cmd_for(char *args)
{
    char loopname[SMABUF]; /* Loop name - string form */
    char holdbuf[NORMSIZ]; /* Command section of the for */
    char val[MAXSIZ]; /* Buffer to build up things for add_queue */
    int firstval = 0; /* Low val */
    int lastval = -1; /* High val */
    int crement; /* Decrement or increment */
    int nooutput = false; /* Show or not on msg stuff */
    int i; /* Counter */
    char *p;
    char *q;
    char *r;

    p = first(args);
    r = args; /* Keep this, splitting string here */

    if (!*p) {
        msg("-- Usage: for loopname min,max command");
        msg("          for loopname (n1,n2,n3,...,nn) command");

        return;
    }
    else {
        if (!strcmp(p, "-nooutput")) {
            nooutput = true;
            q = rest(r);
            r = q; /* For down below when we get rest */
            p = first(q);
            debug(4, "for: nooutput is true, first is %s, rest is %s", p, r);

            if (!*p) {
                msg("-- Usage: for loopname min,max command");
                msg("          for loopname (n1,n2,n3,...,nn) command");

                return;
            }
        }

        strcpy(loopname, p);

        if (test_assign(loopname)) {
            msg("-- for: Requires an undefined loopname. Try a new name.");

            return;
        }

        if (!valid_assign_name(loopname)) {
            msg("-- for: Invalid loopname(%s). Must be char, num or _ only", loopname);

            return;
        }

        q = rest(r);

        if (!q) {
            *args = '\0';
        } else {
            strcpy(args, q);
        }
    }

    parse_variables(args);

    /* foreach() stuff */
    if (*args == '(') {
        p = strchr(args, ')'); /* Get end loop */

        if (!p) {
            msg("-- Usage: forloopname (n1,n2,n3,...,nn) command");

            return;
        }

        *p++ = '\0';
        strcpy(holdbuf, p);
        q = args + 1;
        p = strchr(q, ',');

        while (p) {
            *p = '\0';
            sprintf(val, "assign ^%s %s", loopname, q);
            add_queue(val, 1);
            sprintf(val, "msg \'%s\'", holdbuf);
            add_queue(val, 0);
            add_queue(holdbuf, 0);
            q = p + 1;
            p = strchr(q, ',');
        }

        sprintf(val, "assign ^%s %s", loopname, q);
        add_queue(val, 1);
        sprintf(val, "msg \'%s\'", holdbuf);
        add_queue(val, 0);
        add_queue(holdbuf, 0);
        sprintf(val, "assign -^%s", loopname);
        add_queue(val, 0);

        if (!nooutput) {
            add_queue("echo -- for done.", 0);
        }

        return;
    }

    /* Old for loop stuff below */
    p = first(args);

    if (!p || !*p) {
        msg("-- Usage: for loopname min,max command");
        msg("          for loopname (n1,n2,n3,...,nn) command");

        return;
    }

    q = strchr(p, ',');

    if (!q
        || (!isdigit(*(q + 1)) && *(q + 1) != '-')
        || (*(q + 1) == '-' && !isdigit(*(q + 2)))) {
        msg("-- Usage: for loopname min,max command");

        return;
    }

    *q = '\0';
    q++;
    firstval = atoi(p);
    lastval = atoi(q);

    if (lastval < firstval) {
        crement = -1;
    } else {
        crement = 1;
    }

    q = rest(args);

    if (!q) {
        msg("-- Usage: for loopname min,max command");

        return;
    } else {
        strcpy(holdbuf, q);
    }

    i = firstval;
    lastval += crement;

    while (i != lastval) {
        /* By pass the msg system with the ^ */
        sprintf(val, "assign ^%s %d", loopname, i);
        add_queue(val, 1);
        sprintf(val, "msg \'%s\'", holdbuf);
        add_queue(val, 0);
        add_queue(holdbuf, 0);
        i += crement;
    }

    sprintf(val, "assign -^%s", loopname);
    add_queue(val, 0);

    if (!nooutput) {
        add_queue("echo -- for done.", 0);
    }
}

/* IF stuff */
void cmd_if(char *args)
{
    char cndtl[SMABUF];
    char rest[MAXSIZ];
    int parencnt = 0;
    char *p;
    char *q;

    p = skip_space(args);

    if ((*p != '(') || (strchr(args, ')') == (char *)NULL)) {
        msg("-- Usage: if (conditional expr) command");

        return;
    }

    for (q = p; *q; ++q) {
        if (*q == '(') {
            ++parencnt;
        } else if (*q == ')') {
            --parencnt;

            if (!parencnt) {
                break;
            }
        }
    }

    *q = '\0';
    strcpy(cndtl, p + 1);
    ++q;
    strcpy(rest, q);
    debug(2, "if: cndtl is %s", cndtl);
    debug(2, "if: rest is %s", rest);

    if (analyze_cndtl(cndtl)) {
        msg("IF HAS OCCURED");
        add_queue(rest, 0);
    }
}

int analyze_cndtl(char *s)
{
    char *p;
    char *q;
    char left[SMABUF];
    char right[SMABUF];
    char oper[SMABUF];
    int lint;
    int rint;
    int oper_flag = -1;

    /*
     * 1 = ==
     * 2 >
     * 3 >=
     * 4 <
     * 5 <=
     * 6 != <>
     * 7 !        test for empty string
     */
    p = skip_space(s);

    if (pattern_match(p, "* = *", pattern)
        || pattern_match(p, "* == *", pattern)) {
        oper_flag = 1;
    } else if (pattern_match(p, "* > *", pattern)) {
        oper_flag = 2;
    } else if (pattern_match(p, "* >= *", pattern)) {
        oper_flag = 3;
    } else if (pattern_match(p, "* < *", pattern)) {
        oper_flag = 4;
    } else if (pattern_match(p, "* <= *", pattern)) {
        oper_flag = 5;
    } else if (pattern_match(p, "* != *", pattern)
               || pattern_match(p, "* <> *", pattern)) {
        oper_flag = 6;
    }

    if (!*pattern1) {
        *left = '\0';
    } else {
        strcpy(left, pattern1);
    }

    if (!*pattern2) {
        *right = '\0';
    } else {
        strcpy(right, pattern2);
    }

    p = skip_space(left);
    remove_space_at_end(left);

    if (*p == '$') {
        q = get_assign(p + 1);

        if (q) {
            strcpy(left, q);
        } else {
            *left = '\0';
        }
    } else if (*p == '\"') {
        ++p;
        q = strchr(p, '\"');

        if (q) {
            *q = '\0';
        } else {
            msg("Error...need an ending quote");

            return 0;
        }
    }

    strcpy(oper, p);
    strcpy(left, oper);
    p = skip_space(right);
    remove_space_at_end(right);

    if (*p == '$') {
        q = get_assign(p + 1);
        strcpy(right, q);
    } else if (*p == '\"') {
        ++p;
        q = strchr(p, '\"');

        if (q) {
            *q = '\0';
        } else {
            msg("Error...need an ending quote");

            return 0;
        }
    }

    strcpy(oper, p);
    strcpy(right, oper);
    debug(2, "left: \'%s\'", left);
    debug(2, "right: \'%s\'", right);

    switch (oper_flag) {
    case 1:
        if (!strcmp(left, right)) {
            return 1;
        }

        break;
    case 2:
        lint = atoi(left);
        rint = atoi(right);

        if (lint > rint) {
            return 1;
        }

        break;
    case 3:
        lint = atoi(left);
        rint = atoi(right);

        if (lint >= rint) {
            return 1;
        }

        break;
    case 4:
        lint = atoi(left);
        rint = atoi(right);

        if (lint < rint) {
            return 1;
        }

        break;
    case 5:
        lint = atoi(left);
        rint = atoi(right);

        if (lint <= rint) {
            return 1;
        }

        break;
    case 6:
        if (strcmp(left, right)) {
            return 1;
        }

        break;
    default:
        msg("-- if: unknown oper_flag %d report to author", oper_flag);

        break;
    }

    return 0;
}
