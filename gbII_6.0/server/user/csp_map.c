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
 * this program; if not, write to the free Software Foundation, Inc., 51
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * CSP_MAP Copyright (c) 1993 GBDT, John P. Deragon portions copyright Robert
 * Chansky
 *
 * #ident  "@(#)csp_map.c       1.2 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/csp_map.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 */

#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "csp.h"
#include "csp_types.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

static racetype *race;
extern char *Planet_types[];

int getowner(planettype *, int, int);
char getsymbol(planettype *, int, int, racetype *, int);
int gettype(planettype *, int, int);

void CSP_map(int playernum, int governor, int snum, int pnum, planettype *p)
{
    int i;
    int y;
    int x;
    int sh;
    int iq;
    shiptype *s;
    int shiplocs[MAX_X][MAX_Y];

    race = races[playernum - 1];

    if (!getsmap(Smap, p)) {
        notify(playernum, governor, "MAP Aborted: Error in getsmap\n");
        loginfo(ERRORLOG, WANTERRNO, "CSP_map(): Error in getsmap\n");

        return;
    }

    if (p->info[playernum - 1].numsectsowned) {
        iq = 1;
    } else {
        iq = 0;
    }

    sh = p->ships;
    memset(shiplocs, 0, sizeof(shiplocs));

    while (sh) {
        if (!getship(&s, sh)) {
            sh = 0;

            continue;
        }

        if ((s->owner == playernum)
            && authorized(governor, s)
            && (s->popn || (s->type == OTYPE_PROBE))) {
            iq = 1;
        }

        if (s->alive && landed(s)) {
            shiplocs[s->land_x][s->land_y] = Shipltrs[s->type];
        }

        sh = nextship(s);
        free(s);
    }

    /* > t CSP_INTRO Star# StarName Plan# PlanName Comp Tox Ens X Y Geo Inv DD s */
    /* > t CSP_INTRO %d %s %d %s %d %d %d %d %d %d %d %d */

    if (p->slaved_to) {
        sprintf(buf,
                "%c %d %d %s %d %s %f %d %d %d %d %d %d %d %d\n",
                CSP_CLIENT,
                CSP_MAP_INTRO,
                snum + 1,
                Stars[snum]->name,
                pnum + 1,
                Stars[snum]->pnames[pnum],
                compatibility(p, race),
                p->conditions[TOXIC],
                p->slaved_to,
                p->Maxx,
                p-Maxy,
                race->governor[governor].toggle.geography,
                race->governor[governor].toggle.inverse,
                race->governor[governor].toggle.double_digits,
                race->governor[governor].toggle.color);
    } else {
        sprintf(buf,
                "%c %d %d %s %d %s %f %d 0 %d %d %d %d %d %d\n",
                CSP_CLIENT,
                CSP_MAP_INTRO,
                snum + 1,
                Stars[snum]->name,
                pnum + 1,
                Stars[snum]->pnames[pnum],
                compatibility(p, race),
                p->conditions[TOXIC],
                p->Maxx,
                p->Maxy,
                race->governor[governor].toggle.geography,
                race->governor[governor].toggle.inverse,
                race->governor[governor].toggle.double_digits,
                race->governor[governor].toggle.color);
    }

    notify(playernum, governor, buf);

    /* > t CSP_DYNAMIC1 Type Sects Guns MobPoints Res Des Fuel Xtals */
    /* > t CSP_DYNAMIC1 %d %d %d %d %d %d %d %d */
    /* > t CSP_DYNAMIC2 Mob AMob Pop ^Pop TPop Mil TMil Tax ATax Dep EstPro */
    /* > t CSP_DYNAMIC2 %d %d %d %d %d %d %d %f %f %d %f*/
    /* > t CSP_ALIENS ALIEN1 ALIEN2 ... ALIENn */
    /* > t CSP_ALIENS %s */

    sprintf(buf,
            "%c %d %d %d %d %ld %d %d %d %d\n",
            CSP_CLIENT,
            CSP_MAP_DYNAMIC_1,
            p->type,
            p->info[playernum - 1].numsectsowned,
            p->info[playernum - 1].guns,
            p->info[playernum - 1].mob_points,
            p->info[playernum - 1].resource,
            p->info[playernum - 1].destruct,
            p->info[playernum - 1].fuel,
            p->info[playernum - 1].crystals);

    notify(playernum, governor, buf);

    sprintf(buf,
            "%c %d %s %s %ld %ld %d %ld %ld %d %d %ld %d\n",
            CSP_CLIENT,
            CSP_MAP_DYNAMIC_2,
            p->info[playernum - 1].mob_set,
            p->info[playernum - 1].comread,
            p->info[playernum - 1].popn,
            p->popn,
            round_rand(0.01 * (100.0 - p->conditions[TOXIC]) * p->maxpopn),
            p->info[playernum - 1].troops,
            p->troops,
            (int)p->info[playernum - 1].tax,
            (int)p->info[playernum - 1].newtax,
            p->total_resources,
            (int)p->info[playernum - 1].est_production);

    notify(playernum, governor, buf);
    sprintf(buf, "%c %d ", CSP_CLIENT, CSP_MAP_ALIENS);

    if (p->explored || (race->tech >= TECH_EXPLORE)) {
        for (i = 1; i < MAXPLAYERS; ++i) {
            if (p->info[i -1].numsectsowned && (i != playernum)) {
                sprintf(temp, "%d", i);
                strcat(buf, temp);
            }
        }

        strcat(buf, "\n");
    } else {
        sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_MAP_ALIENS, FALSE);
    }

    notify(playernum, governor, buf);

    for (y = 0; y < p->Maxy; ++y) {
        sprintf(buf, "%c %d ", CSP_CLIENT, CSP_MAP_DATA);
        notify(playernum, governor, buf);

        for (x = 0; x < p->Maxx; ++x) {
            if (shiplocs[x][y] && iq) {
                sprintf(buf,
                        "%d%c%d;",
                        gettype(p, x, y),
                        shiplocs[x][y],
                        getowner(p, x, y));
            } else {
                sprintf(buf,
                        "%d%c%d;",
                        gettype(p, x, y),
                        getsymbol(p, x, y, race, playernum),
                        getowner(p, x, y));
            }

            notify(playernum, governor, buf);
        }

        notify(playernum, governor, "\n");
    }

    sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_MAP_END);
    notify(playernum, governor, buf);
}

