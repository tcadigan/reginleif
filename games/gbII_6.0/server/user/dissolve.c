/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and detail.s
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
 * dissolve.c -- Commit suicide, nuke all ships and sectors;
 *
 * July 24th, 1989, John Deragon, deragon@jethro.nyu.edu
 *
 * #ident  "@(#)dissolve.      1.8 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/dissolve.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 */
#include "dissolve.h"

#include <math.h>
#include <stdlib.h>

#include "../server/buffers.h"
#include "../server/config.h"
#include "../server/doship.h"
#include "../server/doturn.h"
#include "../server/files_shl.h"
#include "../server/GB_server.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/rand.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/shlmisc.h"
#include "../server/vars.h"

#include "tele.h"

void dissolve(int playernum, int governor)
{
    int n_ships;
    int i;
    int j;
    int z;
    int x2;
    int y2;
    int hix;
    int hiy;
    int lowx;
    int lowy;
    unsigned char waste;
    char nuke;
    char racepass[100];
    char govpass[100];
    shiptype *sp;
    racetype *race;
    planettype *pl;
    sectortype *s;

#ifndef DISSOLVE
    notify(playernum,
           governor,
           "Dissolve has been disabled. Please notify deity.\n");

    return;
#endif

    n_ships = Numships();

    if (argn < 3) {
        sprintf(buf, "Self-destruct sequence requires passwords.\n");
        notify(playernum, governor, buf);
        sprintf(buf,
                "Syntax: dissolve <race password> <leader password> <option> to initiate.\n");

        notify(playernum, governor, buf);

        return;
    } else {
        sprintf(buf, "WARNING!! WARNING!! WARNING!!\n");
        notify(playernum, governor, buf);
        sprintf(buf, "-------------------------------\n");
        notify(playernum, governor, buf);
        sprintf(buf, "Entering self destruct sequence!\n");
        notify(playernum, governor, buf);

        sscanf(args[1], "%s", racepass);
        sscanf(args[2], "%s", govpass);

        waste = 0;

        if (argn > 3) {
            sscanf(args[3], "%c", &nuke);

            if (nuke == 'w') {
                waste = 1;
            }
        }

#ifdef CHAP_AUTH
        /* Need to fix -mfw */
        Getracenum(racepass, govpass, &i, &j, govpass, waste);

#else
        Getracenum(racepass, govpass, &i, &j);
#endif

        if (!i) {
            sprintf(buf, "Password mismatch, self-destruct not initiated!\n");
            notify(playernum, governor, buf);

            return;
        }

        for (i = 1; i <= n_ships; ++i) {
            getship(&sp, i);

            if (sp->owner == playernum) {
                kill_ship(playernum, sp);
                sprintf(buf, "-- Ship #%d, self-destruct enabled\n", i);
                notify(playernum, governor, buf);
                putship(sp);
            }

            free(sp);
        }

        getsdata(&Sdata);

        for (z = 0; z < Sdata.numstars; ++z) {
            getstar(&Stars[z], z);

            if (isset(Stars[z]->explored, playernum)) {
                for (i = 0; i < Stars[z]->numplanets; ++i) {
                    getplanet(&pl, z, i);

                    if (pl->info[playernum - 1].explored
                        && pl->info[playernum - 1].numsectsowned) {
                        pl->info[playernum - 1].fuel = 0;
                        pl->info[playernum - 1].destruct = 0;
                        pl->info[playernum - 1].resource = 0;
                        pl->info[playernum - 1].popn = 0;
                        pl->info[playernum - 1].troops = 0;
                        pl->info[playernum - 1].tax = 0;
                        pl->info[playernum - 1].newtax = 0;
                        pl->info[playernum - 1].crystals = 0;
                        pl->info[playernum - 1].numsectsowned = 0;
                        pl->info[playernum - 1].explored = 0;
                        pl->info[playernum - 1].autorep = 0;
                    }

                    getsmap(Smap, pl);

                    lowx = 0;
                    lowy = 0;
                    hix = pl->Maxx - 1;
                    hiy = pl->Maxy - 1;

                    for (y2 = lowy; y2 <= hiy; ++y2) {
                        for (x2 = lowx; x2 <= hix; ++x2) {
                            s = &Sector(*pl, x2, y2);

                            if (s->owner == playernum) {
                                s->owner = 0;
                                s->troops = 0;
                                s->popn = 0;

                                if (waste) {
                                    /* Code folded from here */
                                    s->condition = WASTED;
                                    /* Unfolding */
                                }
                            }
                        }
                    }

                    putsmap(Smap, pl);
                    putstar(Stars[z], z);
                    putplanet(pl, z, i);
                    free(pl);
                }
            }
        }

        race = races[playernum - 1];
        race->dissolved = 1;
        putrace(race);
        sprintf(buf, "%s [%d] has dissolved.\n", race->name, playernum);
        post(buf, DECLARATION);
    }
}

int revolt(planettype *pl, int victim, int agent)
{
    int x;
    int y;
    int hix;
    int hiy;
    int lowx;
    int lowy;
    int changed_hands = 0;
    racetype *race;
    racetype *race2;
    sectortype *s;

    race = races[victim - 1];
    race2 = races[agent - 1];

    getsmap(Smap, pl);

    /* Do the revolt */
    lowx = 0;
    lowy = 0;
    hix = pl->Maxx - 1;
    hiy = pl->Maxy - 1;

    for (y = lowy; y <= hiy; ++y) {
        for (x = lowx; x <= hix; ++x) {
            s = &Sector(*pl, x, y);

            if ((s->owner == victim) && s->popn) {
                /*
                 * HUT Gardan 13.2.1997 Added sector pref to chance to convert
                 * sector. Chance is pref / 2, removed tax factor
                 */
                if (int_rand(1, 200) < (100 * race2->likes[s->type])) {
                    if (int_rand(1, (int)s->popn) > (10 * race->fighters * s->troops)) {
                        /* Enemy gets it */
                        s->owner = agent;
                        /* Some people killed */
                        s->popn = int_rand(1, (int)s->popn);
                        /* All troops destroyed */
                        s->troops = 0;
                        pl->info[victim - 1].numsectsowned -= 1;
                        pl->info[agent - 1].numsectsowned += 1;
                        pl->info[victim - 1].mob_points -= s->mobilization;
                        pl->info[agent - 1].mob_points += s->mobilization;
                        ++changed_hands;
                    }
                }
            }
        }
    }

    putsmap(Smap, pl);

    return changed_hands;
}
