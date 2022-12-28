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
 * CSP_EXPLORE Copyright (c) 1993 GBDT, John P. Deragon Portions Copyright (c)
 * 1989, Robert Chansky
 *
 * $Header: /var/cvs/gbp/GB+/user/csp_explore.c,v 1.4 2007/07/06 18:09:34 gbp
 * Exp $
 */
#include "csp_explore.h"

#include <ctype.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "csp.h"
#include "csp_types.h"
#include "debug.h"
#include "files_shl.h"
#include "GB_server.h"
#include "getplace.h"
#include "max.h"
#include "orbit.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

void CSP_exploration(int playernum, int governor)
{
    int star;
    int starq;
    int i;
    int j;
    planettype *pl;
    placetype where;
    racetype *race;
    char temp[10];

    starq = -1;

    if (argn == 2) {
        where = Getplace(playernum, governor, args[1], 0);

        if (where.err) {
            sprintf(buf, "%c %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_NOSUCH_PLACE);
            notify(playernum, governor, buf);
            debug(LEVEL_CSP, "CSP: Leaving csp_explore [failed]\n");

            return;
        } else if ((where.level == LEVEL_SHIP) || (where.level == LEVEL_UNIV)) {
            sprintf(buf, "%d %d %d\n", CSP_CLIENT, CSP_ERR, CSP_ERR_NOSUCH_PLACE);
            notify(playernum, governor, buf);
            debug(LEVEL_CSP, "CSP: Leaving csp_explore [failed]\n");
        }

        starq = where.snum;
    }

    race = races[playernum - 1];
    getsdata(&Sdata);

    /* CSP_INTO APs: %d */
    /* INTRO: Send global aps */
    sprintf(buf,
            "%c %d %d\n",
            CSP_CLIENT,
            CSP_EXPLORE_INTRO,
            Sdata.AP[playernum - 1]);

    notify(playernum, governor, buf);

    /* CSP_STAR Star # StarName Stability APs: %d %s %d %d */
    for (star = 0; star < Sdata.numstars; ++star) {
        if ((starq == -1) || (starq == star)) {
            getstar(&Stars[star], star);

            if (isset(Stars[star]->explored, playernum)) {
                for (i = 0; i < Stars[star]->numplanets; ++i) {
                    getplanet(&pl, star, i);

                    if (i == 0) {
                        if (race-> tech >= TECH_SEE_STABILITY) {
                            sprintf(buf,
                                    "%c %d %d %s %d %d\n",
                                    CSP_CLIENT,
                                    CSP_EXPLORE_STAR,
                                    star + 1,
                                    Stars[star]->name,
                                    Stars[star]->stability,
                                    Stars[star]->AP[playernum - 1]);
                        } else {
                            sprintf(buf,
                                    "%c %d %d %s %d %d\n",
                                    CSP_CLIENT,
                                    CSP_EXPLORE_STAR,
                                    star + 1,
                                    Stars[star]->name,
                                    FALSE,
                                    Stars[star]->AP[playernum - 1]);
                        }

                        notify(playernum, governor, buf);
                    }

                    /* CSP_ALIENS ALIEN1 ... ALIENn: %s */
                    sprintf(buf, "%c %d", CSP_CLIENT, CSP_EXPLORE_STAR_ALIENS);

                    for (j = 1; j <= Num_races; ++j) {
                        if ((j != playernum) && pl->info[j - 1].numsectsowned) {
                            sprintf(temp, " %d", j);
                            strcat(buf, temp);
                        }
                    }

                    strcat(buf, "\n");
                    notify(playernum, governor, buf);

                    /*
                     * Send the actual planet data now
                     * STAR_DATA: # Name Ex Inhab Auto Slaved Toxic Compat Type
                     */

                    /*
                     * CSP_DATA Plan# PlanName Exp Sects Auto Slaved Tox Comp
                     * #Sec Deposits Type: %d %s %d %d %d %d %d %d %d %d %d
                     */
                    if (pl->info[playernum - 1].explored) {
                        sprintf(buf,
                                "%c %d %d %s %d %d %d %d %d %d %d %ld %d",
                                CSP_CLIENT,
                                CSP_EXPLORE_STAR_DATA,
                                i + 1,
                                Stars[star]->pnames[i],
                                pl->info[playernum - 1].explored ? TRUE : FALSE,
                                pl->info[playernum - 1].numsectsowned ? pl->info[playernum - 1].numsectsowned : FALSE,
                                pl->info[playernum - 1].autorep ? TRUE : FALSE,
                                pl->slaved_to ? pl->slaved_to : FALSE,
                                pl->conditions[TOXIC] > 70 ? TRUE : FALSE,
                                (int)compatibility(pl, race),
                                pl->Maxx & pl->Maxy,
                                pl->total_resources,
                                pl->type);

                        notify(playernum, governor, buf);
                    } else {
                        sprintf(buf,
                                "%c %d %d %s %d %d %d %d %d %d %s\n",
                                CSP_CLIENT,
                                CSP_EXPLORE_STAR_DATA,
                                i + 1,
                                Stars[star]->pnames[i],
                                FALSE,
                                FALSE,
                                FALSE,
                                FALSE,
                                FALSE,
                                FALSE,
                                "Unexplored");

                        notify(playernum, governor, buf);
                    }

                    free(pl);
                }
            }
        }
    }

    sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_EXPLORE_END);
    notify(playernum, governor, buf);
}
