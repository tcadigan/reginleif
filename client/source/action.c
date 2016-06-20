/*
 * action.c: Handles the action matching and response
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1992-1993
 *
 * See the COPYRIGHT file.
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "gb.h"
#include "proto.h"
#include "str.h"
#include "types.h"
#include "vars.h"

static Action *action_head = NULL;
static Action *action_tail = NULL;

Action *find_action(char *pat);
void cmd_listaction(char *args);
void cmd_unaction(char *pat);
void action_update_index(void);

extern int fprintf(FILE *, const char *, ...);
extern int atoi(const char *);
extern char *strcat(char *, const char *);

/* Action ROUTINES */

void cmd_action(char *args)
{
    Action *p;
    char *ptr;
    char *ptr2;
    char buf[MAXSIZ];
    int nooutput = FALSE;
    int quiet = FALSE;
    int notify = FALSE;
    int active = TRUE;
    int argval = 1;
    int edit = FALSE;
    int activating = FALSE;

    ptr = get_args(argval, 0);

    if(!*ptr) {
        cmd_listaction(NULL);
        strfree(ptr);

        return;
    }

    while(*ptr == '-') {
        if(streq(ptr, "-nooutput")) {
            nooutput = TRUE;
        }
        else if(streq(ptr, "-edit")) {
            edit = TRUE;
        }
        else if(streq(ptr, "-quiet")) {
            quiet = TRUE;
        }
        else if(streq(ptr, "-notify")) {
            notify = TRUE;
        }
        else if(streq(ptr, "-active")) {
            active = TRUE;
            activating = TRUE;
        }
        else if(streq(ptr, "-inactive")) {
            active = FALSE;
            activating = TRUE;
        }
        else {
            cmd_unaction(ptr + 1);
            strfree(ptr);

            return;
        }

        ++argval;
        strfree(ptr);
        ptr = get_args(argval, 0);
    }

    if(streq(ptr, "edit")
       || streq(ptr, "nooutput")
       || streq(ptr, "quiet")
       || streq(ptr, "notify")
       || streq(ptr, "active")
       || streq(ptr, "inactive")) {
        msq("-- Action: Invalid name ('%s') since it is an option.");
        strfree(ptr);

        return;
    }
    else if((*ptr == '#') && !(edit || activating)) {
        msg("-- Action: can not start with %c due to their use in indexes.", *ptr);
        strfree(ptr);

        return;
    }

    p = find_action(ptr);
    ptr2 = get_args(argval + 1, 100);

    if(edit) {
        if(p) {
            sprintf(buf,
                    "action %s%s%s\"%s\" %s",
                    (p->nooutput ? "-nooutput " : ""),
                    (p->notify ? "-notify" : ""),
                    (p->quiet ? "-quiet" : ""),
                    p->pattern,
                    fstring(p->action));

            set_edit_buffer(buf);
        }
        else {
            msg("-- Action: '%s' not found.", ptr);
        }

        strfree(ptr);
        strfree(ptr2);

        return;
    }
    else if(activating && !*ptr2) {
        if(p) {
            p->active = active;
            msg("-- Action: '%s' %sactivated", ptr, (active ? "" : "de"));
        }
        else {
            msg("--Action: '%s' not found.", ptr);
        }

        strfree(ptr);
        strfree(ptr2);

        return;
    }

    if(!p) {
        p = (Action *)malloc(sizeof(Action));

        if(!p) {
            msg("-- Could not allocate memory for action.");
            strfree(ptr);
            strfree(ptr2);

            return;
        }

        if(!action_head) {
            action_head = p;
            action_tail = action_head;
            p->next = NULL;
            p->prev = NULL;
        }
        else {
            p->prev = action_tail;
            action_tail->next = p;
            p->next = NULL;
            action_tail = p;
        }
    }

    p->pattern = ptr;
    p->action = ptr2;
    p->nooutput = nooutput;
    p->notify = notify;
    p->quiet = quiet;
    p->active = active;

    action_update_index();
    action_match_suppress = TRUE;

    msg("-- Action($%d): added %s%s%s'%s' = '%s'",
        p->indx,
        (p->nooutput ? "(nooutput) " : ""),
        (p->notify ? "(notify) " : ""),
        (p->quiet ? "(quiet) " : ""),
        p->pattern,
        p->action);

    action_match_suppress = FALSE;
    sprintf(buf, "#%d", p->indx);
    add_assign("pid", buf);
}

