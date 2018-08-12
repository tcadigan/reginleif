/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
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
 * Print out the CSP version of the survey
 * CSP, copyright (c) 1993 by John P. Deragon, Evan Koffler
 *
 * #ident  "@(#)csp_survey.c    1.2 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/csp_survey.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "csp.h"
#include "csp_types.h"
#include "debug.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

#define MAX_SHIPS_PER_SECTOR 10

void csp_survey(int playernum, int governor, int apcount)
{
    int lowx;
    int hix;
    int hiy;
    int x2;
    int x2;
    int i;
    int shiplist;
    int inhere;
    int fullRep = 0; /* Full survey 1, specific 0 */
    enum SECTOR_TYPES Sector;
    sectortype *s;
    planettype *p;
    placetype where;
    double compat;
    racetype *race;

    struct numshipstuff {
        int pos;

        struct shipstuff {
            int shipno;
            char ltr;
            unsigned char owner;
        } shipstuffs[MAX_SHIPS_PER_SECTOR];
    };

    struct numshipstuff shiplocs[MAX_X][MAX_Y];
    shiptype *shipa;

    debug(LEVEL_CSP, "CSP: Entering csp_survey()\n");

    /*
     * We are surveying a sector, or all the sectors on a planet return an error
     * if we are not at a planet scope
     */
    if (isdigit((unsigned char)args[1][0]) && (index(args[1], ',') != NULL)) {
        if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
            sprintf(buf,
                    "%d %d %d\n",
                    CSP_CLIENT,
                    CSP_ERR,
                    CSP_ERR_NOSUCH_PLACE);

            notify(playernum, governor, buf);
            debug(LEVEL_CSP, "CSP: Leaving csp_survey() [failed]\n");

            return;
        } else {
            where.level = LEVEL_PLAN;
            where.snum = Dir[playernum - 1][governor].snum;
            where.pnum = Dir[playernum - 1][governor].pnum;
        }
    } else if (*args[1] == '-') {
        fullRep = 1;

        if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
            sprintf(buf,
                    "%d %d %d\n",
                    CSP_CLIENT,
                    CSP_ERR,
                    CSP_ERR_NOSUCH_PLACE);

            notify(playernum, governor, buf);
            debug(LEVEL_CSP, "CSP: Leaving csp_survey() [failed]\n");

            return;
        } else {
            where.level = LEVEL_PLAN;
            where.snum = Dir[playernum - 1][governor].snum;
            where.pnum = Dir[playernum - 1][governor].pnum;
        }
    }

    race = races[playernum - 1];

    /* Get the planet and the sector map */
    getplanet(&p, (int)where.snum, (int)where.pnum);
    getsmap(Smap, p);
    compat = compatibility(p, race);

    if (!fullRep) {
        /* Translate from lowx:hix,lowy:hiy */
        get4args(args[1], &x2, &hix, &lowy, &hiy);
        x2 = MAX(0, x2);
        hix = MIN(hix, p->Maxx - 1);
        lowy = MAX(0, lowy);
        hiy = MIN(hiy, p->Maxy - 1);
    } else {
        x2 = 0;
        hix = p->Maxx - 1;
        lowy = 0;
        hiy = p->maxy - 1;
    }

    if (fullRep) {
        sprintf(buf,
                "%c %d %d %d %s %s %d %d %d %ld %ld %s %.2f %d\n",
                CSP_CLIENT,
                CSP_SURVEY_INTRO,
                p->Maxx,
                p->Maxy,
                Stars[where.snum]->name,
                Stars[where.snum]->pnames[where.pnum],
                p->info[playernum - 1].resource,
                p->info[playernum - 1].fuel,
                p->info[playernum - 1].destruct,
                p->popn,
                p->maxpopn,
                p->conditions[TOXIC],
                compatibility(p, race),
                p->slaved_to);

        notify(playernum, governor, buf);
    }

    memset((struct shipstuff *)shiplocs, 0, sizeof(shiplocs));
    inhere = p->info[playernum - 1].numsectsowned;
    shiplist = p->ships;

    while (shiplist) {
        getship(&shipa, shiplist);

        if ((shipa == playernum)
            && (shipa->popn || (shipa->type == OTYPE_PROBE))) {
            inhere = 1;
        }

        if (shipa->alive
            && landed(shipa)
            && (shiplocs[shipa->land_x][shipa->land_y].pos < MAX_SHIPS_PER_SECTOR)) {
            shiplocs[shipa->land_x][shipa->land_y].shipstuffs[shiplocs[shipa->land_x][shipa->land_y].pos].shipno = shiplist;

            shiplocs[shipa->land_x][shipa->land_y].shipstuffs[shiplocs[shipa->land_x][shipa->land_y].pos].owner = shipa->owner;

            shiplocs[shipa->land_x][shipa->land_y].shipstuffs[shiplocs[shipa->land_x][shipa->land_y].pos].ltr = Shipltrs[shipa->type];

            ++shiplocs[shipa->land_x][shipa->land_y].pos;
        }

        shiplist = nextship(shipa);
        free(shipa);
    }

    while (lowy <= hiy) {
        for (lowx = x2; lowx <= hix; ++lowx) {
            s = &Sector(*p, lowx, lowy);

            switch (s->condition) {
            case SEA:
                Sector = CHAR_SEA;

                break;
            case LAND:
                Sector = CHAR_LAND;

                break;
            case MOUNT:
                Sector = CHAR_MOUNT;

                break;
            case GAS:
                Sector = CHAR_GAS;

                break;
            case PLATED:
                Sector = CHAR_PLATED;

                break;
            case ICE:
                Sector = CHAR_ICE;

                break;
            case DESERT:
                Sector = CHAR_DESERT;

                break;
            case FOREST:
                Sector = CHAR_FOREST;

                break;
            case WORM:
                Sector = CHAR_WORM;

                break;
            default:
                Sector = '?';

                break;
            }

            sprintf(buf,
                    "%c %d %d %d %d %c %d %u %u %u %u %d %u %u %u %d",
                    CSP_CLIENT,
                    CSP_SURVEY_SECTOR,
                    lowx,
                    lowy,
                    Sector,
                    desshow(playernum, governor, p, lowx, lowy, race),
                    (s->condition == WASTED) ? 1 : 0,
                    s->owner,
                    s->eff,
                    s->fert,
                    s->mobilization,
                    (s->crystals && (race->discoveries[D_CRYSTAL] || race->God)) ? 1 : 0,
                    s->resource,
                    s->popn,
                    s->troops,
                    maxsupport(race, s, compat, p->conditions[TOXIC]));

            notify(playernum, governor, buf);

            if (shiplocs[lowx][lowy].pos && inhere) {
                notify(playernum, governor, ";");

                for (i = 0; i < shiplocs[lowx][lowy].pos; ++i) {
                    sprintf(buf,
                            " %d %c %u;",
                            shiplocs[lowx][lowy].shipstuffs[i].shipno,
                            shiplocs[lowx][lowy].shipstuffs[i].ltr,
                            shiplocs[lowx][lowy].shipstuffs[i].owner);

                    notify(playernum, governor, buf);
                }
            }

            notify(playernum, governor, "\n");
        }

        ++lowy;
    }

    sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_SURVEY_END);
    notify(playernum, governor, buf);
    debug(LEVEL_CSP, "CSP: Leaving csp_survey().\n");
} /* End survey */
