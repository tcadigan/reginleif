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
 * land.c -- Land a ship. Also...dock -- Dock a ship with another
 * ship. And... assault -- A very un-PC version of land/dock
 *
 * #ident  "@(#)land.c  1.12 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/land.c,v 1.5 2007/07/06 18:06:56 gbp Exp $
 */
#include "land.h"

#include <math.h>
#include <stdlib.h>

#include "../server/buffers.h"
#include "../server/doship.h"
#include "../server/files_shl.h"
#include "../server/first.h"
#include "../server/GB_server.h"
#include "../server/getplace.h"
#include "../server/lists.h"
#include "../server/max.h"
#include "../server/moveship.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/rand.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/shlmisc.h"
#include "../server/vars.h"

#include "fire.h"
#include "load.h"
#include "shootblast.h"
#include "tele.h"

extern long Shipdata[NUMSTYPES][NUMABILS];
static int roll;

extern void land(int, int, int);
extern int crash(shiptype *, double);
extern int docked(shiptype *);
extern int overloaded(shiptype *);

void land(int playernum, int governor, int apcount)
{
    shiptype *s;
    shiptype *s2;
    planettype *p;
    sectortype *sect;
    int shipno;
    int ship2no;
    int x = -1;
    int y = -1;
    int i;
    int numdest = -1;
    int strength = 0;
    int nupdates;
    double fuel;
    double dist;
    racetype *race;
    racetype *alien;
    int nextshipno;

    if (argn < 2) {
        notify(playernum, governor, "Land what?\n");

        return;
    }

    nextshipno = start_shiplist(playernum, governor, args[1]);
    shipno = do_shiplist(&s, &nextshipno);

    while (shipno) {
        if (in_list(playernum, args[1], s, &nextshipno)) {
            if (overloaded(s)) {
                sprintf(buf, "%s is too overloaded to land.\n", Ship(s));
                notify(playernum, governor, buf);
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (!Shipdata[s->type][ABIL_CANDOCK]) {
                sprintf(buf,
                        "This ship is not equipped to be landed or docked.\n");

                notify(playernum, governor, buf);
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (s->type == OTYPE_QUARRY) {
                notify(playernum,
                       governor,
                       "You can't load quarries onto ships.\n");

                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (docked(s)) {
                notify(playernum,
                       governor,
                       "That ship is docked to another ship.\n");

                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (args[2][0] == '#') {
                /*
                 * Attempting to land on a friendly ship (for
                 * carriers/stations/etc.)
                 */
                sscanf(args[2] + 1, "%d", &ship2no);

                if (!getship(&s2, ship2no)) {
                    sprintf(buf, "Ship #%d wasn't found.\n", ship2no);
                    notify(playernum, governor, buf);
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (testship(playernum, governor, s2)) {
                    notify(playernum, governor, "Illegal format.\n");
                    free(s);
                    free(s2);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (s2->type == OTYPE_FACTORY) {
                    notify(playernum, governor, "Can't land on factories.\n");
                    free(s);
                    free(s2);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (landed(s)) {
                    if (!landed(s2)) {
                        sprintf(buf,
                                "%s is not landed on a planet.\n",
                                Ship(s2));

                        notify(playernum, governor, buf);
                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    if (s2->storbits != s->storbits) {
                        notify(playernum,
                               governor,
                               "These ships are not in the same star system.\n");

                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    if (s2->pnumorbits != s->pnumorbits) {
                        notify(playernum,
                               governor,
                               "These ships are not landed on the same planet.\n");

                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    if ((s2->land_x != s->land_x)
                        || (s2->land_y != s->land_y)) {
                        notify(playernum,
                               governor,
                               "These ships are not in the same sector.\n");

                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    if (s->on) {
                        sprintf(buf,
                                "%s must be turned off before loading.\n",
                                Ship(s));

                        notify(playernum, governor, buf);
                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    if (s->size > (s2->max_hanger - s2->hanger)) {
                        sprintf(buf,
                                "Mothership does not have %d hanger space available to load ship.\n",
                                s->size);

                        notify(playernum, governor, buf);
                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    /* OK, load them up */
                    remove_sh_plan(s);

                    /*
                     * Get the target ship again because it had a pointer
                     * changed (and put to disk) in the remove routines
                     */
                    free(s2);
                    getship(&s2, ship2no);

                    insert_sh_ship(s, s2);

                    /* Increase mass of mothership */
                    s2->mass += s->mass;
                    s2->hanger += (unsigned short)s->size;
                    fuel = 0.0;

                    sprintf(buf,
                            "%s loaded onto %s using %.1f fuel.\n",
                            Ship(s),
                            Ship(s2),
                            fuel);

                    notify(playernum, governor, buf);

                    s->docked = 1;
                    clearhyper(s);

                    putship(s2);
                    free(s2);
                } else if (s->docked) {
                    sprintf(buf, "%s is already docked or landed.\n", Ship(s));
                    notify(playernum, governor, buf);
                    free(s);
                    free(s2);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                } else {
                    /*
                     * Check if the ships are in the same scope level. Maarten
                     */
                    if (s->whatorbits != s2->whatorbits) {
                        notify(playernum,
                               governor,
                               "Those ships are not in the same scope.\n");

                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    /* Check to see if close enough to land */
                    dist = sqrt((double)Distsq(s2->xpos, s2->ypos, s->xpos, s->ypos));

                    if (dist > DIST_TO_DOCK) {
                        sprintf(buf,
                                "%s must be %.2f or closer to %s.\n",
                                Ship(s),
                                DIST_TO_DOCK,
                                Ship(s2));

                        notify(playernum, governor, buf);
                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    fuel = 0.05 + (dist * 0.025 * sqrt(s->mass));

                    if (s->fuel < fuel) {
                        sprintf(buf, "Not enough fuel.\n");
                        notify(playernum, governor, buf);
                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    if (s->size > (s2->max_hanger - s2->hanger)) {
                        sprintf(buf,
                                "Mothership does not have %d hanger space available to load ship.\n",
                                s->size);

                        notify(playernum, governor, buf);
                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    use_fuel(s, fuel);

                    /* Remove the ship from whatever scope it is currently in */
                    if (s->whatorbits == LEVEL_PLAN) {
                        remove_sh_plan(s);
                    } else if (s->whatorbits == LEVEL_STAR) {
                        remove_sh_star(s);
                    } else {
                        notify(playernum,
                               governor,
                               "Ship is not in planet or star scope.\n");

                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    /*
                     * Get the target ship again because it had a pointer
                     * changed (and put to disk) in the remove routines
                     */
                    free(s2);
                    getship(&s2, ship2no);

                    insert_sh_ship(s, s2);

                    /* Increases mass of mothership */
                    s2->mass += s->mass;
                    s2->hanger += (unsigned short)s->size;

                    sprintf(buf,
                            "%s landed on %s using %.1f fuel.\n",
                            Ship(s),
                            Ship(s2),
                            fuel);

                    notify(playernum, governor, buf);

                    clearhyper(s);
                    s->docked = 1;

                    putship(s2);
                    free(s2);
                }
            } else {
                /* Attempting to land on a planet */
                if (s->docked) {
                    sprintf(buf, "%s is docked.\n", Ship(s));
                    notify(playernum, governor, buf);
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                sscanf(args[2], "%d,%d", &x, &y);

                if (s->whatorbits != LEVEL_PLAN) {
                    sprintf(buf, "%s doesn't orbit a planet.\n", Ship(s));
                    notify(playernum, governor, buf);
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (!Shipdata[s->type][ABIL_CANLAND]) {
                    sprintf(buf, "This ship is not equipped to land.\n");
                    notify(playernum, governor, buf);
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if ((s->storbits != Dir[playernum - 1][governor].snum)
                    || (s->pnumorbits != Dir[playernum - 1][governor].pnum)) {
                    sprintf(buf, "You have to cs to the planet it orbits.\n");
                    notify(playernum, governor, buf);
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (!s->max_speed) {
                    sprintf(buf, "This ship is not rated for maneuvering.\n");
                    notify(playernum, governor, buf);
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (!enufAP(playernum, governor, Stars[s->storbits]->AP[playernum - 1], apcount)) {
                    sprintf(buf, "Not enough APs.\n");
                    notify(playernum, governor, buf);
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                getplanet(&p, (int)s->storbits, (int)s->pnumorbits);

                /* Handle wormhole */
                if (p->type == TYPE_WORMHOLE) {
                    free(p);
                    go_thru_wormhole(s);
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                sprintf(buf,
                        "Planet /%s/%s has gravity field of %.2f.\n",
                        Stars[s->storbits]->name,
                        Stars[s->storbits]->pnames[s->pnumorbits],
                        gravity(p));

                notify(playernum, governor, buf);

                dist = sqrt((double)Distsq(Stars[s->storbits]->xpos + p->xpos,
                                           Stars[s->storbits]->ypos + p->ypos,
                                           s->xpos,
                                           s->ypos));

                sprintf(buf, "Distance to planet: %.2f.\n", dist);
                notify(playernum, governor, buf);

                if (dist > DIST_TO_LAND) {
                    sprintf(buf,
                            "%s must be %.3g or closer to the planet (%.2f)\n",
                            Ship(s),
                            DIST_TO_LAND,
                            dist);

                    notify(playernum, governor, buf);
                    free(s);
                    free(p);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                fuel = s->mass * gravity(p) * LAND_GRAV_MASS_FACTOR;

                if ((x < 0)
                    || (y < 0)
                    || (x > (p->Maxx - 1))
                    || (y > (p->Maxy - 1))) {
                    sprintf(buf, "Illegal coordinates.\n");
                    notify(playernum, governor, buf);
                    free(s);
                    free(p);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                /* Handle FIRST_COMBAT rule here */
                nupdates = get_num_updates();

                for (i = 1; i <= Num_races; ++i) {
                    if ((i != playernum) && p->info[i - 1].popn) {
                        race = races[playernum - 1];
                        alien = races[i - 1];

                        if (alien->Guest) {
                            notify(playernum,
                                   governor,
                                   "Can't land on a Guest's planet.\n");

                            free(s);
                            free(p);
                            shipno = do_shiplist(&s, &nextshipno);

                            continue;
                        }

                        if (nupdates < CombatUpdate) {
                            /*
                             * If combat is not enabled yet, you cannot even
                             * land on an alien planet
                             */
                            if (!isset(race->allied, i)
                                || !isset(alien->allied, playernum)) {
                                sprintf(buf,
                                        "Can only land on ally's planets before Combat enabled.\nCombat enabled at update [%d]\n",
                                        CombatUpdate);

                                notify(playernum, governor, buf);
                                free(s);
                                free(p);
                                shipno = do_shiplist(&s, &nextshipno);

                                continue;
                            }
                        }
                    }
                }

#ifdef DEFENSE
                /*
                 * People who have declared war on you will fire at your landing
                 * ship
                 */

                /*
                 * HUTm (kse) Enslaved planets do not shoot at enslaver's ships
                 */
                if (p->slaved_to != s->owner) {
                    for (i = 1; i <= Num_races; ++i) {
                        if (s->alive
                            && (i != playernum)
                            && p->info[i - 1].popn
                            && p->info[i - 1].guns
                            && p->info[i - 1].destruct) {
                            alien = races[i - 1];

                            if (isset(alien->atwar, (int)s->owner)) {
                                /* Attack the landing ship */
                                strength = MIN((int)p->info[i - 1].guns,
                                               (int)p->info[i - 1].destruct);

                                if (strength) {
                                    shoot_planet_to_ship(alien,
                                                         p,
                                                         s,
                                                         strength,
                                                         buf,
                                                         temp);

                                    post(temp, COMBAT);
                                    notify_star(0,
                                                0,
                                                (int)s->owner,
                                                (int)s->storbits,
                                                temp);

                                    warn(i,
                                         (int)Stars[s->storbits]->governor[i - 1],
                                         buf);

                                    notify((int)s->owner,
                                           (int)s->governor,
                                           buf);

                                    p->info[i - 1].destruct -= strength;
                                }
                            }
                        }

                        if (!s->alive) {
                            putplanet(p, (int)s->storbits, (int)s->pnumorbits);
                            putship(s);
                            free(p);
                            free(s);

                            return;
                        }
                    }
                }
#endif

                if (s->damage) {
                    sprintf(buf,
                            "Attempting to land ship with %d%% damage...\n",
                            (int)s->damage);

                    notify(playernum, governor, buf);
                }

                /*
                 * Check to see if the ship crashes from lack of fuel or damage
                 */
                if (crash(s, fuel)) {
                    /* Damaged ships stand a chance of crash landing */
                    if (roll) {
                        sprintf(buf, "You rolled a %d!\n", roll);
                    } else {
                        sprintf(buf,
                                "You had %.1ff while the landing required %.1ff\n",
                                s->fuel,
                                fuel);
                    }

                    notify(playernum, governor, buf);

                    numdest = shoot_ship_to_planet(s,
                                                   p,
                                                   round_rand((double)s->destruct / 3.0),
                                                   x,
                                                   y,
                                                   1,
                                                   0,
                                                   HEAVY,
                                                   long_buf,
                                                   short_buf);

                    if (numdest > 0) {
                        sprintf(buf,
                                "BOOM!! %s crashes on sector %d,%d with blast radius of %d.\n",
                                Ship(s),
                                x,
                                y,
                                numdest);
                    } else {
                        sprintf(buf,
                                "BOOM!! %s crashed on sector %d,%d with no collateral damage.\n",
                                Ship(s),
                                x,
                                y);
                    }

                    for (i = 1; i <= Num_races; ++i) {
                        if (p->info[i - 1].numsectsowned || (i == playernum)) {
                            warn(i,
                                 (int)Stars[s->storbits]->governor[i - 1],
                                 buf);
                        }
                    }

                    kill_ship((int)s->owner, s);
                } else {
                    s->land_x = x;
                    s->land_y = y;
                    s->xpos = p->xpos + Stars[s->storbits]->xpos;
                    s->ypos = p->ypos + Stars[s->storbits]->ypos;
                    use_fuel(s, fuel);
                    s->docked = 1;
                    clearhyper(s);
                    /* No destination */
                    s->whatdest = LEVEL_PLAN;
                    s->deststar = s->storbits;
                    s->destpnum = s->pnumorbits;

                    if (getsector(&sect, p, x, y)) {
                        if (sect->condition == WASTED) {
                            sprintf(buf,
                                    "Warning: That sector is a wasteland!\n");

                            notify(playernum, governor, buf);
                        } else if (sect->owner && (sect->owner != playernum)) {
                            race = races[playernum - 1];
                            alien = races[sect->owner - 1];

                            if (!isset(race->allied, sect->owner)
                                || !isset(alien->allied, playernum)) {
                                sprintf(buf,
                                        "You have landed on an alien sector (%s).\n",
                                        alien->name);

                                notify(playernum, governor, buf);
                            } else {
                                sprintf(buf,
                                        "You have landed on allied sector (%s).\n",
                                        alien->name);

                                notify(playernum, governor, buf);
                            }
                        }
                    } else {
                        notify(playernum,
                               governor,
                               "error in sector database, ship not landed, notify deity.\n");

                        return;
                    }

                    if (s->whatorbits == LEVEL_UNIV) {
                        deductAPs(playernum, governor, apcount, 0, 1);
                    } else {
                        deductAPs(playernum,
                                  governor,
                                  apcount,
                                  (int)s->storbits,
                                  0);
                    }

                    putplanet(p, (int)s->storbits, (int)s->pnumorbits);
                    putsector(sect, p, x, y);

                    /* Send messages to anyone there */
                    sprintf(buf,
                            "%s observed landing on sector %d,%d,planet /%s/%s.\n",
                            Ship(s),
                            s->land_x,
                            s->land_y,
                            Stars[s->storbits]->name,
                            Stars[s->storbits]->pnames[s->pnumorbits]);

                    for (i = 1; i <= Num_races; ++i) {
                        if (p->info[i - 1].numsectsowned && (i != playernum)) {
                            notify(i,
                                   (int)Stars[s->storbits]->governor[i - 1],
                                   buf);
                        }
                    }

                    sprintf(buf, "%s landed on planet.\n", Ship(s));
                    notify(playernum, governor, buf);
                    free(sect);
                }

                free(p);
            }

            putship(s);
        }

        free(s);
        shipno = do_shiplist(&s, &nextshipno);
    }
}

int crash(shiptype *s, double fuel)
{
    roll = 0;

    if (s->fuel < fuel) {
        return 1;
    } else {
        roll = int_rand(1, 100);

        if (roll <= (int)s->damage) {
            return 1;
        } else {
            return 0;
        }
    }
}

int docked(shiptype *s)
{
    return (s->docked && (s->whatdest == LEVEL_SHIP));
}

int overloaded(shiptype *s)
{
    int max_resource = 0;
    int max_fuel = 0;
    int max_destruct;

    /*
     * HUT modification (tze): Ship is considered overloaded if there's more
     * than 1 fuel over capacity
     */

    if (s->type == OTYPE_FACTORY) {
        max_resource = Shipdata[s->type][ABIL_CARGO];
        max_fuel = Shipdata[s->type][ABIL_FUELCAP];
        max_destruct = Shipdata[s->type][ABIL_DESTCAP];
    } else {
        max_resource = s->max_resource;
        max_fuel = s->max_fuel;
        max_destruct = s->max_destruct;
    }

    return ((s->resource > max_resource)
            || (s->fuel > (max_fuel + 1))
            || ((s->popn + s->troops) > s->max_crew)
            || (s->destruct > max_destruct));
}

int wormhole_damage(int here, int there)
{
    /*
     * Return maximum possible damage for traversal of this wormhole. Based on
     * travel distance.
     */
    int dmg;
    startype *h;
    startype *t;
    double tdist;

    h = Stars[here];
    t = Stars[there];

    tdist = sqrt((double)Distsq(h->xpos, t->ypos, t->xpos, t->ypos));
    dmg = (int)((tdist / UNIVSIZE) * 200);

    return dmg;
}

void go_thru_wormhole(shiptype *ship)
{
    startype *here;
    startype *there;
    int h;
    int t;
    int damage = 0;
    int maxdamage;
    planettype *dpl;

    if (ship->used_wormhole) {
        sprintf(buf,
                "That ship already went through a wormhole this segment.\n");

        notify(ship->owner, ship->governor, buf);
    }

    h = ship->storbits;
    here = Stars[h];
    t = here->wh_dest_starnum;
    there = Stars[t];

    getplanet(&dpl, t, (int)there->numplanets - 1);

    if (ship->tech < TECH_WORMHOLE) {
        maxdamage = wormhole_damage(h, t);
        damage = (int)((1 - (ship->tech / TECH_WORMHOLE)) * maxdamage);
        ship->damage += damage;
    }

    if (damage) {
        sprintf(buf,
                "Ship[%d] incurred %d%% damage\n",
                ship->number,
                damage);

        notify(ship->owner, ship->governor, buf);
    }

    if (ship->damage > 100) {
        sprintf(buf, "Damage destroyed ship\n");
        kill_ship((int)ship->owner, ship);

        return;
    }

    remove_sh_plan(ship);
    ship->storbits = t;
    ship->pnumorbits = there->numplanets - 1;
    ship->xpos = there->xpos + dpl->xpos;
    ship->ypos = there->ypos + dpl->ypos;
    ship->protect.planet = 0;
    insert_sh_plan(dpl, ship);
    ship->used_wormhole = 1;
    StarsInhab[ship->storbits] = 1;
    setbit(there->inhabited, ship->owner);
    setbit(there->explored, ship->owner);
    dpl->explored = 1;
    dpl->info[ship->owner - 1].explored = 1;
    ship->whatdest = LEVEL_PLAN;
    ship->deststar = ship->storbits;
    ship->destpnum = ship->pnumorbits;

    putship(ship);
    putplanet(dpl, (int)ship->storbits, (int)ship->pnumorbits);
    putstar(there, t);

    sprintf(buf,
            "%s has traversed a wormhole!\nShip is at: %s\n",
            Ship(ship),
            prin_ship_orbits(ship));

    notify(ship->owner, ship->governor, buf);
}
