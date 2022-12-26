/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chanksy, et al.
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
 * fire.c -- Fire at ship or planet from ship or planet
 *
 * #ident  "@(#)fire.c        1.9 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/fire.c,v 1.5 2007/07/06 18:06:56 gbp Exp $
 */
#include "fire.h"

#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "doship.h"
#include "GB_server.h"
#include "getplace.h"
#include "files_shl.h"
#include "first.h"
#include "rand.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "shlmisc.h"
#include "vars.h"

#include "autoshoot.h"
#include "load.h"
#include "shootblast.h"
#include "tele.h"

extern int retal_strength(shiptype *);
extern int adjacent(int, int, int, int, planettype *);
extern int landed(shiptype *);
extern void check_overload(shiptype *, int, int *);
extern void check_retal_strength(shiptype *, int *);
extern int laser_on(shiptype *);

/* Ship vs ship */
void fire(int playernum, int governor, int apcount, int cew, orbitinfo *unused5)
{
    int fromship;
    int toship;
    int nextshipno;
    int haveship;
    int sh = -1;
    shiptype *from;
    shiptype *to;
    shiptype *ship;
    shiptype dummy;
    planettype *p;
    planettype *planet;
    int strength;
    int maxstrength;
    int retal;
    int damage;

    /* Check to see if past First Combat update */
    if (get_num_updates() < CombatUpdate) {
        sprintf(buf,
                "fire command is disabled until after Combat Update: [%d]\n",
                CombatUpdate);

        notify(playernum, governor, buf);

        return;
    }

    /* For telegramming and retaliating */
    memset((char *)Nuked, 0, sizeof(Nuked));

    if (argn < 3) {
        notify(playernum,
               governor,
               "Syntax: 'fire <ship> <target> [<strength>]'.\n");

        return;
    }

    nextshipno = start_shiplist(playernum, governor, args[1]);
    fromship = do_shiplist(&from, &nextshipno);

    while (fromship) {
        if (in_list(playernum, args[1], from, &nextshipno)) {
            if (!from->active) {
                sprintf(buf, "%s is irradiated and inactive.\n", Ship(from));
                notify(playernum, governor, buf);
                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            if (from->type == STYPE_MINEF) {
                notify(playernum,
                       governor,
                       "That ship is not designed to be fired manually\n");

                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            if (from->whatorbits == LEVEL_UNIV) {
                if (!enufAP(playernum, governor, Sdata.AP[playernum - 1], apcount)) {
                    free(from);
                    fromship = do_shiplist(&from, &nextshipno);

                    continue;
                }
            } else if (!enufAP(playernum, governor, Stars[from->storbits]->AP[playernum - 1], apcount)) {
                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            if (cew) {
                if (!from->cew) {
                    notify(playernum,
                           governor,
                           "That ship is not equipped to fire CEWs.\n");

                    free(from);
                    fromship = do_shiplist(&from, &nextshipno);

                    continue;
                }

                if (!from->mounted) {
                    notify(playernum,
                           governor,
                           "You need to have a crystal mounted to fire CEWs.\n");

                    free(from);
                    fromship = do_shiplist(&from, &nextshipno);
                }
            }

            if (args[2][0] == '#') {
                sscanf(args[2] + 1, "%d", &toship);
            } else {
                sscanf(args[2], "%d", &toship);
            }

            if (toship <= 0) {
                notify(playernum, governor, "Bad ship number.\n");
                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            if (toship == fromship) {
                notify(playernum,
                       governor,
                       "Here I am, a brain the size of a universe, and you test me this way?\n");

                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            if (!getship(&to, toship)) {
                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            if (races[to->owner - 1]->Guest) {
                notify(playernum, governor, "You may not attack Guest ships.\n");
                free(from);
                free(to);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            /* Save defense attack strength for retaliation */
            check_retal_strength(to, &retal);
            memcpy(&dummy, to, sizeof(shiptype));

            if (from->type == OTYPE_AFV) {
                if (!landed(from)) {
                    sprintf(buf, "%s isn't landed on a planet!\n", Ship(from));
                    notify(playernum, governor, buf);
                    free(from);
                    free(to);
                    fromship = do_shiplist(&from, &nextshipno);

                    continue;
                }

                if (!landed(to)) {
                    sprintf(buf, "%s isn't landed on a planet!\n", Ship(to));
                    notify(playernum, governor, buf);
                    free(from);
                    free(to);
                    fromship = do_shiplist(&from, &nextshipno);

                    continue;
                }
            }

            if (landed(from) && landed(to)) {
                if ((from->storbits != to->storbits)
                    || (from->pnumorbits != to->pnumorbits)) {
                    notify(playernum,
                           governor,
                           "Landed ships can only attack other landed ships if they are on the same planet!\n");

                    free(from);
                    free(to);
                    fromship = do_shiplist(&from, &nextshipno);

                    continue;
                }

                getplanet(&p, (int)from->storbits, (int)from->pnumorbits);

                if (!adjacent((int)from->land_x, (int)from->land_y, (int)to->land_x, (int)to->land_y, p)) {
                    notify(playernum,
                           governor,
                           "You are not adjacent to your target!\n");

                    free(from);
                    free(to);
                    free(p);
                    fromship = do_shiplist(&from, &nextshipno);

                    continue;
                }

                free(p);
            }

            if (cew) {
                if (from->fuel < (double)from->cew) {
                    sprintf(buf, "You need %d fuel to fire CEWs.\n", from->cew);
                    notify(playernum, governor, buf);
                    free(from);
                    free(to);
                    fromship = do_shiplist(&from, &nextshipno);

                    continue;
                } else if (landed(from) || landed(to)) {
                    notify(playernum,
                           governor,
                           "CEWs cannot originate from or target ships landed on planets.\n");

                    free(from);
                    free(to);
                    fromship = do_shiplist(&from, &nextshipno);

                    continue;
                } else {
                    sprintf(buf, "CEW strength %d.\n", from->cew);
                    notify(playernum, governor, buf);
                    strength = from->cew / 2;
                }
            } else {
                check_retal_strength(from, &maxstrength);

                if (argn >= 4) {
                    sscanf(args[2], "%d", &strength);
                } else {
                    check_retal_strength(from, &strength);
                }

                if (strength > maxstrength) {
                    strength = maxstrength;

                    if (laser_on(from)) {
                        sprintf(buf, "Laser strength set to %d\n", strength);
                    } else {
                        sprintf(buf, "Guns set to %d\n", strength);
                    }
                }
            }

            /* Check to see if there is crystal overloads */
            if (laser_on(from) || cew) {
                check_overload(from, cew, &strength);
            }

            if (strength <= 0) {
                sprintf(buf, "No attack. (no strength specified)\n");
                notify(playernum, governor, buf);
                putship(from);
                free(from);
                free(to);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            /* Check for smart guns from HAP -mfw */
            if (from->smart_gun) {
                int ip;

                ip = 0;
                from->smart_fire = 0;

                while (from->smart_list[ip] && !from->smart_fire) {
                    if (from->smart_list[ip] == Shipltrs[to->type]) {
                        if (from->smart_gun == PRIMARY) {
                            strength = MIN(from->primary, from->smart_strength);
                        } else {
                            strength = MIN(from->secondary,
                                           from->smart_strength);
                        }

                        from->smart_fire = 1;

                        notify((int)from->owner,
                               from->governor,
                               "Smart guns activated!\n");
                    } /* if match */

                    ++ip;
                } /* while */
            } /* if smart */

            damage = shoot_ship_to_ship(from,
                                        to,
                                        strength,
                                        cew,
                                        0,
                                        long_buf,
                                        short_buf);

            if (damage < 0) {
                notify(playernum, governor, "Illegal attack.\n");
                free(from);
                free(to);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            if (laser_on(from) || cew) {
                use_fuel(from, 2.0 * (double)strength);
            } else {
                /*
                 * HUT modification (tze)
                 * If ship has no destruction loaded it uses planetary stockpile
                 */
                if (from->destruct < strength) {
                    getplanet(&planet, from->storbits, from->pnumorbits);
                    planet->info[from->owner - 1].destruct -= MIN(planet->info[from->owner - 1].destruct, strength - from->destruct);

                    use_destruct(from, from->destruct);
                    putplanet(planet, from->storbits, from->pnumorbits);
                    free(planet);
                } else {
                    use_destruct(from, strength);
                }
            }

            /* use_destruct(from, strength); */

            if (!to->alive) {
                post(short_buf, COMBAT);
            }

            notify_star(playernum,
                        governor,
                        (int)to->owner,
                        (int)from->storbits,
                        short_buf);

            warn((int)to->owner, (int)to->governor, long_buf);
            notify(playernum, governor, long_buf);

            /* Defending ship retaliates */
            strength = 0;

            if (retal && damage && to->protect.self) {
                strength = retal;

#ifdef USE_VN
                /*
                 * CWL Berserkers vary how much destruct to use against a foe
                 * missiles, mines, pods, explorers, SETI, and shuttles get 10
                 * guns fired at them. Everything else gets full load.
                 */
                if (to->type == OTYPE_BERS) {
                    if ((from->type == STYPE_MISSILE)
                        || (from->type == STYPE_MINEF)
                        || (from->type == STYPE_SHUTTLE)
                        || (from->type == STYPE_POD)
                        || (from->type == OTYPE_SETI)) {
                        strength = 10;
                    } else if ((from->type == OTYPE_GR)
                               || (from->type == STYPE_EXPLORER)) {
                        strength = 20;
                    }

                    strength = MIN(dummy.primary, strength);
                } /* end CWL */
#endif

                /* Smart gun retaliate from HAP -mfw */
                if (to->smart_gun) {
                    int ip;

                    ip = 0;
                    to->smart_fire = 0;


                    while (to->smart_list[ip] && !to->smart_fire) {
                        if (to->smart_list[ip] == Shipltrs[from->type]) {
                            if (to->smart_gun == PRIMARY) {
                                strength = MIN(dummy.primary,
                                               to->smart_strength);
                            } else {
                                strength = MIN(dummy.secondary,
                                               to->smart_strength);
                            }

                            to->smart_fire = 1;

                            warn((int)to->owner,
                                 (int)to->governor,
                                 "Smart gun activated!\n");
                        } /* From ship is in our smart list */

                        ++ip;
                    } /* while */
                } /* Smart retaliate */

                if (laser_on(to)) {
                    check_overload(to, 0, &strength);
                }

                damage = shoot_ship_to_ship(&dummy,
                                            from,
                                            strength,
                                            0,
                                            1,
                                            long_buf,
                                            short_buf);

                if (damage >= 0) {
                    if (laser_on(to)) {
                        use_fuel(to, 2.0 * (double)strength);
                    } else {
                        /*
                         * HUT modification (tze)
                         * If ship has no destruction loaded it uses planetary
                         * stockpile
                         */
                        if (to->destruct < strength) {
                            getplanet(&planet, to->storbits, to->pnumorbits);
                            planet->info[to->owner - 1].destruct -= MIN(planet->info[to->owner - 1].destruct, strength - to->destruct);
                            use_destruct(to, to->destruct);
                            putplanet(planet, to->storbits, to->pnumorbits);
                            free(planet);
                        } else {
                            use_destruct(to, strength);
                        }
                    }

                    /* use_destruct(to, strength); */

                    if (!from->alive) {
                        post(short_buf, COMBAT);
                    }

                    notify_star(playernum,
                                governor,
                                (int)to->owner,
                                (int)from->storbits,
                                short_buf);

                    notify(playernum, governor, long_buf);
                    warn((int)to->owner, (int)to->governor, long_buf);
                }
            }

            /*
             * Protecting ships retaliate individually if damage was inflicted
             */

            /* AFVs immune to retaliation of this type */
            if (damage && from->alive && (from->type != OTYPE_AFV)) {
                /* Star level ships */
                if (to->whatorbits == LEVEL_STAR) {
                    sh = Stars[to->storbits]->ships;
                }

                /* Planet level ships */
                if (to->whatorbits == LEVEL_PLAN) {
                    getplanet(&p, (int)to->storbits, (int)to->pnumorbits);
                    sh = p->ships;
                    free(p);
                }

                while (sh && from->alive) {
                    /* XXX: infinite loop possibility here, I think -mfw */
                    haveship = getship(&ship, sh);

                    if (ship->protect.on
                        && (ship->protect.ship == toship)
                        && (sh != fromship)
                        && (sh != toship)
                        && ship->alive
                        && ship->active) {
                        check_retal_strength(ship, &strength);

                        /* Smart guns protect from HAP -mfw */
                        if (ship->smart_gun) {
                            int ip;

                            ip = 0;
                            ship->smart_fire = 0;

                            while (ship->smart_list[ip] && !ship->smart_fire) {
                                if (ship->smart_list[ip] == Shipltrs[from->type]) {
                                    if (ship->smart_gun == PRIMARY) {
                                        strength = MIN(ship->primary,
                                                       ship->smart_strength);
                                    } else {
                                        strength = MIN(ship->secondary,
                                                       ship->smart_strength);
                                    }

                                    ship->smart_fire = 1;

                                    warn((int)to->owner,
                                         (int)to->governor,
                                         "Smart gun activated!\n");
                                } /* From ship is in our smart list */

                                ++ip;
                            } /* while */
                        } /* Smart protect */

                        if (laser_on(ship)) {
                            check_overload(ship, 0, &strength);
                        }

                        damage = shoot_ship_to_ship(ship,
                                                    from,
                                                    strength,
                                                    0,
                                                    0,
                                                    long_buf,
                                                    short_buf);

                        if (damage >= 0) {
                            if (laser_on(ship)) {
                                use_fuel(ship, 2.0 * (double)strength);
                            } else {
                                /*
                                 * HUT modification (tze)
                                 * If ship has no destruct loaded it uses
                                 * planetary stockpile
                                 */
                                if (ship->destruct < strength) {
                                    getplanet(&planet,
                                              ship->storbits,
                                              ship->pnumorbits);

                                    planet->info[ship->owner - 1].destruct -= MIN(planet->info[ship->owner - 1].destruct, strength - ship->destruct);

                                    use_destruct(ship, ship->destruct);
                                    putplanet(planet,
                                              ship->storbits,
                                              ship->pnumorbits);

                                    free(planet);
                                } else {
                                    use_destruct(ship, strength);
                                }
                            }

                            /* use_destruct(ship, strength); */

                            if (!from->alive) {
                                post(short_buf, COMBAT);
                            }

                            notify_star(playernum,
                                        governor,
                                        (int)ship->owner,
                                        (int)from->storbits,
                                        short_buf);

                            notify(playernum, governor, long_buf);
                            warn((int)ship->owner,
                                 (int)ship->governor,
                                 long_buf);
                        }

                        putship(ship);
                    }

                    if (haveship) {
                        sh = nextship(ship);
                        free(ship);
                    } else {
                        sh = 0;
                    }

                    haveship = 0;
                }
            }

            putship(from);
            putship(to);
            deductAPs(playernum, governor, apcount, (int)from->storbits, 0);

            free(from);
            free(to);
        } else {
            free(from);
        }

        fromship = do_shiplist(&from, &nextshipno);
    }
}

/* Ship vs planet */
void bombard(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int fromship;
    int nextshipno;
    shiptype *from;
    planettype *planet;
    int strength;
    int maxstrength;
    int x;
    int y;
    int i;

    /* Check to see if past First Combat update */
    if (get_num_updates() < CombatUpdate) {
        sprintf(buf,
                "bombard command is disabled until after Combat Update: [%d]\n",
                CombatUpdate);

        notify(playernum, governor, buf);

        return;
    }

    if (argn < 2) {
        notify(playernum,
               governor,
               "Syntax: 'bombard <ship> [<x,y> [<strength>]]'.\n");

        return;
    }

    nextshipno = start_shiplist(playernum, governor, args[1]);
    fromship = do_shiplist(&from, &nextshipno);

    while (fromship) {
        if (in_list(playernum, args[1], from, &nextshipno)) {
            if ((from->storbits != Dir[playernum - 1][governor].snum)
                || (from->pnumorbits != Dir[playernum - 1][governor].pnum)) {
                notify(playernum,
                       governor,
                       "Change scope to the planet this ship is orbiting.\n");

                free(from);

                return;
            }

            if (!enufAP(playernum, governor, Stars[from->storbits]->AP[playernum - 1], apcount)) {
                /* Not enough APs */
                sprintf(buf, "Not enough APs for %s.\n", Ship(from));
                notify(playernum, governor, buf);
                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            if (from->whatorbits != LEVEL_PLAN) {
                sprintf(buf,
                        "%s must be in orbit around a planet to bombard.\n",
                        Ship(from));

                notify(playernum, governor, buf);
                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            if (!from->active) {
                sprintf(buf, "%s irradiated and inactive.\n", Ship(from));
                notify(playernum, governor, buf);
                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            if ((from->type == OTYPE_AFV) && !landed(from)) {
                sprintf(buf, "%s is not landed on the planet.\n", Ship(from));
                notify(playernum, governor, buf);
                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            check_retal_strength(from, &maxstrength);

            if (argn > 3) {
                sscanf(args[3], "%d", &strength);
            } else {
                check_retal_strength(from, &strength);
            }

            if (strength > maxstrength) {
                strength = maxstrength;

                if (laser_on(from)) {
                    sprintf(buf, "Laser strength set to %d\n", strength);
                } else {
                    sprintf(buf, "Guns set to %d\n", strength);
                }

                notify(playernum, governor, buf);
            }

            /* Check to see if there is crystal overload */
            if (laser_on(from)) {
                check_overload(from, 0, &strength);
            }

            if (strength <= 0) {
                sprintf(buf, "No attack. (no strength specified)\n");
                notify(playernum, governor, buf);
                putship(from);
                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            /* Get planet */
            getplanet(&planet, (int)from->storbits, (int)from->pnumorbits);

            /* Make sure we're not bombarding a guest planet -mfw */
            for (i = 1; i <= Num_races; ++i) {
                if ((i != playernum) && planet->info[i - 1].popn) {
                    if (races[i - 1]->Guest) {
                        notify(playernum,
                               governor,
                               "Can't bombard a Guest's planet.\n");

                        free(from);
                        free(planet);
                        fromship = do_shiplist(&from, &nextshipno);

                        continue;
                    }
                }
            }

#ifdef USE_WORMHOLE
            racetype *race = races[playernum - 1];

            if (p->type == TYPE_WORMHOLE) {
                if ((race->tech >= TECH_WORMHOLE) || race->God) {
                    sprintf(buf,
                            "The bombs would just disappear into the wormhole.\n");
                } else {
                    sprintf(buf, "There is no surface to bomb\n");
                }

                notify(playernum, governor, buf);
                free(planet);
                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }
#endif

            if (argn > 2) {
                sscanf(args[2], "%d,%d", &x, &y);

                if ((x < 0)
                    || (x > (planet->Maxx - 1))
                    || (y < 0)
                    || (y > (planet->Maxy - 1))) {
                    notify(playernum, governor, "Illegal sector.\n");
                    free(planet);
                    free(from);
                    fromship = do_shiplist(&from, &nextshipno);

                    continue;
                }
            } else {
                /* The auto_bomb() function is going to pick the coords */
                x = -1;
                y = -1;
            }

            if (landed(from)
                && !adjacent((int)from->land_x, (int)from->land_y, x, y, planet)) {
                notify(playernum,
                       governor,
                       "You are not adjacent to that sector.\n");

                free(planet);
                free(from);
                fromship = do_shiplist(&from, &nextshipno);

                continue;
            }

            auto_bomb(from, planet, x, y, strength, 0);

            /* Write the stuff to disk */
            putship(from);
            putplanet(planet, (int)from->storbits, (int)from->pnumorbits);
            deductAPs(playernum, governor, apcount, (int)from->storbits, 0);
            free(planet);
        }

        free(from);
        fromship = do_shiplist(&from, &nextshipno);
    }
}

#ifdef DEFENSE
/* Planet vs ship */
void defend(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int toship;
    int sh;
    shiptype *to;
    shiptype *ship;
    shiptype dummy;
    planettype *p;
    sectortype *sect;
    int strength;
    int retal;
    int damage;
    int x = 0;
    int y = 0;
    int numdest;
    racetype *race;

    /* Check to see if past First combat update */
    if (get_num_updates() < CombatUpdate) {
        sprintf(buf,
                "defend command is disabled until after Combat Update: [%d]\n",
                CombatUpdate);

        notify(playernum, governor, buf);

        return;
    }

    /* For telegramming and retaliating */
    memset((char *)Nuked, 0, sizeof(Nuked));

    /* Get the planet from the player's current scope */
    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        notify(playernum,
               governor,
               "You have to set scope to the planet first.\n");

        return;
    }

    if (argn < 3) {
        notify(playernum,
               governor,
               "Syntax: 'defend <ship> <sector> [<strength>]'.\n");

        return;
    }

    if (args[1][0] == '#') {
        sscanf(args[1] + 1, "%d", &toship);
    } else {
        sscanf(args[1], "%d", &toship);
    }

    if (toship <= 0) {
        notify(playernum, governor, "Bad ship number.\n");

        return;
    }

    if (!enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
        return;
    }

    getplanet(&p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    if (!p->info[playernum - 1].numsectsowned) {
        notify(playernum, governor, "You do not occupy any sectors here.\n");
        free(p);

        return;
    }

    if (p->slaved_to && (p->slaved_to != playernum)) {
        notify(playernum, governor, "This planet is enslaved.\n");
        free(p);

        return;
    }

    if (!getship(&to, toship)) {
        free(p);

        return;
    }

    if (to->whatorbits != LEVEL_PLAN) {
        notify(playernum, governor, "The ship is not in planet orbit.\n");
        free(to);
        free(p);

        return;
    }

    if ((to->storbits != Dir[playernum - 1][governor].snum)
        || (to->storbits != Dir[playernum - 1][governor].pnum)) {
        notify(playernum,
               governor,
               "Target is not in orbits around this planet.\n");

        free(to);
        free(p);

        return;
    }

    if (landed(to)) {
        notify(playernum,
               governor,
               "Planet guns can't fire on landed ships.\n");

        free(to);
        free(p);

        return;
    }

    if (races[to->owner - 1]->Guest) {
        notify(playernum, governor, "Can't fire on Guest ships.\n");
        free(to);
        free(p);

        return;
    }

    /* Save defense strength for retaliation */
    check_retal_strength(to, &retal);
    memcpy(&dummy, to, sizeof(shiptype));

    if (argn >= 3) {
        sscanf(args[2], "%d,%d", &x, &y);
    }

    if ((x < 0)
        || (x > (p->Maxx - 1))
        || (y < 0)
        || (y > (p->Maxy - 1))) {
        notify(playernum, governor, "Illegal sector.\n");
        free(p);
        free(to);

        return;
    }

    if (!getsector(&sect, p, x, y)) {
        notify(playernum,
               governor,
               "Error in sector database, notify deity.\n");

        free(p);
        free(to);

        return;
    }

    /* Check to see if you own the sector */
    if (sect->owner != playernum) {
        notify(playernum, governor, "Nice try.\n");
        free(sect);
        free(p);
        free(to);

        return;
    }

    free(sect);

    if (argn >= 4) {
        sscanf(args[3], "%d", &strength);
    } else {
        strength = p->info[playernum - 1].guns;
    }

    strength = MIN(strength, p->info[playernum - 1].destruct);
    strength = MIN(strength, p->info[playernum - 1].guns);

    if (strength <= 0) {
        sprintf(buf,
                "No attack - %d guns, %dd\n",
                p->info[playernum - 1].guns,
                p->info[playernum - 1].destruct);

        notify(playernum, governor, buf);
        free(p);
        free(to);

        return;
    }

    race = races[playernum - 1];
    damage = shoot_planet_to_ship(race,
                                  p,
                                  to,
                                  strength,
                                  long_buf,
                                  short_buf);

    if (!to->alive && (to->type == OTYPE_TOXIC)) {
        /* Get planet again since toxicity probably has changed */
        free(p);
        getplanet(&p,
                  Dir[playernum - 1][governor].snum,
                  Dir[playernum - 1][governor].pnum);
    }

    if (damage < 0) {
        sprintf(buf, "Target out of range %.2d!\n", SYSTEMSIZE);
        notify(playernum, governor, buf);
        free(p);
        free(to);

        return;
    }

    if (p->info[playernum - 1].destruct < strength) {
        p->info[playernum - 1].destruct = 0;
    } else {
        p->info[playernum - 1].destruct -= strength;
    }

    if (!to->alive) {
        post(short_buf, COMBAT);
    }

    notify_star(playernum,
                governor,
                (int)to->owner,
                (int)to->storbits,
                short_buf);

    warn((int)to->owner, (int)to->governor, long_buf);
    notify(playernum, governor, long_buf);

    /* Defending ship retaliates */
    strength = 0;

    if (retal && damage && to->protect.self) {
        strength = retal;

        if (laser_on(to)) {
            check_overload(to, 0, &strength);
        }

        numdest = shoot_ship_to_planet(&dummy,
                                       p,
                                       strength,
                                       x,
                                       y,
                                       1,
                                       0,
                                       0,
                                       long_buf,
                                       short_buf);

        if (numdest >= 0) {
            if (laser_on(to)) {
                use_fuel(to, 2.0 * (double)strength);
            } else {
                use_destruct(to, strength);
            }

            post(short_buf, COMBAT);

            notify_star(playernum,
                        governor,
                        (int)to->owner,
                        (int)to->storbits,
                        short_buf);

            notify(playernum, governor, long_buf);
            warn((int)to->owner, (int)to->governor, long_buf);
        }
    }

    /* Protecting ships retaliate individually if damage was inflicted */
    if (damage) {
        sh = p->ships;

        while (sh) {
            getship(&ship, sh);

            if (ship->protect.on
                && (ship->protect.ship == toship)
                && (sh != toship)
                && ship->alive
                && ship->active) {
                if (laser_on(ship)) {
                    check_overload(ship, 0, &strength);
                }

                check_retal_strength(ship, &strength);

                numdest = shoot_ship_to_planet(ship,
                                               p,
                                               strength,
                                               x,
                                               y,
                                               1,
                                               0,
                                               0,
                                               long_buf,
                                               short_buf);

                if (numdest >= 0) {
                    if (laser_on(ship)) {
                        use_fuel(ship, 2.0 * (double)strength);
                    } else {
                        use_destruct(ship, strength);
                    }

                    post(short_buf, COMBAT);

                    notify_star(playernum,
                                governor,
                                (int)ship->owner,
                                (int)ship->storbits,
                                short_buf);

                    notify(playernum, governor, long_buf);
                    warn((int)ship->owner, (int)ship->governor, long_buf);
                }

                putship(ship);
            }

            sh = nextship(ship);
            free(ship);
        }
    }

    /* Write the ship stuff out to disk */
    putship(to);

    putplanet(p,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    deductAPs(playernum,
              governor,
              apcount,
              Dir[playernum - 1][governor].snum,
              0);

    free(p);
    free(to);
}
#endif

void detonate(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    shiptype *s;
    int shipno;
    int nextshipno;

    /* Check to see if past Combat Update */
    if (get_num_updates() < CombatUpdate) {
        sprintf(buf,
                "detonate command is disabled until after Combat Update: [%d]\n",
                CombatUpdate);

        notify(playernum, governor, buf);

        return;
    }

    nextshipno = start_shiplist(playernum, governor, args[1]);
    shipno = do_shiplist(&s, &nextshipno);

    while (shipno) {
        if (in_list(playernum, args[1], s, &nextshipno)) {
            if (s->type != STYPE_MINEF) {
                notify(playernum, governor, "That is not a mine.\n");
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            } else if (!s->on) {
                notify(playernum, governor, "The mine is not activated.\n");
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            } else if (s->docked || (s->whatorbits == LEVEL_SHIP)) {
                notify(playernum, governor, "The mine is docked or landed.\n");
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            free(s);
            do_mine(shipno, 1);
        } else {
            free(s);
        }

        shipno = do_shiplist(&s, &nextshipno);
    }
}

int retal_strength(shiptype *s)
{
    int strength = 0;
    int avail = 0;
    planettype *p;

    if (!s->alive) {
        return 0;
    }

    if (!Shipdata[s->type][ABIL_SPEED] && !landed(s)) {
        return 0;
    }

    /* Land based ships */
#ifndef USE_VN
    if (!s->popn) {
        return 0;
    }

    if (s->guns == PRIMARY) {
        if ((s->type == STYPE_FIGHTER) || (s->type == OTYPE_AFV)) {
            avail = s->primary;
            avail = MIN(s->popn, s->primary);
        }
    } else if (s->guns == SECONDARY) {
        if ((s->type == STYPE_FIGHTER) || (s->type == OTYPE_AFV)) {
            avail = s->secondary;
        } else {
            avail = MIN(s->popn, s->secondary);
        }
    } else {
        avail = 0;
    }

#else
    if (!s->popn && (s->type != OTYPE_BERS)) {
        return 0;
    }

    if (s->guns == PRIMARY) {
        if ((s->type == STYPE_FIGHTER)
            || (s->type == OTYPE_AFV)
            || (s->type == OTYPE_BERS)) {
            avail = s->primary;
        } else {
            avail = MIN(s->popn, s->secondary);
        }
    } else if (s->guns == SECONDARY) {
        if ((s->type == STYPE_FIGHTER)
            || (s->type == OTYPE_AFV)
            || (s->type == OTYPE_BERS)) {
            avail = s->secondary;
        } else {
            avail = MIN(s->popn, s->secondary);
        }
    } else {
        avail = 0;
    }
#endif

    avail = MIN(s->retaliate, avail);

    /*
     * HUT modification (tze)
     * Landed ships can use planetary stockpile directly
     */
    if ((avail > s->destruct) && landed(s)) {
        getplanet(&p, (int)s->storbits, (int)s->pnumorbits);

        strength = MIN(s->destruct + planets[s->storbits][s->pnumorbits]->info[s->owner - 1].destruct, avail);

        free(p);
    } else {
        strength = MIN(s->destruct, avail);
    }

    /*
     * fprintf(stderr,
     *         "\nn:0=%d\navail=%d\ns->destruct=%d\ns->owner=%d\nlanded=%d\n(%s/%s):p->destruct=%d\n",
     *         s->number,
     *         avail,
     *         s->destruct,
     *         s->owner,
     *         landed(s),
     *         Stars[s->storbits]->name,
     *         Stars[s->storbits]->pnames[s->pnumorbits],
     *         planets[s->storbits][s->pnumorbits]->info[s->owner - 1].destruct);
     *
     * strength = MIN(s->destruct, avail);
     */

    return strength;
}

int adjacent(int fx, int fy, int tx, int ty, planettype *p)
{
    if (abs(fy - ty) <= 1) {
        if (abs(fx - tx) <= 1) {
            return 1;
        } else if ((fx == (p->Maxx - 1)) && (tx == 0)) {
            return 1;
        } else if ((fx == 0) && (tx == (p->Maxx - 1))) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

int landed(shiptype *ship)
{
    if ((ship->whatdest == LEVEL_PLAN) && ship->docked) {
        return 1;
    } else {
        return 0;
    }
}

void check_overload(shiptype *ship, int cew, int *strength)
{
    if ((ship->laser && ship->fire_laser) || cew) {
        if (int_rand(0, *strength) > (int)(((1.0 - (0.01 * ship->damage)) * ship->tech) / 2.0)) {
            /* Check to see if the ship blows up */
            sprintf(buf,
                    "%s: Matter-antimatter EXPLOSION form overloaded crystal on %s.\n",
                    Dispshiploc(ship),
                    Ship(ship));

            kill_ship((int)ship->owner, ship);
            *strength = 0;
            warn((int)ship->owner, (int)ship->governor, buf);
            post(buf, COMBAT);

            notify_star((int)ship->owner,
                        (int)ship->governor,
                        0,
                        (int)ship->storbits,
                        buf);
        } else if (int_rand(0, *strength) > (int)(((1.0 - (0.01 * ship->damage)) * ship->tech) / 4.0)) {
            sprintf(buf,
                    "%s: Crystal damage from overloading on %s.\n",
                    Dispshiploc(ship),
                    Ship(ship));

            ship->fire_laser = 0;
            ship->mounted = 0;
            *strength = 0;
            warn((int)ship->owner, (int)ship->governor, buf);
        }
    }
}

void check_retal_strength(shiptype *ship, int *strength)
{
    *strength = 0;

    /* Irradiated ships don't retaliate */
    if (ship->active && ship->alive) {
        if (laser_on(ship)) {
            *strength = MIN(ship->fire_laser, (int)ship->fuel / 2);
        } else {
            *strength = retal_strength(ship);
        }
    }
}

int laser_on(shiptype *ship)
{
    if (ship->laser && ship->fire_laser) {
        return 1;
    } else {
        return 0;
    }
}
