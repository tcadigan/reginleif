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
 * #ident  "@(#)dospace.c        1.10 12/1/93
 */
#include "dospace.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "doship.h"
#include "doturn.h"
#include "GB_server.h"
#include "moveplanet.h"
#include "power.h"
#include "races.h"
#include "rand.h"
#include "ships.h"
#include "tweakables.h"
#include "vars.h"

#include "build.h"
#include "order.h"
#include "tele.h"

void do_turn(int);
int APadd(int, int, racetype *);
int governed(racetype *);
void fix_stability(startype *);
void do_reset(int);
void handle_victory(void);
void make_discoveries(racetype *);

#ifdef MARKET
void maintain(racetype *, int, int);
#endif

int attack_planet(shiptype *);
void output_ground_attacks(void);
int planet_points(planettype *);
int vp_planet_points(planettype *);

#define ALIVE(i) ((ships[(i)] != NULL) && ships[(i)]->alive && ships[(i)]->owner)

void doOrbit(int shipno)
{
    shiptype *sh;
    planettype *pl;
    int i;
    int damage = COMBAT_ORBIT_DAMAGE;
    char buf2[255];

    if (!valid_ship(shipno)) {
        return;
    }

    sh = ships[shipno];

    if (!ALIVE(shipno)) {
        return;
    }

    if (sh->whatorbits != LEVEL_PLAN) {
        return;
    }

    pl = planets[sh->storbits][sh->pnumorbits];

    /* If they own it do nothing */
    if (pl->info[sh->owner - 1].numsectsowned > 0) {
        return;
    }

    for (i = 1; i <= Num_races; ++i) {
        if ((i != sh->owner)
            && (pl->info[i - 1].numsectsowned > 0)
            && (!isset(races[sh->owner - 1]->allied, i)
                || !isset(races[i - 1]->allied, sh->owner))) {
            /* Orbiting inhabited planet - do damage */
            sh->damage += damage;
            sprintf(buf,
                    "Ship #[%d] orbiting inhabited planet before combat update [%d], incurred [%d%%] damage.\n",
                    shipno,
                    CombatUpdate,
                    damage);

            if (sh->damage >= 100) {
                kill_ship(sh->owner, sh);
            }

            if (!sh->alive) {
                strcat(buf, "Damage incurred destroyed ship.\n");
            }

            warn((int)sh->owner, (int)sh->governor, buf);
            sprintf(buf2, "[%d,%d]", sh->owner, sh->governor);
            strcat(buf2, buf);
            post(buf2, COMBAT);
            sh->build_cost = (int)cost(sh);
        }
    }
}

void doUniv(int shipno)
{
    shiptype *sh;
    unsigned short damage;
    double dmg;
    char buf2[512];

    if (!valid_ship(shipno)) {
        return;
    }

    sh = ships[shipno];
    memset(buf2, 0, sizeof(buf2));

    if (!ALIVE(shipno)) {
        return;
    }

    if (sh->whatorbits != LEVEL_UNIV) {
        return;
    }

    if (sh->type == OTYPE_PROBE) {
        return;
    }

    dmg = 20 / (1 + (sh->tech / 50));

    if (int_rand(1, 100) <= (dmg + sh->damage)) {
        damage = dmg;

        if (damage < 1) {
            damage = 1;
        } else if (damage > 100) {
            damage = 100;
        }

        sh->damage += damage;

        if (sh->damage >= 100.0) {
            sprintf(buf2,
                    "Ship [#%d] headed for %s, in trouble in deep space. %d%% damage incurred. Damage incurred destroyed ship.",
                    shipno,
                    prin_ship_dest(sh->owner, sh->governor, sh),
                    damage);

            kill_ship(sh->owner, sh);
            push_telegram((int)sh->owner, (int)sh->governor, buf2);
        } else if (((damage > 10.0) || (sh->damage > 70.0))
                   && sh->wants_reports) {
            sprintf(buf2,
                    "Ship [#%d] headed for %s, in trouble in deep space. %d%% damage incurred.",
                    shipno,
                    prin_ship_dest(sh->owner, sh->governor, sh),
                    damage);

            push_telegram((int)sh->owner, (int)sh->governor, buf2);
        }
    }
}
