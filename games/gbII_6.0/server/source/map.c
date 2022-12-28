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
 * map.c -- Display sector map of current planet
 *
 * #ident  "@(#)map.c   1.10 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/map.c,v 1.3 2007/07/06 18:09:34 gbp Exp $
 */
#include "map.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "client.h"
#include "files_shl.h"
#include "GB_server.h"
#include "getplace.h"
#include "log.h"
#include "max.h"
#include "power.h"
#include "races.h"
#include "rand.h"
#include "ranks.h"
#include "shipdata.h"
#include "ships.h"
#include "shlmisc.h"
#include "vars.h"

#include "csp_map.h"
#include "fire.h"
#include "orbit.h"

#define DISP_DATA 1

static racetype *race;

void map(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    planettype *p;
    placetype where;

    where = Getplace(playernum, governor, args[1], 0);

    if (where.err) {
        return;
    } else if (where.level == LEVEL_SHIP) {
        notify(playernum, governor, "Bad scope.\n");

        return;
    } else if (where.level == LEVEL_PLAN) {
        getplanet(&p, (int)where.snum, (int)where.pnum);

        if (client_can_understand(playernum, governor, CSP_MAP_INTRO)) {
            CSP_map(playernum, governor, (int)where.snum, (int)where.pnum, p);
        } else {
            show_map(playernum,
                     governor,
                     (int)where.snum,
                     (int)where.pnum,
                     p,
                     DISP_DATA,
                     0);
        }

        free(p);

        if (Stars[where.snum]->stability > 50) {
            notify(playernum,
                   governor,
                   "WARNING! This planet's primary is unstable.\n");
        }
    } else {
        /* Make orbit map instead */
        orbit(playernum, governor, apcount, -1, NULL);
    }
}

void show_map(int playernum,
              int governor,
              int snum,
              int pnum,
              planettype *p,
              int show,
              int iq)
{
    int x;
    int y;
    int i;
    int f = 0;
    int owner;
    int owned1;
    int sh;
    shiptype *s;
    char shiplocs[MAX_X][MAX_Y];
    hugestr output;

    memset((char *)shiplocs, 0, sizeof(shiplocs));

    race = races[playernum - 1];

    if (!getsmap(Smap, p)) {
        notify(playernum, governor, "MAP Aborted: Error in getsmap\n");
        loginfo(ERRORLOG, WANTERRNO, "show_map(): Error in getsmap\n");

        return;
    }

    if (!race->governor[governor].toggle.geography) {
        /*
         * Traverse ship list on planet; find out if we can look at ships here
         */
        if (p->info[playernum - 1].numsectsowned) {
            iq = 1;
        } else {
            iq = 0;
        }

        sh = p->ships;

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
    }

    /* Report that this is a planet map */
    sprintf(output, "$");
    sprintf(buf, "%s;", Stars[snum]->pnames[pnum]);
    strcat(output, buf);
    sprintf(buf, "%d;%d;%d;", p->Maxx, p->Maxy, show);
    strcat(output, buf);

    /* Send map data */
    for (y = 0; y < p->Maxy; ++y) {
        for (x = 0; x < p->Maxx; ++x) {
            owner = Sector(*p, x, y).owner;

            if (owner == race->governor[governor].toggle.highlight) {
                owned1 = 1;
            } else {
                owned1 = 0;
            }

            if (shiplocs[x][y] && iq) {
                if (race->governor[governor].toggle.color) {
                    sprintf(buf, "%c%c", (char)(owner + '?'), shiplocs[x][y]);
                } else {
                    if (owned1 && race->governor[governor].toggle.inverse) {
                        sprintf(buf, "1%c", shiplocs[x][y]);
                    } else {
                        sprintf(buf, "0%c", shiplocs[x][y]);
                    }
                }
            } else {
                if (race->governor[governor].toggle.color) {
                    sprintf(buf,
                            "%c%c",
                            (char)(owner + '?'),
                            desshow(playernum, governor, p, x, y, race));
                } else {
                    if (owned1 && race->governor[governor].toggle.inverse) {
                        sprintf(buf,
                                "1%c",
                                desshow(playernum, governor, p, x, y, race));
                    } else {
                        sprintf(buf,
                                "0%c",
                                desshow(playernum, governor, p, x, y, race));
                    }
                }
            }

            strcat(output, buf);
        }
    }

    strcat(output, "\n");
    notify(playernum, governor, output);

    if (show) {
        if (race->Metamorph) {
            sprintf(temp,
                    "Type: %8s   Sects %7s: %3u   Aliens:",
                    Planet_types[p->type],
                    "covered",
                    p->info[playernum - 1].numsectsowned);
        } else {
            sprintf(temp,
                    "Type: %8s   Sects %7s: %3u   Aliens:",
                    Planet_types[p->type],
                    "owned",
                    p->info[playernum - 1].numsectsowned);
        }

        if (p->explored || (race->tech >= TECH_EXPLORE)) {
            f = 0;

            for (i = 1; i < MAXPLAYERS; ++i) {
                if (p->info[i - 1].numsectsowned && (i != playernum)) {
                    f = 1;

                    if (isset(race->atwar, i)) {
                        sprintf(buf, "*%d", i);
                    } else {
                        sprintf(buf, " %d", i);
                    }

                    strcat(temp, buf);
                }
            }

            if (!f) {
                strcat(temp, "(none)");
            }
        } else {
            strcat(temp, "\?\?\?");
        }

        strcat(temp, "\n");
        notify(playernum, governor, temp);

        sprintf(temp,
                "              Guns : %3d             Mob Points : %ld\n",
                p->info[playernum - 1].guns,
                p->info[playernum - 1].mob_points);

        notify(playernum, governor, temp);

        sprintf(temp,
                "      Mobilization : %3d (%3d)     Compatibility: %.2f%%",
                p->info[playernum - 1].comread,
                p->info[playernum - 1].mob_set,
                compatibility(p, race));

        if (p->conditions[TOXIC] > 50) {
            sprintf(buf, "    (%d%% TOXIC)", p->conditions[TOXIC]);
            strcat(temp, buf);
        }

        strcat(temp, "\n");
        notify(playernum, governor, temp);

        sprintf(temp,
                "Resource stockpile : %-9u    Fuel stockpile: %u\n",
                p->info[playernum - 1].resource,
                p->info[playernum - 1].fuel);

        notify(playernum, governor, temp);

        if (race->Metamorph) {
            sprintf(temp,
                    "      Destruct cap : %-9u%18s: %-5lu (%lu/%u)\n",
                    p->info[playernum - 1].destruct,
                    "Tons of biomass",
                    p->info[playernum - 1].popn,
                    p->popn,
                    round_rand(0.01 * (100.0 - p->conditions[TOXIC]) * p->maxpopn));
        } else {
            sprintf(temp,
                    "      Destruct cap : %-9u%18s: %-5lu (%lu/%u)\n",
                    p->info[playernum - 1].destruct,
                    "Total Population",
                    p->info[playernum - 1].popn,
                    p->popn,
                    round_rand(0.01 * (100.0 - p->conditions[TOXIC]) * p->maxpopn));
        }

        notify(playernum, governor, temp);

        sprintf(temp,
                "          Crystals : %-9u%18s: %-5lu (%lu)\n",
                p->info[playernum - 1].crystals,
                "Ground forces",
                p->info[playernum - 1].troops,
                p->troops);

        notify(playernum, governor, temp);

        sprintf(temp,
                "%ld Total Resource Deposits     Tax rate %u%%  New %u%%\n",
                p->total_resources,
                p->info[playernum - 1].tax,
                p->info[playernum - 1].newtax);

        notify(playernum, governor, temp);

        sprintf(temp,
                "Estimated Production Next Update : %.2f\n",
                p->info[playernum - 1].est_production);

        notify(playernum, governor, temp);

        if (p->slaved_to) {
            sprintf(temp, "      ENSLAVED to player %d\n", p->slaved_to);
            notify(playernum, governor, temp);
        }
    }
}

