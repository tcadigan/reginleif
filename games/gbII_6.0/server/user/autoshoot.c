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
 * autoshoot() -- Shoot <-> retailiate routine
 * auto_bomb() -- Ship bombards planet
 *
 * #ident  "@(#)autoshoot.c    1.8 13/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/autoshoot.c,v 1.3 2007/07/06 18:09:34 gbp Exp
 * $
 */
#include "autoshoot.h"

#include <stdlib.h> /* Added for free() (kse) */
#include <string.h> /* Added for strcat() (kse) */

#include "../server/buffers.h"
#include "../server/doturn.h"
#include "../server/files_shl.h"
#include "../server/GB_server.h"
#include "../server/perm.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/rand.h"
#include "../server/ships.h"
#include "../server/shlmisc.h"
#include "../server/vars.h"

#include "fire.h"
#include "load.h"
#include "shootblast.h"
#include "tele.h"

extern long Shipdata[NUMSTYPES][NUMABILS];
extern int Bombard(shiptype *, planettype *, racetype *);

/* Ship #shipno bombards planet, then alert whom it may concern. */
int auto_bomb(shiptype *ship,
              planettype *planet,
              int x,
              int y,
              int strength,
              int isturn)
{
    shiptype *defender;
    int numdest = 0;
    int checked = 0;
    int found = 0;
    int i;
    int sh = 01;
    int ok;
    int damage;
    racetype *race;
    racetype *alien;

#ifdef USE_VN
    shiptype pdn;
    int amount_to_shoot;
    int rez;
    int retal;
#endif

#ifdef USE_WORMHOLE
    if (planet->type == TYPE_WORMHOLE) {
        return -1;
    }
#endif

    race = races[ship->owner - 1];

    /* Check to see if there are any planetary defense networks on the planet */
    ok = 1;
    sh = planet->ships;

    while (sh && ok) {
        if (isturn) {
            defender = ships[sh];
        } else {
            getship(&defender, sh);
        }

        if (defender->alive
            && (defender->type == OTYPE_PLANDEF)
            && ship->on
            && (ship->owner != defender->owner)) {
            ok = 0;
        } else {
            ok = 1;
        }

#ifdef USE_VN
        /* CWL berserker take a pot shot at PDNs */
        if (!ok && (ship->type == OTYPE_BERS)) {
            rez = 1;

            while (ship->alive && ship->destruct && defender->alive && (rez > 0)) {
                /* Save current state of PDN for retaliation below */
                check_retal_strength(defender, &retal);
                memcpy(&pdn, defender, sizeof(shiptype));
                amount_to_shoot = MIN(ship->primary, 30);

                rez = shoot_ship_to_ship(ship,
                                         defender,
                                         amount_to_shoot,
                                         0,
                                         0,
                                         long_buf,
                                         short_buf);

                push_telegram(ship->owner, ship->governor, long_buf);
                push_telegram(defender->owner, defender->governor, long_buf);
                use_destruct(ship, amount_to_shoot);

                if (!defender->alive) {
                    post(short_buf, COMBAT);
                }

                /* PDN gets a turn to retaliate */
                if (retal && rez && defender->protect.self) {
                    shoot_ship_to_ship(&pdn,
                                       ship,
                                       retal,
                                       0,
                                       1,
                                       long_buf,
                                       short_buf);

                    push_telegram(defender->owner,
                                  defender->governor,
                                  long_buf);

                    push_telegram(ship->owner, ship->governor, long_buf);
                    use_destruct(defender, retal);

                    if (!ship->alive) {
                        post(short_buf, COMBAT);
                    }
                }
            }

            ok = 1;

            if (!isturn) {
                putship(defender);
            }
        }
        /* End CWL */
#endif

        sh = nextship(defender);

        if (!isturn) {
            free(defender);
        }

#ifdef USE_VN
        /* Berserker was killed or out of ammo, let's return */
        if (!ship->alive || !ship->destruct) {
            return 0;
        }
#endif
    }

    if (!ok && !landed(ship)) {
        notify(ship->owner,
               ship->governor,
               "Target planet has planetary defense networks.\nThese have to be eliminated before you can attack sectors.\n");

        return 0;
    }

    if ((x < 0) || (y < 0)) {
        x = 0;
        y = 0;

        /* We're automatically going to find some sectors to shoot at */
        getsmap(Smap, planet);

        /* Look for someone to bombard - check for war */
        Getxysect(planet, 0, 0, 1); /* Reset */

        while (!found && Getxysect(planet, &x, &y, 0)) {
            if (Sector(*planet, x, y).owner
                && (Sector(*planet, x, y).owner != ship->owner)
                && (Sector(*planet, x, y).condition != WASTED)) {
                checked = 1;

                if (isset(race->atwar, Sector(*planet, x, y).owner)) {
                    found = 1;
                }

#ifdef USE_VN
                if ((ship->type == OTYPE_BERS)
                    && (Sector(*planet, x, y).owner == ship->special.mind.target)) {
                    found = 1;
                }
#endif
            }
        }

        if (checked && !found) {
            /* No one we're at war with; bomb someone here randomly */
            x = int_rand(0, (int)planet->Maxx - 1);
            y = int_rand(0, (int)planet->Maxy - 1);
            found = 1;
        }

        if (!checked) {
            /* There were no sectors worth bombing */
            if (!ship->notified) {
                ship->notified = 1;

                sprintf(buf,
                        "%s reports /%s/%s has already been saturation bombed.\n",
                        Ship(ship),
                        Stars[ship->storbits]->name,
                        Stars[ship->storbits]->pnames[ship->pnumorbits]);

                notify(ship->owner, ship->governor, buf);

                return 01;
            }
        }
    }

    if (strength < 1) {
        strength = MIN((Shipdata[ship->type][ABIL_GUNS] * (100 - ship->damage)) / 100.0, ship->destruct);
    }

    /* For telegramming and retaliating */
    memset((char *)Nuked, 0, sizeof(Nuked));

    if (strength) {
        /*
         * Here we use the 'found variable to determine if we've called getsmap
         * or not, because if x,y were already set then found will be false.
         */
        if (found) {
            numdest = shoot_ship_to_planet(ship,
                                           planet,
                                           strength,
                                           x,
                                           y,
                                           0,
                                           0,
                                           0,
                                           long_buf,
                                           short_buf);

            if (numdest < 0) {
                numdest = 0;
            }
        } else {
            numdest = shoot_ship_to_planet(ship,
                                           planet,
                                           strength,
                                           x,
                                           y,
                                           1,
                                           0,
                                           0,
                                           long_buf,
                                           short_buf);

            if (numdest < 0) {
                notify(ship->owner,
                       ship->governor,
                       "Illegal attack, not destruct available.\n");

                return 0;
            }
        }
    } else {
        /* No weapons */
        if (!ship->notified) {
            ship->notified = 1;
            notify(ship->owner,
                   ship->governor,
                   "Illegal attack, no attack strength specified or available.\n");
        }

        return 0;
    }

    if (laser_on(ship)) {
        use_fuel(ship, 2.0 * (double)strength);
    } else {
        use_destruct(ship, strength);
    }

    post(short_buf, COMBAT);
    notify_star(ship->owner, ship->governor, 0, (int)ship->storbits, short_buf);

    for (i = 1; i <= Num_races; ++i) {
        if (Nuked[i - 1]) {
            warn(i, Stars[ship->storbits]->governor[i - 1], long_buf);
        }
    }

    notify(ship->owner, ship->governor, long_buf);

#ifdef DEFENSE
    /* Planet retaliates - AFVs are immune to this */
    if (numdest && (ship->type != OTYPE_AFV)) {
        damage = 0;

        for (i = 1; i <= Num_races; ++i) {
            if (Nuked[i - 1] && !planet->slaved_to) {
                /* Add planet defense strength */
                alien = races[i - 1];
                strength = MIN(planet->info[i - 1].destruct,
                               planet->info[i - 1].guns);

                if (planet->info[i - 1].destruct < strength) {
                    planet->info[i - 1].destruct = 0;
                } else {
                    planet->info[i - 1].destruct -= strength;
                }

                damage = shoot_planet_to_ship(alien,
                                              planet,
                                              ship,
                                              strength,
                                              long_buf,
                                              short_buf);

                warn(i, (int)Stars[ship->storbits]->governor[i - 1], long_buf);
                notify(ship->owner, ship->governor, long_buf);

                if (!ship->alive) {
                    post(short_buf, COMBAT);
                }

                notify_star(ship->owner,
                            ship->governor,
                            i,
                            (int)ship->storbits,
                            short_buf);
            }
        }
    }
#endif

    /* Protecting ships retaliate individually if damage was inflicted */
    /* AFVs are immune to this */
    if (numdest && ship->alive && (ship->type != OTYPE_AFV)) {
        sh = planet->ships;

        while (sh && ship->alive) {
            getship(&defender, sh);

            if (defender->protect.planet
                && (sh != ship->number)
                && defender->alive
                && defender->active) {
                if (laser_on(defender)) {
                    check_overload(defender, 0, &strength);
                }

                check_retal_strength(defender, &strength);

                damage = shoot_ship_to_ship(defender,
                                            ship,
                                            strength,
                                            0,
                                            0,
                                            long_buf,
                                            short_buf);

                if (damage >= 0) {
                    if (laser_on(defender)) {
                        use_fuel(defender, 2.0 * (double)strength);
                    } else {
                        use_destruct(defender, strength);
                    }

                    if (!ship->alive) {
                        post(short_buf, COMBAT);
                    }

                    notify_star(ship->owner,
                                ship->governor,
                                (int)defender->owner,
                                (int)ship->storbits,
                                short_buf);

                    warn((int)defender->owner,
                         (int)defender->governor,
                         long_buf);

                    notify(ship->owner, ship->governor, long_buf);
                }

                putship(defender);
            }

            sh = nextship(defender);
            free(defender);
        }
    }

    if (found) {
        putsmap(Smap, planet);
    }

    return numdest;
}
