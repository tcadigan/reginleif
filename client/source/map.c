/*
 * map.c: Handles the map data (planet and star) from server
 *
 * Written by Evan D. Koffler <evank@netcom.com>
 *
 * Copyright (c) 1990-1993
 *
 * See the COPYRIGHT file.
 */
#include "map.h"

#include "ansi.h"
#include "arrays.h"
#include "gb.h"
#include "key.h"
#include "option.h"
#include "str.h"
#include "types.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

char maplog[MAXSIZ];

void print_X(int Maxx);

void plot_surface(char *t)
{
    int x;
    int y;
    int Maxx;
    int Maxy;
    char *u;
    char *v;
    int inverse = 0;
    char temp[SMABUF];
    char convbuf[SMABUF];

    /* -mfw */
    char c;
    int want_color = 0;
    int colored = -1;

#ifdef CLIENT_DEVEL
    if (client_devel) {
        msg(":: plot_surface()");
    }
#endif

    want_color = (options[DISP_ANSI / 32] & ((DISP_ANSI < 32) ?
                                             (1 << DISP_ANSI)
                                             : (1 << (DISP_ANSI % 32))));

    /* Skip the marking char $ */
    u = t + 1;

    /* Get planet name */
    v = strchr(u, ';');

    /* Null term the name */
    *v = '\0';

    /* Print out planet name */
    msg("     \'%s\'", u);

    /* Get X size same way */
    u = v + 1;
    v = strchr(u, ';');
    *v = '\0';
    Maxx = atoi(u);

    /* Get Y size same way */
    u = v + 1;
    v = strchr(u, ';');
    *v = '\0';
    Maxy = atoi(u);

    /* Get show, not used currently */
    u = v + 1;
    v = strchr(u, ';');
    *v = '\0';

    print_X(Maxx);

    u = v + 1;

    for (y = 0; y < Maxy; ++y) {
        sprintf(convbuf, "%c%c ", (y / 10) + '0', (y % 10) + '0');
        strcpy(maplog, convbuf);

        for (x = 0; x < Maxx; ++x) {
            c = *u++;

            if (c == '1') {
                if (!inverse) {
                    /* For testing -mfw */
                    strcat(maplog, INVERSE_CHAR_STR);
                    inverse = 1;
                }
            } else if ((c > '?') && want_color) {
                colored = c % MAX_RCOLORS;
                strcat(maplog, race_colors[colored]);
                strcat(maplog, ANSI_FOR_BLACK);
            } else if (inverse) {
                strcat(maplog, INVERSE_CHAR_STR);
                inverse = 0;
            } else if (colored > -1) {
                colored = -1;
                strcat(maplog, ANSI_NORMAL);
            }

            strncat(maplog, u, 1);
            ++u;
        }

        if (inverse) {
            /* strcat(maplog, INVERSE_CHAR_STR); */
            strcat(maplog, ANSI_NORMAL);
        }

        inverse = 0;

        if (options[MAP_DOUBLE / 32] & ((MAP_DOUBLE < 32) ?
                                        (1 << MAP_DOUBLE)
                                        : (1 << (MAP_DOUBLE % 32)))) {
            sprintf(temp,
                    "%s%c%c",
                    (options[MAP_SPACE / 32] & ((MAP_SPACE < 32) ?
                                                (1 << MAP_SPACE)
                                                : (1 << (MAP_SPACE % 32)))) ?
                    " "
                    : "",
                    (y / 10) + '0',
                    (y % 10) + '0');

            strcat(maplog, temp);
        }

        msg("%s", maplog);
        add_recall(maplog, 0);
    }

    if (options[MAP_DOUBLE / 32] & ((MAP_DOUBLE < 32) ?
                                    (1 << MAP_DOUBLE)
                                    : (1 << (MAP_DOUBLE % 32)))) {
        if (options[MAP_SPACE / 32] & ((MAP_SPACE < 32) ?
                                       (1 << MAP_SPACE)
                                       : (1 << (MAP_SPACE % 32)))) {
            msg("");
        }

        print_X(Maxx);
    }

    cursor_to_window();
}