char desshow(int playernum,
             int governor,
             planettype *p,
             int x,
             int y,
             racetype *r)
{
    sectortype *s = &Sector(*p, x, y);

    if (s->troops && !r->governor[governor].toggle.geography) {
        if (s->owner == playernum) {
            return CHAR_MY_TROOPS;
        } else if (isset(r->allied, s->owner)) {
            return CHAR_ALLIED_TROOPS;
        } else if (isset(r->atwar, s->owner)) {
            return CHAR_ATWAR_TROOPS;
        } else {
            return CHAR_NEUTRAL_TROOPS;
        }
    }

    if (s->owner
        && !r->governor[governor].toggle.geography
        && !r->governor[governor].toggle.color) {
        if (!r->governor[governor].toggle.inverse
            || (s->owner != r->governor[governor].toggle.highlight)) {
            if (!r->governor[governor].toggle.double_digits) {
                return ((s->owner % 10) + '0');
            } else {
                if ((s->owner < 10) || (x % 2)) {
                    return ((s->owner % 10) + '0');
                } else {
                    return ((s->owner / 10) + '0');
                }
            }
        }
    }

    if (s->crystals && (r->discoveries[D_CRYSTAL] || r->God)) {
        return CHAR_CRYSTAL;
    }

    switch (s->condition) {
    case WASTED:

        return CHAR_WASTED;
    case SEA:

        return CHAR_SEA;
    case LAND:

        return CHAR_LAND;
    case MOUNT:

        return CHAR_MOUNT;
    case GAS:

        return CHAR_GAS;
    case PLATED:

        return CHAR_PLATED;
    case ICE:

        return CHAR_ICE;
    case DESERT:

        return CHAR_DESERT;
    case FOREST:

        return CHAR_FOREST;
    case WORM:

        return CHAR_WORM;
    default:

        return '?';
    }
}
