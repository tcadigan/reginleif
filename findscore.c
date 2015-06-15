/*
 * findscore.c: Rog-O-Matic XIV (CMU) Sun Jul 6 20:13:19 1986 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * Read the Rogue scoreboard to determine a goal score.
 *
 * EDITLOG
 * LastEditDate = Sun Jul 6 20:13:19 1986 - Michael Mauldin
 * LastFileName = /usre3/mlm/src/rog/ver14/findscore.c
 *
 * HISTORY
 * 6-Jul-86 Michael Mauldin (mlm) at Carnegie-Mellon University
 *     Created.
 */
#include "findscore.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "install.h"
#include "utility.h"

#define TEMPFL "/tmp/RscoreXXXXXX"
#define ISDIGIT(c) (((c) >= '0') && ((c) <= '9'))

int findscore(char *rogue, char *roguename)
{
    int score;
    int best = 1;
    char cmd[100];
    char buffer[BUFSIZ];
    char *s;
    char *tmpfname = TEMPFL;
    FILE *tmpfil;

    /* Run 'rogue -s', and put the scores into a temp file */
    sprintf(cmd, "%s -s > %s", rogue, mktemp(tmpfname));
    system(cmd);

    /* If no temp file created, return default score */
    tmpfil = fopen(tmpfname, "r");
    if(tmpfil == NULL) {
        return best;
    }

    /* Skip to the line starting with 'Rank...'. */
    while(fgets(buffer, BUFSIZ, tmpfil) != NULL) {
        if(stlmatch(buffer, "Rank")) {
            break;
        }
    }

    if(!feof(tmpfil)) {
        while(fgets(buffer, BUFSIZ, tmpfil) != NULL) {
            /* point s at buffer */
            s = buffer;

            /* Skip over rank */
            while(ISDIGIT(*s)) {
                ++s;
            }

            /* Skip to score */
            while((*s == ' ') || (*s == '\t')) {
                ++s;
            }

            /* Read score */
            score = atoi(s);

            /* Skip over score */
            while(ISDIGIT(*s)) {
                ++s;
            }

            /* Skip to player */
            while((*s == ' ') || (*s == '\t')) {
                ++s;
            }

            /* Found our hero's name */
            if(stlmatch(s, roguename)) {
                /* Rogy is on top! */
                if(best < 0) {
                    best = score;
                }

                /* 'best' is now target */
                break;
            }
        }
    }

    unlink(tmpfname);

    /* Don't quit for very small scores, it's not worth it */
    if(best < 2000) {
        best = -1;
    }

    return best;
}