void print_X(int Maxx)
{
    int x;
    char buf[MAXSIZ];
    char temp[SMABUF];

    strcpy(buf, "   ");

    for (x = 0; x < Maxx; ++x) {
        sprintf(temp, "%d", x / 10);
        strcat(buf, temp);
    }

    msg(buf);
    strcpy(buf, "   ");

    for (x = 0; x < Maxx; ++x) {
        sprintf(temp, "%d", x % 10);
        strcat(buf, temp);
    }

    msg(buf);

    if (options[MAP_SPACE / 32] & ((MAP_SPACE < 32) ?
                                   (1 << MAP_SPACE)
                                   : (1 << (MAP_SPACE % 32)))) {
        msg("");
    }
}

void plot_orbit(char *t)
{
    char *p;
    char *q;
    char name[SMABUF];
    char colbuf[NORMSIZ];
    char symbol;

    /* -mfw */
    int want_color;
    int colored = -1;

    int array;

    /* Converted from int to char to handle color */
    char stand1;

    char stand2;
    int x;
    int y;

#ifdef CLIENT_DEVEL
    if (client_devel) {
        msg(":: plot_orbit()");
    }
#endif

    want_color = (options[DISP_ANSI / 32] & ((DISP_ANSI < 32) ?
                                             (1 << DISP_ANSI)
                                             : (1 << (DISP_ANSI % 32))));
    term_clear_screen();
    q = t + 1;

    p = strchr(q, ';');

    while (p) {
        /* New */
        *p = '\0';

        sscanf(q,
               "%c %d %d %d %c %c %s",
               &stand1,
               &x,
               &y,
               &array,
               &symbol,
               &stand2,
               name);

        x = (int)x * Midx;
        y = (int)y * Midy;

        if ((x <= S_X) && (y <= S_Y)) {
#ifdef ARRAY
            if ((symbol == 'M') && (array > 0) && (array <= 8)) {
                /* Space mirror */
                DispArray(x, y, 9, 5, Mirror[array - 1], 1.0);
            } else if ((symbol == '*') && (array > 0) && (array <= 16)) {
                /* Nova */
                DispArray(x, y, 11, 7, Novae[array - 1], 1.0);
            }
#endif

            /* DEBUG in here somewhere, too sleepy, going to bed. -mfw */
            if ((stand1 > '?') && want_color) {
                /* Do color here -mfw */
                colored = stand1 % MAX_RCOLORS;

                sprintf(colbuf,
                        "%s%s%c%s",
                        race_colors[colored],
                        ANSI_FOR_BLACK,
                        symbol,
                        ANSI_NORMAL);

                term_move_cursor(x, y);
                term_puts(colbuf, strlen(colbuf));
            } else if (stand1 == '1') {
                term_standout_on();
                term_move_cursor(x, y);
                term_putchar(symbol);
            } else {
                term_move_cursor(x, y);
                term_putchar(symbol);
            }

            if (stand1 == '1') {
                term_standout_off();
            }

            term_puts(" ", 1);

            if ((stand2 > '?') && want_color) {
                /* Do color here -mfw */
                colored = stand2 % MAX_RCOLORS;

                sprintf(colbuf,
                        "%s%s%s%s",
                        race_colors[colored],
                        ANSI_FOR_BLACK,
                        name,
                        ANSI_NORMAL);

                term_puts(colbuf, strlen(colbuf));
            } else if (stand2 == '1') {
                term_standout_on();
                term_puts(name, strlen(name));
            } else {
                term_puts(name, strlen(name));
            }

            if (stand2 == '1') {
                term_standout_off();
            }
        }

        /* New */
        q = p + 1;
        p = strchr(q, ';');
    }

    term_standout_off();
    last_output_row = output_row;
}

#ifdef ARRAY
/* Display array on screen. At the moment magnification is not implemented. */
void DispArray(int x, int y, int maxx, int maxy, char *array[], float mag)
{
    int x2;
    int y2;
    int curx;
    int cury;

#ifdef CLIENT_DEVEL
    if (client_devel) {
        msg(":: DispArray()");
    }
#endif

    for (cury = y - (maxy / 2), y2 = 0; y2 < maxy; ++cury) {
        if ((cury >= 0) && (cury <= S_Y)) {
            curx = x - (maxx / 2);
            term_move_cursor(curx, cury);

            for (x2 = 0; x2 < maxx; ++curx) {
                if ((curx >= 0) && (curx <= S_X)) {
                    /* One to right */
                    term_putchar(array[y2][x2]);
                }

                ++x2;
            }
        }

        ++y2;
    }
}

#endif
