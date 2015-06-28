/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

/* For time_t */
#include <sys/types.h>
#include <stdio.h>
#include "hack.h"

#ifdef BSD
#include <sys/time.h>
#else
#include <time.h>
#endif

extern char plname[];
extern char *rip[] = {
    "                       ----------                      ",
    "                      /          \\                    ",
    "                     /    REST    \\                   ",
    "                    /      IN      \\                  ",
    "                   /     PEACE      \\                 ",
    "                  /                  \\                ",
    "                  |                  |                 ",
    "                  |                  |                 ",
    "                  |                  |                 ",
    "                  |                  |                 ",
    "                  |                  |                 ",
    "                  |       1001       |                 ",
    "                 *|     *  *  *      | *               ",
    "        _________)/\\\\_//(\\/(/\\)/\\//\\/|_)_______\n",
0,
};

void outrip()
{
    char **dp = rip;
    char *dpx;
    struct tm *lt;
    time_t date;
    char buf[BUFSZ];
    int x;
    int y;

    cls();
    time(&date);
    lt = localtime(&date);
    strcpy(buf, plname);
    buf[16] = 0;

    center(6, buf);
    sprintf(buf, "%ld AU", u.ugold);

    center(7, buf);
    if(strncmp(killer, "the ", 4) == 0) {
        sprintf(buf, "killed by%s", "");
    }
    else {
        if(strcmp(killer, "starvation") == 0) {
            sprintf(buf, "killed by%s", "");
        }
        else {
            if(index(vowels, *killer) != 0) {
                sprintf(buf, "killed by%s", "a");
            }
            else {
                sprintf(buf, "Killed by%s", " an");
            }
        }
    }

    center(8, buf);
    strcpy(buf, killer);
    if(strlen(buf) > 16) {
        int i;
        int i0;
        int i1;
        i1 = 0;
        i0 = i1;

        for(i = 0; i <= 16; ++i) {
            if(buf[i] == ' ') {
                i0 = i;
                i1 = i + 1;
            }
        }

        if(i0 == 0) {
            i1 = 16;
            i0 = i1;
        }

        buf[i1 + 16] = 0;
        center(10, buf + i1);
        buf[i0] = 0;
    }

    center(9, buf);
    if(lt->tm_year >= 100) {
        sprintf(buf, "20%2d", (lt->tm_year - 100));
    }
    else {
        sprintf(buf, "19%2d", lt->tm_year);
    }

    center(11, buf);
    for(y = 8; *dp != NULL; ++y) {
        x = 0;
        dpx = *dp;
        while(dpx[x] != NULL) {
            while(dpx[x] == ' ') {
                ++x;
            }
            
            curs(x, y);
            
            while((dpx[x] != NULL) && (dpx[x] != ' ')) {
                extern int done_stopprint;
                
                if(done_stopprint) {
                    return;
                }

                ++curx;
                putchar(dpx[x]);
                ++x;
            }
        }

        ++dp;
    }

    getret();
}

void center(int line, char *text)
{
    char *ip;
    char *op;

    ip = text;
    op = &rip[line][28 - ((strlen(text) + 1) / 2)];

    while(*ip != 0) {
        *op = *ip;
        ++op;
        ++ip;
    }
}

    
