/*
 * action.c: Handles the action matching and response
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1992-1993
 *
 * See the COPYRIGHT file
 */
#include "action.h"

#include "args.h"
#include "gb.h"
#include "key.h"
#include "option.h"
#include "str.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static Action *action_head = NULL;
static Action *action_tail = NULL;

void cmd_unaction(char *pat);
void cmd_listaction(char *args);
void action_update_index(void);
void cmd_clearaction(char *args);
Action *find_action(char *pat);

void cmd_action(char *args)
{
    Action *p;
    char *ptr;
    char *ptr2;
    char buf[MAXSIZ];
    int nooutput = false;
    int quiet = false;
    int notify = false;
    int active = true;
    int argval = 1;
    int edit = false;
    int activating = false;

    ptr = get_args(argval, 0);

    if (!*ptr) {
        cmd_listaction(NULL);
        strfree(ptr);

        return;
    }

    while (*ptr == '-') {
        if (!strcmp(ptr, "-nooutput")) {
            nooutput = true;
        } else if (!strcmp(ptr, "-edit")) {
            edit = true;
        } else if (!strcmp(ptr, "-quiet")) {
            quiet = true;
        } else if (!strcmp(ptr, "-notify")) {
            notify = true;
        } else if (!strcmp(ptr, "-active")) {
            active = true;
            activating = true;
        } else if (!strcmp(ptr, "-inactive")) {
            active = false;
            activating = true;
        } else {
            cmd_unaction(ptr + 1);
            strfree(ptr);

            return;
        }

        ++argval;
        strfree(ptr);
        ptr = get_args(argval, 0);
    }

    if (!strcmp(ptr, "edit")
        || !strcmp(ptr, "nooutput")
        || !strcmp(ptr, "quiet")
        || !strcmp(ptr, "notify")
        || !strcmp(ptr, "active")
        || !strcmp(ptr, "inactive")) {
        msg("-- Action: Invalid name ('%s') since it is an option.");
        strfree(ptr);

        return;
    } else if ((*ptr == '#') && !(edit || activating)) {
        msg("-- Action: can not start with %c due to their use in indexes.", *ptr);
        strfree(ptr);

        return;
    }

    p = find_action(ptr);
    ptr2 = get_args(argval + 1, 100);

    if (edit) {
        if (p) {
            sprintf(buf,
                    "action %s%s%s\"%s\" %s",
                    (p->nooutput ? "-nooutput " : ""),
                    (p->notify ? "-notify" : ""),
                    (p->quiet ? "-quiet" : ""),
                    p->pattern,
                    fstring(p->action));

            set_edit_buffer(buf);
        } else {
            msg("-- Action: '%s' not found.", ptr);
        }

        strfree(ptr);
        strfree(ptr2);

        return;
    } else if (activating && !*ptr2) {
        if (p) {
            p->active = active;
            msg("-- Action: '%s' %sactivated", ptr, (active ? "" : "de"));
        } else {
            msg("--Action: '%s' not found.", ptr);
        }

        strfree(ptr);
        strfree(ptr2);

        return;
    }

    if (!p) {
        p = malloc(sizeof(Action));

        if (!p) {
            msg("-- Could not allocate memory for action.");
            strfree(ptr);
            strfree(ptr2);

            return;
        }

        if (!action_head) {
            action_head = p;
            action_tail = action_head;
            p->next = NULL;
            p->prev = NULL;
        } else {
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
    action_match_suppress = true;

    msg("-- Action($%d): added %s%s%s'%s' = '%s'",
        p->indx,
        (p->nooutput ? "(nooutput) " : ""),
        (p->notify ? "(notify) " : ""),
        (p->quiet ? "(quiet) " : ""),
        p->pattern,
        p->action);

    action_match_suppress = false;
    sprintf(buf, "#%d", p->indx);
    add_assign("pid", buf);
}

/* Remove action node from list by name */
void cmd_unaction(char *pat)
{
    Action *p;

    p = find_action(pat);

    if (!p) {
        msg("-- Action %s was not defined.", pat);

        return;
    }

    /* Head of list? */
    if (!p->prev) {
        /* Not sole node, move head up one */
        if (p->next) {
            action_head = p->next;
            action_head->prev = (Action *)NULL;
        } else { /* Sole node */
            action_head = (Action *)NULL;
            action_tail = (Action *)NULL;
        }
    } else if (!p->next) { /* End of list */
        action_tail = p->prev;
        p->prev->next = (Action *)NULL;
    } else { /* Middle of list */
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

    if (!action_head) {
        msg("-- No actions defined.");

        return;
    }

    msg("-- Actions (Globbaly %s):",
        (options[ACTIONS / 32] & ((ACTIONS < 32) ?
                                  (1 << ACTIONS)
                                  : (1 << (ACTIONS % 32)))) ?
        "ACTIVE"
        : "INACTIVE");

    for (p = action_head; p; p = p->next) {
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

    if (!action_head) {
        return;
    }

    for (p = action_head; p; p = p->next) {
        p->indx = i++;
    }
}

void save_actions(FILE *fd)
{
    Action *p;

    if (!action_head) {
        return;
    }

    fprintf(fd, "\n#\n# Actions\n#\n#");

    for (p = action_head; p; p = p->next) {
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

    for (p = action_head; p; p = p->next) {
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

    if (*pat == '#') {
        val = atoi(pat + 1);
        p = action_head;

        while (p && (p->indx != val)) {
            p = p->next;
        }
    } else {
        p = action_head;

        while (p && strcmp(pat, p->pattern)) {
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
    int outval = false;

    if (!(options[ACTIONS / 32] & ((ACTIONS < 32) ?
                                   (1 << ACTIONS)
                                   : (1 << (ACTIONS % 32))))) {
        return false;
    }

    debug(1, "match: %s is string", s);

    for (ptr = action_head; ptr; ptr = ptr->next) {
        if (pattern_match(s, ptr->pattern, pattern) && ptr->active) {
            p = ptr->pattern;

            for (p = ptr->pattern; *p; ++p) {
                if (*p == '*') {
                    ++cnt;
                }
            }

            *buf = '\0';

            for (i = 0; i < cnt; ++i) {
                p = skip_space(pattern[i]);
                sprintf(temp, "\"%s\" ", p);
                strcat(buf, temp);
            }

            debug(1, "send to args(%d): %s", cnt, buf);
            p = parse_sec_args(ptr->action, buf);
            debug(1, "got back: %s", p);

            if (!ptr->quiet) {
                sprintf(temp, "-- Action(#%d) activated by: %-30s", ptr->indx, s);
                display_msg(temp);
            }

            if ((options[SHOW_ACTIONS / 32] & ((SHOW_ACTIONS < 32) ?
                                               (1 << SHOW_ACTIONS)
                                               : (1 << (SHOW_ACTIONS % 32))))
                || ptr->notify) {
                process_key(p, true);
            } else {
                process_key(p, false);
            }

            strfree(p);

            if (!outval) {
                outval = ptr->nooutput;
            }
        } /* if match */
    } /* for ptr */

    return outval;
}
