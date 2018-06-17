/*
 * help.c: Handles the help for the client
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */

/*
 * The help command expects a specfic format. This format is as follows:
 *      1) Anything before the first line beginning with "-- " is ignored.
 *      2) Evertyhing after a "-- " is considered a topic that is to be checked
 *         for a match to display the contents.
 *      3) If a match occurs, then everything from the current "-- " up to the
 *         next line beginning with "-- " is displayed.
 *      4) Multiple levels are created by preceeding the command with a *.
 *
 * Example:
 * This is not diplayed.
 * -- first
 *   A line of help regarding first.
 * -- second
 *   A line of help regarding second.
 */
#include "help.h"

#include "str.h"
#include "types.h"

#include <stdbool.h>
#include <string.h>

/*
 * Various 'types' of help. Done and need to exit; one specific item
 * requested; all items requested; set help requested; and lastly bind
 * help requested
 */
enum help_type {
    HELP_DONE = -1,
    HELP_ONE = 0,
    HELP_ALL = 1,
    HELP_MULTI = 2
};

extern char pbuf[];
extern int num_columns;

void help(char *args, FILE *fdhelp)
{
    enum help_type mode = HELP_ONE; /* Searching for command (0)/all commands (1) */
    int found = false;    /* Find the entry */
    int multi = false;    /* Multi level command? */
    int oldmulti = false; /* Storage status variable */
    char *ptr;
    char multistr[200];   /* Store current multi level name */
    char buf[BUFSIZ];
    char temp[SMABUF];

    if(!*args || !args || (*args == '?')) {
        mode = HELP_ALL;
        msg("-- Client Help:");
        msg("Entries with further topics are marked by (*). Do not type the (*).");
    }

    set_column_maker(20);
    strcpy(multistr, "-=null=-");

    while(fgets(buf, BUFSIZ, fdhelp)) {
        if(*buf == '\n') {
            if(found) {
                msg("");
            }

            continue;
        }

        ptr = strchr(buf, '\n');

        if(ptr) {
            *ptr = '\0';
        }

        /* Topic */
        if (!strncmp(buf, "-- *", strlen("-- *"))
            || !strncmp(buf, "-- ", strlen("-- "))) {

            if (!strncmp(buf, "-- *", strlen("-- *"))) {
                multi = true;
            }

            if(found) {
                /*
                 * It's multi we've printed out multi info now we need
                 * to print out subtopics
                 */
                if (mode == HELP_MULTI) {
                    mode = HELP_DONE;
                    found = false;
                }
                else {
                    /* We are all done, so quit */
                    break;
                }
            }

            /* Find what we are looking for? */
            if (mode == HELP_ONE) {
                /* Find the multi we are looking for? */
                if (multi && !strcmp(buf + strlen("-- *"), args)) {
                    found = true;
                    mode = HELP_MULTI;
                    sprintf(multistr, "-- %s", buf + strlen("-- *"));
                    msg("");
                } else if (!strcmp(buf + strlen("-- "), args)) {
                    found = true;
                    multi = false;
                    msg("");
                }
            }
            else {
                /* We've printed all the multi there are */
                if (mode == HELP_DONE) {
                    if (!strncmp(buf, multistr, strlen(multistr))) {
                        found = true;

                        break;
                    }
                    else {
                        do_column_maker(buf + strlen(multistr));
                    }
                }

                /* Print specials (multi) only once */
                if (mode == HELP_ALL) {
                    if(oldmulti) {
                        if (!strncmp(buf + strlen("-- "), multistr, strlen(multistr))) {
                            continue;
                        }

                        oldmulti = false;
                    }

                    if(multi) {
                        if (!strncmp(buf + strlen("-- "), multistr, strlen(multistr))) {
                            continue;
                        }
                        else {
                            sprintf(temp, "%s (*)", buf + strlen("-- *"));
                            do_column_maker(temp);
                            strcpy(multistr, buf + strlen("-- *"));
                            oldmulti = true;
                            multi = false;
                        }
                    }
                    else {
                        do_column_maker(buf + strlen("-- "));
                    }
                }
            } /* Special indent */
        }
        else {
            /* Not a topic line */
            if(found) {
                /* We found the entry print to end */
                msg("%s", buf);
            }
        }
    }

    if(mode) {
        flush_column_maker();
    }

    fclose(fdhelp);

    if(!found && !mode) {
        msg("-- No help available on that topic");
        msg("-- type: helpc for a list of client commands");
        msg("-- type: helps for a list of server info");
        msg("-- type: help to check the server help files");

        return;
    }

    msg("-- end of help.");
}