int gettype(planettype *p, int x, int y)
{
    return Sector(*p, x, y).condition;
}

int getowner(planettype *p, int x, int y)
{
    return Sector(*p, x, y).owner;
}

char getsymbol(planettype *p, int x, int y, racetype *r, int playernum)
{
    sectortype *s;
    char retval;

    s = &Sector(*p, x, y);

    if (s->crystals && (r->discoveries[D_CRYSTAL] || r->God)) {
        retval = CSPD_XTAL_SYMBOL;
    } else if (s->troops) {
        if (s->owner == Playernum) {
            retval = CSPD_TROOP_MIN_SYMBOL;
        } else if (isset(r->allied, s->owner)) {
            retval = CSPD_TROOP_ALLIED_SYMBOL;
        } else if (isset(r->war, s->owner)) {
            retval = CSPD_TROOP_ENEMY_SYMBOL;
        } else {
            retval = CSPD_TROOP_NEUTRAL_SYMBOL;
        }
    } else {
        switch (s->condition) {
        case WASTED:
            retval = CHAR_WASTED;

            break;
        case SEA:
            retval = CHAR_SEA;

            break;
        case LAND:
            retval = CHAR_LAND;

            break;
        case MOUNT:
            retval = CHAR_MOUNT;

            break;
        case GAS:
            retval = CHAR_GAS;

            break;
        case PLATED:
            retval = CHAR_PLATED;

            break;
        case ICE:
            retval = CHAR_ICE;

            break;
        case DESERT:
            retval = CHAR_DESERT;

            break;
        case FOREST:
            retval = CHAR_FOREST;

            break;
        case WORM:
            retval = CHAR_WORM;

            break;
        default:
            retval = '?';

            break;
        }
    }

    return retval;
}
