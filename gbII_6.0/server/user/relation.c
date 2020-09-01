/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (C) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
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
 * relation.c -- State relations among players
 *
 * #ident  "@(#)relation.c      1.11 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/relation.c,v 1.3 2007/07/06 18:09:34 gbp Exp $
 */

#include "buffers.h"
#include "csp.h"
#include "csp_types.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

extern char *Desnames[];

extern void relation(int, int, int);
extern void csp_relation(int, int);
extern int isallied(racetype *, int, int, int);

void relation(int playernum, int governor, int apcount)
{
    int numraces;
    int p;
    int q;
    char const *rt;
    racetype *r;
    racetype *race;
    racetype *myrace;

    numraces = Num_races;

    if (client_can_understand(playernum, governor, CSP_RELATION_COMMAND)) {
        csp_relation(playernum, governor);
    } else {
        if (argn == 1) {
            q = playernum;
        } else {
            q = GetPlayer(args[1]);

            if (!q) {
                notify(playernum, governor, "No such player.\n");

                return;
            }
        }

        race = races[q - 1];
        myrace = races[playernum - 1];

        sprintf(buf,
                "\n              Racial Relations Report for %s\n\n",
                race->name);

        notify(playernum, governor, buf);
        sprintf(buf,
                " #       know             Race name       Yours        Theirs\n");

        notify(playernum, governor, buf);
        sprintf(buf,
                " -       ----             ---------       -----        ------\n");

        notify(playernum, governor, buf);

        for (p = 1; p <= numraces; ++p) {
            if (p != race->Playernum) {
                r = races[p - 1];

                /*
                 * Show the other races type according to our own relation
                 * report.
                 */
                if (myrace->God || (myrace->translate[p - 1] > 30)) {
                    if (r->Metamorph) {
                        rt = "Morph ";
                    } else {
                        rt = "Normal";
                    }
                } else {
                    /* Unknown */
                    rt = "      ";
                }

                sprintf(buf,
                        "%2d %s (%3d%%) %20.20s : %10s   %10s\n",
                        p,
                        rt,
                        race->translate[p - 1],
                        r->name,
                        allied(race, p, 100, (int)race->God),
                        allied(r, q, (int)race->translate[p - 1], (int)race->God));

                notify(playernum, governor, buf);
            }
        }
    }
}

void csp_relation(int playernum, int governor)
{
    int numraces;
    int p;
    int q;
    int irt;
    racetype *r;
    racetype *race;
    racetype *myrace;

    numraces = Num_races;

    if (argn == 1) {
        q = playernum;
    } else {
        q = GetPlayer(args[1]);

        if (!q) {
            sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_ERR_NOSUCH_PLAYER);
            notify(playernum, governor, buf);

            return;
        }
    }

    race = races[q - 1];
    myrace = races[playernum - 1];
    sprintf(buf,
            "%c %d %d %s\n",
            CSP_CLIENT,
            CSP_RELATION_INTRO,
            q,
            race->name);

    notify(playernum, governor, buf);

    for (p = 1; p <= numraces; ++p) {
        if (p != race->playernum) {
            r = races[p - 1];

            /*
             * Show the other races type according to our own relation report.
             */
            if (myrace->God || (myrace->translate[p - 1] > 30)) {
                if (r->Metamorph) {
                    irt = CSPD_RACE_MORPH;
                } else {
                    irt = CSPD_RACE_NORMAL;
                }
            } else {
                irt = CSPD_RACE_UNKNOWN;
            }

            sprintf(buf,
                    "%c %d %d %d %d %d %d %s\n",
                    CSP_CLIENT,
                    CSP_RLATION_DATA,
                    p,
                    irt,
                    race->translate[p - 1],
                    iallied(race, p, 100, (int)race->God),
                    iallied(r, q, (int)race->translate[p - 1], (int)race->God),
                    r->name);
        }
    }

    sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_RELATION_END);
    notify(playernum, governor, buf);
}

char const *allied(racetype *r, int p, int q, int god)
{
    if (isset(r->atwar, p)) {
        return "WAR";
    } else if (isset(r->allied, p)) {
        return "ALLIED";
    } else {
        return "neutral";
    }
}

int iallied(racetype *r, int p, int q, int god)
{
    if (isset(r->atwar, p)) {
        return CSPD_RELAT_WAR;
    } else if (isset(r->allied, p)) {
        return CSPD_RELAT_ALLIED;
    } else {
        return CSPD_RELAT_NEUTRAL;
    }
}
