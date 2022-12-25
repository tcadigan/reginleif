/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, at al.
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
 * dosector.dosector.c
 *   produce() -- Produce, stuff like that, on a sector.
 *   spread() -- Spread population around.
 *   explore() -- Mark sector and surrounding sectors as having been explored.
 *
 * #ident  "@(#)dosector.c      1.5 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/dosector.c,v 1.5 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: dosector.c,v $ $Revision: 1.5 $";
 */
#include "dosector.h"

#include <math.h>
#include <stdlib.h>

#include "doturn.h"
#include "max.h"
#include "power.h"
#include "races.h"
#include "rand.h"
#include "ships.h"
#include "vars.h"

extern int Defensedata[];

/* Produce stuff in sector */
void produce(startype *, planettype *, sectortype *);
void spread(planettype *, sectortype *, int, int);
void Migrate2(planettype *, int, int, sectortype *, int *);
void explore(planettype *, sectortype *, int, int, int);
void plate(sectortype *);

int x_adj[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
int y_adj[] = { 1, 1, 1, 0, 0, -1, -1, -1 };

void produce(startype *star,  planettype *planet, sectortype *s)
{
    int ss;
    int maxsup;
    int pfuel = 0;
    int pdes = 0;
    int pres = 0;
    struct plinfo *pinf;
    int prod;
    int diff;
    racetype *race;

    if (!s->owner) {
        return;
    }

    race = races[s->owner - 1];

    if (s->resource && success(s->eff)) {
        prod = round_rand(race->metabolism) * int_rand(1, s->eff);
        prod = MIN(prod, s->resource);

        if (LIMITED_RESOURCES) {
            s->resource -= prod;
        }

        /* Added this from treehouse -mfw */
        if (s->resource == 0) {
            s->resource = TRICKLE_RESOURCES;
        }

        if (s->condition == GAS) {
            pfuel = prod * 2 * FUEL_PROD_FACTOR;
        } else {
            pfuel = prod * FUEL_PROD_FACTOR;
        }

        if (success(s->mobilization * DEST_PROD_FACTOR)) {
            pdes = prod;
        } else {
            pres = prod;
        }

        prod_fuel[s->owner - 1] += pfuel;
        prod_res[s->owner - 1] += pres;
        prod_destruct[s->owner - 1] += pdes;
    }

    /*
     * Try to find crystals. Chance of digging out a crystal depends on
     * efficiency
     */
    if (s->crystals && Crystal(race) && success(s->eff)) {
        ++prod_crystals[s->owner - 1];
        --s->crystals;
    }

    pinf = &planet->info[s->owner - 1];

    /* Increase mobilization to planetary quota */
    if (s->mobilization < pinf->mob_set) {
        /* HUTm (kse) civilians are needed to raise mob */
        if (s->popn) {
            if ((pinf->resource + prod_res[s->owner - 1]) > 0) {
                ++s->mobilization;
                prod_res[s->owner - 1] -= round_rand(MOB_COST);
                ++prod_mob;
            }
        }
    } else if (s->mobilization > pinf->mob_set) {
        --s->mobilization;
        --prod_mob;
    }

    avg_mob[s->owner - 1] += s->mobilization;

    /* Do efficiency */
    if (s->eff < 100) {
        /* HUTm (kse) civilians are needed to raise eff */
        if (s->popn) {
            int chance;

            chance = round_rand((100.0 - (double)planet->info[s->owner - 1].tax)
                                * race->likes[s->condition]);

            if (success(chance)) {
                s->eff += round_rand(race->metabolism);

                if (s->eff >= 100) {
                    plate(s);
                }
            }
        }
    } else {
        plate(s);
    }

    if ((s->condition != WASTED) && race->fertilize && (s->fert < 100)) {
        if (int_rand(0, 100) < race->fertilize) {
            s->fert += 1;
        }
    }

    if (s->fert > 100) {
        s->fert = 100;
    }

    if ((s->condition == WASTED) && success(NATURAL_REPAIR)) {
        s->condition = s->type;
    }

    maxsup = maxsupport(race, s, Compat[s->owner - 1], planet->conditions[TOXIC]);
    diff = s->popn - maxsup;

    if (diff < 0) {
        if (s->popn >= race->number_sexes) {
            ss = round_rand(-(double)diff * race->birthrate);
        } else {
            ss = 0;
        }
    } else {
        ss = -int_rand(0, MIN(2 * diff, s->popn));
    }

#ifdef NOMADS
    if ((ss < 0) && (s->popn == race->number sexes)) {
        ss = 0;
    }
#endif

    s->popn += ss;

    if (s->troops) {
#ifdef COLLECTIVE_MONEY
        race->governor[0].maintain += (UPDATE_TROOP_COST * s->troops);

#else

        race->governor[(unsigned int)star->governor[s->owner - 1]].maintain += (UPDATE_TROOP_COST * s->troops);
#endif
    } else if (!s->popn) {
        s->owner = 0;
    }
}

void spread(planettype *pl, sectortype *s, int x, int y)
{
    int people;
    int x2;
    int y2;
    int j;
    int check;
    racetype *race;

    if (!s->owner) {
        return;
    }

    if (pl->sheep || (pl->slaved_to && (pl->slaved_to != s->owner))) {
        /* No one wants to go anywhere */
        return;
    }

    race = races[s->owner - 1];

    /*
     * New code by Kharush. More power to adventurism. At least race's number of
     * sexes moves, no need to hunt sectors that have too few people. 120
     * instead of 100 balances adventurism between dese/fore races.
     *
     * Code cleared by Gardan 4.3.97. Comments cleared by Kharush 10.3.97 Next
     * version would have an adjustable parameter for adventurism, that is how
     * power adventurism's effect is.
     *
     * Old code returned to effect since new code works funny Gardan 4.6.97
     *
     * New code starts
     *
     * if (double_rand() < (double)((race->adventurism * (120.0 - (double)s->fert)) / 120.0)) {
     *     if (s->popn >= (2 * race->number sexes)) {
     *         people = s->popn - race->number_sexes;
     *         j = int->rand(0, 7);
     *         x2 = x_adj[j];
     *         y2 = y_adj[j];
     *         Migrate2(pl, x + x2, y + y2, s, &people);
     *     }
     * }
     *
     * New code ends
     */

    /* Old code */
    if (s->popn >= (2 * race->number_sexes)) {
        /* The higher the fertility, the less people like to leave */
        people = round_rand(((double)race->adventurism * (double)s->popn * (100.0 - (double)s->fert)) / 100.0);

        /* How many people want to move - one family stays behind */
        /* More rounds for high advent */
        check = round_rand(6.0 * race->adventurism);

        while ((people > 0) && check) {
            j = int_rand(0, 7);
            x2 = x_adj[j];
            y2 = y_adj[j];
            Migrate2(pl, x + x2, y + y2, s, &people);
            --check;
        }
    }

    /* End of old code */
}

void Migrate2(planettype *planet, int xd, int yd, sectortype *ps, int *people)
{
    sectortype *pd;
    int move;

    /* Attempt to migrate beyond screen, or too many people */
    if ((yd > (planet->Maxy - 1)) || (yd < 0)) {
        return;
    }

    if (xd < 0) {
        xd = planet->Maxx - 1;
    } else if (xd > (planet->Maxx - 1)) {
        xd = 0;
    }

    pd = &Sector(*planet, xd, yd);

    if (!pd->owner) {
        /*
         * New code by Kharush. Planet's compatibility affect to adventurism is
         * removed. Also some other changes.
         */

        /*
         * if (double_rand() > (double)races[ps->owner - 1]->likes[ps->condition]) {
         *     /\* No adventurism to unsuitable sectors. *\/
         *     return;
         * }
         */

        move = (int)((double)(*people) * races[ps->owner - 1]->likes[pd->condition]);

        /*
         * Old code
         *
         * move = (int)(((double)(*people) * Compat[ps->owner - 1] * races[ps->owner - 1]->likes[pd->condition]) / 100.0);
         */

        if (!move) {
            return;
        }

        /* New code next three rows. */
        pd->popn += races[ps->owner - 1]->number_sexes;
        ps->popn -= races[ps->owner - 1]->number_sexes;
        *people -= races[ps->owner - 1]->number_sexes;

        /*
         * Old code
         *
         * people -= move;
         * pd->popn += move;
         * ps->popn -= move;
         */

        pd->owner = ps->owner;
        ++tot_captured;
        Claims = 1;
    }
}

/*
 * Mark sectors on the planet as having been "explored". For sea exploration on
 * earthtype planets.
 */
void explore(planettype *planet, sectortype *s, int x, int y, int p)
{
    int d;

    /* Explore sectors surrounding sectors currently explored. */
    if (Sectinfo[x][y].explored) {
        d = (x - 1) % planet->Maxx;
        Sectinfo[abs(d)][y].explored = p;
        d = (x + 1) % planet->Maxx;
        Sectinfo[abs(d)][y].explored = p;

        if (y == 0) {
            Sectinfo[x][1].explored = p;
        } else if (y == planet->Maxy - 1) {
            Sectinfo[x][y - 1].explored = p;
        } else {
            Sectinfo[x][y + 1].explored = p;
            Sectinfo[x][y - 1].explored = Sectinfo[x][y + 1].explored;
        }
    } else if (s->owner == p) {
        Sectinfo[x][y].explored = p;
    }
}

void plate(sectortype *s)
{
    s->eff = 100;

    if (s->condition != GAS) {
        s->condition = PLATED;
    }
}