/* Remove action node from list by name */
void cmd_unaction(char *pat)
{
    Action *p;

    p = find_action(pat);

    if(!p) {
        msg("-- Action %s was not defined.", pat);

        return;
    }

    /* Head of list? */
    if(!p->prev) {
        /* Not sole node, move head up one */
        if(p->next) {
            action_head = p->next;
            action_head->prev = (Action *)NULL;
        }
        else { /* Sole node */
            action_head = (Action *)NULL;
            action_tail = (Action *)NULL;
        }
    }
    else if(!p->next) { /* End of list */
        action_tail = p->prev;
        p->prev->next = (Action *)NULL;
    }
    else { /* Middle of list */
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }

    msg("-- Action %s removed.", pat);
    strfree(p->pattern);
    strfree(p->action);
    free(p);
    action_update_index();
}

/* List actions by name. */
void cmd_listaction(char *args)
{
    Action *p;
    int i = 1;

    if(!action_head) {
        msg("-- No actions defined.");

        return;
    }

    msg("-- Actions (Globbaly %s):",
        (GET_BIT(options, ACTIONS) ? "ACTIVE" : "INACTIVE"));

    for(p = action_head; p; p = p->next) {
        p->indx = i;

        msg("%3d) %s%s%s%s%s = %s",
            i++,
            (p->active ? "" : "(inactive) "),
            (p->nooutput ? "(nooutput) " : ""),
            (p->notify ? "(notify) " : ""),
            (p->quiet ? "(quiet) " : ""),
            p->pattern,
            p->action);
    }

    msg("-- End of action list.");
}

void action_update_index(void)
{
    Action *p;
    int i = 1;

    if(!action_head) {
        return;
    }

    for(p = action_head; p; p = p->next) {
        p->indx = i++;
    }
}

void save_actions(FILE *fd)
{
    Action *p;

    if(!action_head) {
        return;
    }

    fprintf(fd, "\n#\n# Actions\n#\n#");

    for(p = action_head; p; p = p->next) {
        fprintf(fd,
                "action %s%s%s%s\"%s\" %s\n",
                (p->active ? "" : "-inactive "),
                (p->nooutput ? "-nooutput " : ""),
                (p->notify ? "-notify " : ""),
                (p->quiet ? "-quiet " : ""),
                p->pattern,
                fstring(p->action));
    }
}

/* Erase all nodes from the list */
void cmd_clearaction(char *args)
{
    Action *p;

    for(p = action_head; p; p = p->next) {
        strfree(p->pattern);
        strfree(p->action);
        free(p);
    }

    action_head = NULL;
    msg("-- All actions cleared.");
}

/* Find the node in the list by name */
Action *find_action(char *pat)
{
    int val;
    Action *p;

    if(*pat == '#') {
        val = atoi(pat + 1);
        p = action_head;
        
        while(p && (p->indx != val)) {
            p = p->next;
        }
    }
    else {
        p = action_head;

        while(p && !streq(pat, p->pattern)) {
            p = p->next;
        }
    }

    return p;
}

/*****/

/*
 * Uses the pattern matcher so things are in pattern1-pattern10 and
 * can be accessed via $0 thru $9 and $*. What is easiest way to do
 * this? Concatonate the string and pass the format to parse_args and
 * 'fake' a macro? I think so.
 */
int handle_action_matches(char *s)
{
    Action *ptr;
    char temp[MAXSIZ];
    char buf[MAXSIZ];
    char *p;
    int cnt = 0;
    int i;
    int outval = FALSE;

    if(!GET_BIT(options, ACTIONS)) {
        return FALSE;
    }

    debug(1, "match: %s is string", s);

    for(ptr = action_head; ptr; ptr = ptr->next) {
        if(MATCH(s, ptr->pattern) && ptr->active) {
            p = ptr->pattern;

            for(p = ptr->pattern; *p; ++p) {
                if(*p == '*') {
                    ++cnt;
                }
            }

            *buf = '\0';

            for(i = 0; i < cnt; ++i) {
                p = skip_space(pattern[i]);
                sprintf(temp, "\"%s\" ", p);
                strcat(buf, temp);
            }

            debug(1, "send to args(%d): %s", cnt, buf);
            p = parse_sec_args(ptr->action, buf);
            debug(1, "got back: %s", p);

            if(!ptr->quiet) {
                sprintf(temp, "-- Action(#%d) activated by: %-30s", ptr->indx, s);
                display_msg(temp);
            }

            if(GET_BIT(options, SHOW_ACTIONS) || ptr->notify) {
                process_key(p, TRUE);
            }
            else {
                process_key(p, FALSE);
            }

            strfree(p);

            if(!outval) {
                outval = ptr->nooutput;
            }
        } /* if match */
    } /* for ptr */

    return outval;
}
