/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, (or at your option) any later
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
 * doship -- Do one ship turn.
 *
 * #ident  "%W% %G% %Q%"
 *
 * $Header: /var/cvs/gbp/GB+/server/doship.c,v 1.6 2007/07/06 17:18:28 gbp EXP $
 *
 * static char *ver = "@{#}        $RCSfile: doship.c,v $ $Revision: 1.6 $";
 */
#include "doship.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "debug.h"
#include "doturn.h"
#include "power.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

extern long Shipdata[NUMSTYPES][NUMABILS];

void doship(shiptype *, int);
void domass(shiptype *);
void doown(shiptype *);
void domissile(shiptype *);
void do_mine(int, int);
void do_sweeper(int);
void doabm(shiptype *);
void do_repair(shiptype *);
void do_habitat(shiptype *);
void do_pod(shiptype *);
int infect_planet(int, int, int, int);
void do_meta_infect(int, planettype *);
void do_canister(shiptype *);
void do_greenhouse(shiptype *);
void do_god(shiptype *);
void gastype(int, char *);
double crew_factor(shiptype *);
double ap_planet_factor(planettype *);
void do_oap(shiptype *);
void doloc(shiptype *);

void doship(shiptype *ship, int update)
{
    racetype *race;
    shiptype *ship2;
    planettype *planet;
    int aused;
    int setting;
    int armor;
    int max_crew;
    int vn_flag;
    double cloak_fuel;

    /* Ship is active */
    ship->active = 1;

    if (!ship->owner) {
        ship->alive = 0;
    }

    if (update) {
        ++ship->age;
    }

#ifdef USE_VN
    if ((ship->type != OTYPE_VN) && (ship->type != OTYPE_BERS)) {
        vn_flag = 1;
    } else {
        vn_flag = 0;
    }

#else

    vn_flag = 1;
#endif

    /* Ship aging code from HAP -mfw */
    if (update
        && ship->alive
        && (!Shipdata[ship->type][ABIL_HASSWITCH]
            || (Shipdata[ship->type][ABIL_HASSWITCH] && ship->on))
        && (ship->owner != 1)
        && (ship->popn || ship->troops)
        && (ship->type != OTYPE_GOV)
        && (ship->type != OTYPE_FACTORY)
        && vn_flag
        && ship->max_resource
        && (ship->age > SHIP_TOO_OLD)) {
        ship->damage += int_rand(0, ship->age - SHIP_TOO_OLD);

        if (ship->damage >= 100) {
            sprintf(buf,
                    "%s: %s %c%d %s lost due to old age and wear.\n",
                    prin_ship_orbits(ship),
                    Shipnames[ship->type],
                    Shipltrs[ship->type],
                    ship->number,
                    ship->name);

            push_telegram(ship->owner, ship->governor, buf);
            kill_ship(ship->owner, ship);
        }

        if (ship->alive && (ship->age == SHIP_TOO_OLD)) {
            sprintf(buf,
                    "%s: %s %%d %s is showing moderate signs of wear,\nupgrade its tech soon.\n",
                    prin_ship_orbits(ship),
                    Shipnames[ship->type],
                    Shipltrs[ship->type],
                    ship->number,
                    ship->name);

            push_telegram(ship->owner, ship->governor, buf);
        } else if (ship->alive && (ship->age > (2 * SHIP_TOO_OLD))) {
            sprintf(buf,
                    "%s: %s %c %d %s is showing extreme signs of wear,\nit needs a tech upgrade or replacing.\n",
                    printship_orbits(ship),
                    Shipnames[ship->type],
                    Shipltrs[ship->type],
                    ship->number,
                    ship->name);

            push_telegram(ship->owner, ship->governor, buf);
        }
    } /* End of aging code */

    if (ship->alive) {
        /* Repair radiation */
        if (ship->rad) {
            ship->active = 1;

            /*
             * Irradiated ships are immobile...
             * Kill of some people.
             * check to see if ship is active
             */
            if (success(ship->rad)) {
                ship->active = 0;
            }

            if (update) {
                ship->popn = round_rand(ship->popn * 0.80);
                ship->troops = round_rand(ship->troops * 0.80);

                if (ship->rad >= (int)REPAIR_RATE) {
                    ship->rad -= int_rand(0, (int)REPAIR_RATE);
                } else {
                    ship->rad -= int_rand(0, (int)ship->rad);
                }
            }
        } else {
            ship->active = 1;
        }

        if (ship->type == OTYPE_FACTORY) {
            max_crew = Shipdata[ship->type][ABIL_MAXCREW] - ship->troops;
        } else {
            max_crew = ship->max_crew - ship->troops;
        }

        if (!ship->popn && max_crew && !ship->docked) {
            ship->whatdest = LEVEL_UNIV;
        }

        if ((ship->whatorbits != LEVEL_UNIV)
            && (Stars[ship->storbits]->nova_stage > 0)) {
            /*
             * Damage ships from supernovae
             *
             * Maarten: Modified to take into account MOVES_PER_UPDATE
             */
            if (ship->type == OTYPE_FACTORY) {
                armor = Shipdata[ship->type][ABIL_ARMOR];
            } else {
                armor = (ship->armor * (100 - ship->damage)) / 100;
            }

            ship->damage += ((5 * Stars[storbits]->nova_stage)
                             / ((armor + 1) * segments));

            if (ship->damage >= 100) {
                kill_ship((int)ship_owner, ship);

                return;
            }
        }

        if ((ship->type == OTYPE_FACTORY) && !ship->on) {
            race = races[ship->owner - 1];
            ship->tech = race->tech;
        }

        if (ship->active) {
            Moveship(ship, update, 1, 0);
            doloc(ship);
        }

        ship->size = ship_size(ship); /* For debugging */

        if (ship->whatorbits == LEVEL_SHIP) {
            getship(&ship2, (int)ship->destshipno);

            if (ship2->owner != ship->owner) {
                ship2->owner = ship->owner;
                ship2->governor = ship->governor;
                putship(ship2);
            }

            free(ship2); /* Just making sure */
        } else if ((ship->whatorbits != LEVEL_UNIV)
                   && ((ship->popn || ship->troops)
                       || (ship->type == OTYPE_PROBE))) {
            /*
             * Though I have often used TWCs for exploring, I don't think it is
             * right to be able to map out worlds with this type of junk. Either
             * a manned ship or a probe, which is designed for this kind of
             * work.  Maarten
             */
            StarsInhab[ship->storbits] = 1;
            setbit(Stars[ship->storbits]->inhabited, ship->owner);
            setbit(Stars[ship->storbits]->explored, ship->owner);

            if (ship->whatorbits == LEVEL_PLAN) {
                planets[ship->storbits][ship->pnumorbits]->info[ship->owner - 1].explored = 1;
            }
        }

        /* Add ships, popn to total count to add APs */
        if (update) {
            ++Power[ship->owner - 1].ships_owned;
            Power[ship->owner - 1].resource += ship->resource;
            Power[ship->owner - 1].fuel += ship->fuel;
            Power[ship->owner - 1].destruct += ship->destruct;
            Power[ship->owner - 1].popn += ship->popn;
            Power[ship->owner - 1].troops += ship->troops;
        }

        if (ship->whatorbits == LEVEL_UNIV) {
            ++Sdatanumships[ship->owner - 1];
            Sdatapopns[ship->owner] += ship->popn;
        } else {
            ++starnumships[ship->storbits][ship->owner - 1];
            /* Add popn of ships to popn */
            starpopns[ship->storbits][ship->owner - 1] += ship->popn;
            /* Set inhabited for ship only if manned or probe.  Maarten */
            if (ship->popn || ship->troops || (ship->type == OTYPE_PROBE)) {
                StarsInhab[ship->storbits] = 1;
                setbit(Stars[ship->storbits]->inhabited, ship->owner);
                setbit(Stars[ship->storbits]->explored, ship->owner);
            }
        }

        /*
         * Added in for blowing up Waste canisters. Moved over from
         * treehouse. -mfw
         */
        if (update && (ship->type == OTYPE_TOXWC)) {
            if (int_rand(1, 100) <= (TOXCAN_LEAK * ship->age)) {
                sprintf(telegram_buf,
                        "%s is leaking and had to be destroyed as %s.",
                        Ship(ship),
                        prin_ship_orbits(ship));

                push_telegram(ship->owner, ship->governor, telegram_buf);

                if (ship->whatorbits == LEVEL_PLAN) {
                    getplanet(&planet, (int)ship->storbits, (int)ship->pnumorbits);
                    planet->conditions[TOXIC] += ship->special.waste.toxic;
                    putplanet(planet, (int)ship->storbits, (int)ship->pnumorbits);
                    free(planet);
                }

                kill_ship(ship->owner, ship);
            }
        } /* End of waste blowup code */

        if(ship->active) {
#ifdef USE_VN
            if (ship->type != OTYPE_BERS) {
                vn_flag = 1;
            } else {
                vn_flag = 0;
            }

#else

            vn_flag = 1;
#endif

            /* Bombard the planet */
            if (((Shipdata[ship->type][ABIL_GUNS] || Shipdata[ship->type][ABIL_LASER])
                 && (ship->type != STYPE_MINEF))
                && ship->bombard
                && (ship->whatorbits == LEVEL_PLAN)
                && (ship->whatdest == LEVEL_PLAN)
                && (ship->deststar == ship->storbits)
                && (ship->destpnum == ship->pnumorbits)
                && vn_flag) {
                /*
                 * Ship bombards planet
                 *
                 * Bombard(ship,
                 *         shipno,
                 *         planets[ship->storbits][ship->pnumorbits],
                 *         races[ship->owner - 1]);
                 */
                Stinfo[ship->storbits][ship->pnumorbits].inhab = 1;
            }

            /*
             * Repair ship by the amount of crew it has. Industrial complexes
             * can repair (robot ships and offline factories can't repair)
             */
#ifdef USE_VN
            if (ship->type == OTYPE_FACTORY) {
                if (ship->damage
                    && (ship->on
                        || (ship->type == OTYPE_VN)
                        || (ship->type == OTYPE_BERS))) {
                    do_repair(ship);
                }
            } else if (ship->damage
                       && (max_crew
                           || (ship->type == OTYPE_VN)
                           || (ship->type == OTYPE_BERS))) {
                do_repair(ship);
            }

#else

            if (ship->type == OTYPE_FACTORY) {
                if (ship->damage && ship->on) {
                    do_repair(ship);
                }
            } else if (ship->damage && max_crew) {
                do_repair(ship);
            }
#endif

            /* Cloaking code -mfw */
            if (ship->cloaked) {
                if (!ship->cloaking) {
                    ship->cloaked = 0;
                    sprintf(buf,
                            "%s [%d] %s has uncloaked.\n",
                            Shipnames[ship->type],
                            ship->number,
                            ship->name);

                    push_telegram(ship->owner, ship->governor, buf);
                } else {
                    cloak_fuel = (ship->max_fuel * CLOAK_FUEL_CONSUMPTION * TECH_CLOAK) / ship->tech;

                    if (cloak_fuel > ship->fuel) {
                        ship->cloaked = 0;
                        ship->cloaking = 0;

                        sprintf(buf,
                                "%s [%d] %s has uncloaked due to lack of fuel.\n",
                                Shipnames[ship->type],
                                ship->number,
                                ship->name);

                        push_telegram(ship->owner, ship->governor, buf);
                    } else {
                        use_fuel(ship, cloak_fuel);
                    }
                } /* If !activate */
            } else if (Shipdata[ship->type][ABIL_CLOAK] && ship->cloak) {
                if (ship->cloaking) {
                    if (ship->mounted) {
                        cloak_fuel = (ship->max_fuel * CLOAK_FUEL_CONSUMPTION * TECH_CLOAK) / ship->tech;

                        if (cloak_fuel > ship->fuel) {
                            ship->cloaked = 0;
                            ship->cloaking = 0;

                            sprintf(buf,
                                    "%s [%d] %s does not have enough fuel to cloak.\n",
                                    Shipnames[ship->type],
                                    ship->number,
                                    ship->name);

                            push_telegram(ship->owner, ship->governor, buf);
                        } else {
                            ship->cloaked = 1;

                            sprintf(buf,
                                    "%s [%d] %s has cloaked.\n",
                                    Shipnames[ship->type],
                                    ship->number,
                                    ship->name);

                            push_telegram(ship, cloak_fuel);
                        } /* Fuel ok */
                    } else {
                        ship->cloaking = 0;

                        sprintf(buf,
                                "%s [%d] %s cannot cloak, check crystal.\n",
                                Shipnames[ship->type],
                                ship->number.
                                ship->name);

                        push_telegram(ship->owner, ship->governor, buf);
                    } /* Else no mount */
                } /* Cloaking */
            } /* Else no cloak ability */
            /* End cloak code */

#ifdef USE_AMOEBA
            if (ship->type == OTYPE_AMOEBA) {
                do_amoeba(ship);
            }
#endif

#ifdef USE_VN
            /* Von Neumann machine */
            if ((ship->type == OTYPE_VN) || (ship->type == OTYPE_BERS)) {
                do_vn(ship);
            }
#endif

            if (update) {
                /* Do this stuff during updates only */
                switch (ship->type) {
                case OTYPE_INF: /* Infrastructure ship */
                    /* atmos */
                    setting = ship->inf.atmos_setting;

                    while (setting) {
                        aused = do_ap(ship);

                        if (!aused) {
                            setting = 0;
                        } else {
                            --setting;
                        }
                    }

                    break;
                case OTYPE_SETI:
                    do_seti(ship);

                    break;
                case OTYPE_CANIST:
                    do_canister(ship);

                    break;
                case OTYPE_GREEN:
                    do_greenhouse(ship);

                    break;
                case STYPE_MIRROR:
                    do_mirror(ship);

                    break;
                case OTYPE_AP:
                    do_ap(ship);

                    break;
                case STYPE_OAP:
                    do_oap(ship);

                    break;
                case OTYPE_OMCL:
                    do_omcl(ship);

                    break;
                case STYPE_HABITAT:
                    do_habitat(ship);

                    break;
                case STYPE_GOD:
                    do_god(ship);

                    break;
                default:

                    break;
                }
            }

            if (ship->type) {
                do_pod(ship);
            }
        }
    }
}

void doloc(shiptype *ship)
{
    unsigned long sh;

    sh = ship->ships;

    while (sh) {
        if (valid_ship(sh)) {
            ships[sh]->storbits = ship->storbits;
            ships[sh]->pnumorbits = ship->pnumorbits;
            doloc(ships[sh]);
            sh = nextship(ships[sh]);
        }
    }
}

void domass(shiptype *ship)
{
    double rmass;
    int sh;
    shiptype *shipl = NULL;

    /*
     * XXX We've core dumped on the next line. The error in the logfile was
     * "DATA Error: Ship 4 Owner(gov) Unknown" and gdb showed it dumped core
     * here because ship->owner was 0 (null). Seems somehow the owner got fouled
     * up before the call to this function. 3/16/05 -mfw
     *
     * Okay it happened again three times in a row on 7/23/05, except owner was
     * '(' and ship->number was WAY off. However, the database on disk seems
     * intact. Some kind of memory overwrite...I put a hack-check in before
     * doturn() calls this function. -mfw
     */
    rmass = races[ship->owner - 1]->mass;
    sh = ship->ships;
    ship->mass = 0.0;
    ship->hanger = 0;

    while (sh) {
        if (valid_ship(sh)) {
            getship(&shipl, sh);
            domass(shipl); /* Recursive call */
            ship->mass += shipl->mass;
            ship->hanger += shipl->size;
            sh = nextship(shipl);
            free(shipl);
            shipl = NULL;
        }
    }

    if (shipl) {
        free(shipl);
    }

    ship->mass += getmass(ship);
    ship->mass += ((double)(ship->popn + ship->troops) * rmass);
    ship->mass += ((double)ship->destruct * MASS_DESTRUCT);
    ship->mass += (ship->fuel * MASS_FUEL);
    ship->mass += ((double)ship->resource * MASS_RESOURCE);
}

void doown(shiptype * ship)
{
    int sh;

    sh = ship->ships;

    while (sh) {
        if (valid_ship(sh)) {
            ships[sh]->owner = ship->owner;
            ships[sh]->governor = ship->governor;
            doown(ships[sh]); /* Recursive call */
            sh = nextship(ships[sh]);
        }
    }
}

void domissile(shiptype *ship)
{
    int sh2;
    int bombx;
    int bomby;
    int numdest;
    int pdn;
    int i;
    planettype *p;
    double dist;
    placetype where;

    if (!ship->alive || !ship->owner) {
        return;
    }

    if (!ship->on || ship->docked) {
        return;
    }

    if (get_num_updates() < CombatUpdate) {
        sprintf(long_buf,
                "missiles won't work until after Combat Update [%d]\nRE: ship#[%d]\n",
                CombatUpdate,
                ship->number);

        push_telegram((int)ship->owner, (int)ship->governor, long_buf);

        return;
    }

    /* Check to see if it has arrived at it's destination */
    if ((ship->whatdest == LEVEL_PLAN)
        && (ship->whatorbits == LEVEL_PLAN)
        && (ship->destpnum == ship->pnumorbits)) {
        p = planets[ship->storbits][ship->pnumorbits];
        /* Check to see if PDNs are present */
        pdn = 0;
        sh2 = p->ships;

        while (sh2 && !pdn) {
            if (!valid_ship(sh2)) {
                sh2 = 0;
                continue;
            }

            if (ships[sh2]->alive
                && ships[sh2]->on
                && (ships[sh2]->type == OTYPE_PLANDEF)) {
                /* Attack the PDN instead */
                /* Move missile to PDN for attack */
                ship->whatdest = LEVEL_SHIP;
                ship->xpos = ships[sh2]->xpos;
                ship->ypos = ships[sh2]->ypos;
                ship->destshipno = sh2;
                pdn = sh2;
            }

            sh2 = nextship(ships[sh2]);
        }

        if (!pdn) {
            if (ship->special.impact.scatter) {
                bombx = int_rand(1, (int)p->Maxx) - 1;
                bomby = int_rand(1, (int)p->Maxy) - 1;
            } else {
                bombx = ship->special.impact.x % p->Maxx;
                bomby = ship->special.impact.y % p->Maxy;
            }

            where.level = LEVEL_PLAN;
            where.snum = ship->storbits;
            where.pnum = ship->pnumorbits;

            numdest = shoot_ship_to_planet(ship,
                                           p,
                                           (int)ship->destruct,
                                           bombx,
                                           bomby,
                                           1,
                                           0,
                                           HEAVY,
                                           long_buf,
                                           short_buf);

            sprintf(buf,
                    "%s dropped on target %s (%d,%d)\n\t%d sectors destroyed.",
                    Ship(ship),
                    prin_ship_orbits(ship),
                    bombx,
                    bomby,
                    numdest);

            push_telegram((int)ship->owner, (int)ship->governor, buf);

            sprintf(buf,
                    "%s dropped on colony %s (%d,%d)\n\t%d sectors destroyed.",
                    Ship(ship),
                    prin_ship_orbits(ship),
                    bombx,
                    bomby,
                    numdest);

            for (i = 1; i <= Num_races; ++i) {
                if ((p->info[i - 1].numsectsowned) && (i != ship->owner)) {
                    push_telegram(i,
                                  Stars[ship->storbits]->governor[i - 1],
                                  buf);
                }
            }

            if (numdest) {
                sprintf(buf,
                        "%s dropped on %s.\n",
                        Ship(ship),
                        prin_ship_orbits(ship));

#ifndef LIMITED_COMBAT_MESSAGES
                post(buf, COMBAT);

#else

                warn_star(0, 0, (int)ship->storbits, buf, COMBAT);
#endif
            }

            kill_ship((int)ship->owner, ship);
        }
    } else if (ship->whatdest == LEVEL_SHIP) {
        sh2 = ship->destshipno;
        dist = sqrt(Distsq(ship->xpos, ship->ypos, ships[sh2]->xpos, ships[sh2]->ypos));

        if (dist <= (((double)ship->speed * STRIKE_DISTANCS_FACTOR * (100.0 - (double)ship->damage)) / 100.0)) {
#ifdef USE_VN
            /* CWL Allow berserkers to defend against missiles */
            if (ships[sh2]->type == OTYPE_BERS) {
                shoot_ship_to_ship(ships[sh2],
                                   ship,
                                   10,
                                   0,
                                   0,
                                   long_buf,
                                   short_buf);

                push_telegram((int)ship->owner,
                              (int)ship->governor,
                              long_buf);

                push_telegram((int)ships[sh2]->owner,
                              (int)ships[sh2]->governor,
                              long_buf);

#ifndef LIMITED_COMBAT_MESSAGES
                post(short_buf, COMBAT);

#else

                warn_star(ship->owner,
                          ships[sh2]->owner,
                          (int)ship->storbits,
                          short_buf,
                          COMBAT);
#endif

                use_destruct(ships[sh2], 10);
            } /* End CWL */
#endif

            /* CWL mod frigates and fleets with frigates shoot */
            if (ship->alive
                && ((ships[sh2]->type == STYPE_FRIGATE)
                    || ships[sh2]->fleetmember)) {
                /* Ship is a frigate, or a fleet */
                int fship; /* Ship in the fleet */

                if (ships[sh2]->fleetmember) {
                    fship = races[ships[sh2]->owner]->fleet[ships[sh2]->fleetmember].flagship;
                } else {
                    fship = sh2;
                }

                while (fship && ship->alive) {
                    /* While we can still shoot */
                    int chance2see;
                    int guns2fire;

                    if ((ships[fship]->type == STYPE_FRIGATE)
                        && ships[fship]->alive) {
                        chance2see = (int)(2 * sqrt(ships[fship]->tech)) + 5;

                        /*
                         * Hmm...might have to consider range in the above...but
                         * let's test this one first -mfw
                         */

                        if (int_rand(1, 100) <= chance2see) {
                            if (ships[fship]->guns == PRIMARY) {
                                guns2fire = ships[fship]->primary;
                            } else if (ships[fship]->guns == SECONDARY) {
                                guns2fire = ships[fship]->secondary;
                            } else {
                                guns2fire = 0;
                            }

                            shoot_ship_to_ship(ships[fship],
                                               ship,
                                               guns2fire,
                                               0,
                                               0,
                                               long_buf,
                                               short_buf);

                            push_telegram((int)ships[fship]->owner,
                                          (int)ships[fship]->governor,
                                          "Frigate defense net activated!\n");

                            push_telegram((int)ships[fship]->owner,
                                          (int)ships[fship]->governor,
                                          long_buf);

                            push_telegram((int)ships->owner,
                                          (int)ships->governor,
                                          long_buf);

#ifndef LIMITED_COMBAT_MESSAGE
                            post(short_buf, COMBAT);

#else
                            warn_star(ship->owner,
                                      ships[fship]->owner,
                                      (int)ship->storbits,
                                      short_buf,
                                      COMBAT);
#endif

                            use_destruct(ships[fship], guns2fire);
                        } /* If seen */
                    } /* If frigate */

                    fship = ships[fship]->nextinfleet;
                } /* While fship and missile active */
            } /* If we might have a frigate here */
            /* End CWL */

            if (ship->alive) { /* CWL mod */
                /* Do the attack */
                shoot_ship_to_ship(ship,
                                   ships[sh2],
                                   (int)ship->destruct,
                                   0,
                                   0,
                                   long_buf,
                                   short_buf);

                push_telegram((int)ship->owner,
                              (int)ship->governor,
                              long_buf);

                push_telegram((int)ships[sh2]->owner,
                              (int)ships[sh2]->governor,
                              long_buf);

                kill_ship((int)ship->owner, ship);

#ifndef LIMITED_COMBAT_MESSAGE
                post(short_buf, COMBAT);

#else

                warn_star(ship->owner,
                          ships[sh2]->owner,
                          (int)ship->storbits,
                          short_buf,
                          COMBAT);
#endif
            }
        }
    }
}

void do_mine(int shipno, int manual_detonate)
{
    int sh;
    int sh2;
    shiptype *s;
    shiptype *ship;
    planettype *planet;
    racetype *r;

    /*
     * HUTm (kse) If mine is not detonated it is then called from doturn in that
     * case all ship data is in ships table and all data written using putships
     * will be overwritten later by doturn. Anyhow do_mine is also called from
     * fire.c when mine is detonated manually and then getship-putship should be
     * used
     */

    if (manual_detonate) {
        getship(&ship, shipno);
    } else {
        ship = ships[shipno];
    }

    if (!ship->owner || (ship->type != STYPE_MINEF) || !ship->alive) {
        if (manual_detonate) {
            free(ship);
        }

        return;
    }

    if (get_num_updates() < CombatUpdate) {
        return;
    }

    if ((ship->destruct == 0) && ship->special.trigger.disperse) {
        sprintf(buf,
                "Minefield %s dispersed at %s. [destruct stockpile empty]\n",
                Ship(ship),
                prin_ship_orbits(ship));

        notify((int)ship->owner, (int)ship->governor, buf);
        kill_ship((int)ship->owner, ship);
    } else {
        /* Check around and see if we should explode. */
        if (ship->on || manual_detonate) {
            int in_radius = 0;
            double xd;
            double yd;
            double range;

            switch (ship->whatorbits) {
            case LEVEL_STAR:
                sh = Stars[ship->storbits]->ships;

                break;
            case LEVEL_PLAN:
                getplanet(&planet, (int)ship->storbits, (int)ship->pnumorbits);
                sh = planet->ships;
                free(planet);

                break;
            default:
                if (manual_detonate) {
                    free(ship);
                }

                return;
            }

            sh2 = sh;

            /*
             * Traverse the list, look for ships that are closer than the
             * trigger radius...
             */
            in_radius = 0;

            if (!manual_detonate) {
                r = races[ship->owner - 1];

                while (sh && !in_radius) {
                    if (!valid_ship(sh)) {
                        sh = 0;

                        continue;
                    }

                    s = ships[sh];
                    xd = s->xpos - ship->xpos;
                    yd = s->ypos - ship->ypos;
                    range = sqrt((xd * xd) + (yd * yd));

                    if (!isset(r->allied, s->owner)
                        && (s->owner != ship->owner)
                        && ((int)range <= ship->special.trigger.radius)) {
                        in_radius = 1;
                    } else {
                        sh = nextship(s);
                    }
                }
            } else {
                in_radius = 1;
            }

            if (ship-alive) {
                if (in_radius) {
                    sprintf(buf,
                            "Minefield %s detonated at %s\n",
                            Ship(ship),
                            prin_ship_orbits(ship));

#ifndef LIMITED_COMBAT_MESSAGES
                    post(buf, COMBAT);

#else

                    warn_star(s->owner,
                              ship->owner,
                              (int)s->storbits,
                              buf,
                              COMBAT);
#endif

                    warn((int)ship->owner, (int)ship->governor, buf);
                    notify_star((int)ship->owner,
                                (int)ship->governor,
                                0,
                                (int)ship->storbits,
                                buf);

                    /* Kill off the ships nearby */
                    sh = sh2;

                    while (sh) {
                        if (!valid_ship(sh)) {
                            sh = 0;

                            continue;
                        }

                        if (manual_detonate) {
                            getship(&s, sh);
                        } else {
                            s = ships[sh];
                        }

                        if ((ship != shipno)
                            && s->alive
                            && (s->type != OTYPE_CANIST)
                            && (s->type != OTYPE_GREEN)
                            && (s->owner != ship->owner)) {
                            while ((ship->destruct != 0) && s->alive) {
                                use_destruct(ship, 1);

                                if (s->type) {
                                    /* Pods have a 1 in 3 chance of hit */
                                    if (int_rand(1, 3) == 1) {
                                        in_radius = shoot_ship_to_ship(ship,
                                                                       s,
                                                                       1,
                                                                       0,
                                                                       0,
                                                                       long_buf,
                                                                       short_buf);
                                    }
                                } else {
                                    int_radius = shoot_ship_to_ship(ship,
                                                                    s,
                                                                    1,
                                                                    0,
                                                                    0,
                                                                    long_buf,
                                                                    short_buf);
                                }

                                if (in_radius > 0) {
#ifndef LIMITED_COMBAT_MESSAGES
                                    if (!s->alive) {
                                        post(short_buf, COMBAT);
                                    }

#else

                                    warn_star(ship->owner,
                                              s->owner,
                                              (int)ship->storbits,
                                              short_buf,
                                              COMBAT);
#endif

                                    warn((int)s->owner,
                                         (int)s->governor,
                                         long_buf);

                                    warn((int)ship->owner,
                                         (int)ship->governor,
                                         long_buf);

                                    push_telegram((int)s->owner,
                                                  (int)s->governor,
                                                  long_buf);

                                    push_telegram((int)ship->owner,
                                                  (int)ship->governor,
                                                  long_buf);
                                }
                            }

                            if (manual_detonate) {
                                putship(s);
                            }
                        }

                        sh = nextship(s);

                        if (manual_detonate) {
                            free(s);
                        }
                    } /* While sh */
                } /* If in_radius */
            } /* If alive */

            if (manual_detonate) {
                putship(ship);
            }
        }  /* if on || manual_detonate */
    }

    if (manual_detonate) {
        free(ship);
    }
}

void do_sweeper(int shipno)
{
    int sh;
    int sh2;
    shiptype *s;
    shiptype *sweeper;
    planettype *planet;
    racetype *r;
    int rad = 0;
    int chance_to_see;
    int roll;
    int amount_to_use;
    int xd;
    int yd;
    int range;

    if (get_num_updates() < CombatUpdate) {
        return;
    }

    /* Called only from doturn() so all ships are in ships[] */
    void getship(&sweeper, shipno);
    /* Handled by getship above instead -mfw */
    /* sweeper = ships[shipno]; */

    int sweeper = 0;

#ifdef USE_VN
    /* Berserkers are sweepers too */
    if (sweeper->type != OTYPE_BERS) {
        sweeper = 1;
    }
#endif

    if (((sweeper->type == STYPE_SWEEPER) || sweeper)
        && sweeper->alive
        && sweeper->owner) {
        switch(sweeper->whatorbits) {
        case LEVEL_STAR:
            sh = Stars[sweeper->storbits]->ships;

            break;
        case LEVEL_PLAN:
            getplanet(&planet,
                      (int)sweeper->storbits,
                      (int)sweeper->pnumorbits);

            sh = planet->ships;
            free(planet);

            break;
        default:
            free(sweeper);

            break;
        }

        sh2 = sh;

        while (sh && !rad) {
            if (!valid_ship(sh)) {
                sh = 0;

                continue;
            }

            r = races[sweeper->owner - 1];
            getship(&s, sh);
            /* Handled by getship() above -mfw */
            /* s = ships[sh]; */
            xd = s->xpos = sweeper->xpos;
            range = sqrt((xd * xd) + (yd * yd));

            /*
             * New code by Kharush. More power to sweepers. Another change in
             * shootblast.c, shoot_ship_to_ship function.
             */
            if (!isset(r->allied, s->owner)
                && (s->owner != sweeper->owner)
                && ((int)range <= 1200)) {
                /*
                 * Old code
                 *
                 * if (!isset(r->allied, s->owner)
                 *     && (s->owner != sweeper->owner)
                 *     && ((int)range <= 600)) {
                 */
                rad = 1;
            } else {
                sh = nextship(s);
                free(s);
            }
        }

        if (rad) {
            int res;

            sh = sh2;

            while (sh) {
                if (!valid_ship(sh)) {
                    sh = 0;

                    continue;
                }

                getship(&s, sh);
                /* Handled by getship() above -mfw */
                if ((sh != shipno) && s->alive && (s->type == STYPE_MINEF)) {
                    chance_to_see = (int)(5 + (2 * sqrt(sweeper->tech)));
                    amount_to_use = 2;
                    roll = int_rand(1, 36); /* When you reach tech 200, 36 */
                    sprintf(buf,
                            "Minesweeper detect chance %d/%d on mine %d\n",
                            chance_to_see,
                            roll,
                            s->number);

                    warn((int)sweeper->owner,
                         (int)sweeper->governor,
                         buf);

                    if (roll <= chance_to_see) {
                        while ((sweeper->destruct != 0) && s->alive) {
                            res = shoot_ship_to_ship(sweeper,
                                                     s,
                                                     amount_to_use,
                                                     0,
                                                     0,
                                                     long_buf,
                                                     short_buf);

                            if (rez > 0) {
                                warn_star(s->owner,
                                          so->owner,
                                          (int)sweeper->storbits,
                                          short_buf);

                                warn((int)sweeper->owner,
                                     sweeper->governr,
                                     long_buf);

                                warn(s->owner, s->governor, long_buf);
                                use_destruct(sweeper, amount_to_use);
                                putship(sweeper);
                            }
                        }
                    }

                    putship(s);
                }

                sh = nextship(s);
                free(s);
            }
        } /* rad */
    }

    free(sweeper);
}

void doabm(shiptype *ship)
{
    int sh2;
    int numdest;
    int caliber;
    planettype *p;

    if (get_num_updates() < CombatUpdate) {
        return;
    }

    if (!ship->alive || !ship->owner) {
        return;
    }

    if (!ship->on || !ship->retaliate || !ship->destruct) {
        return;
    }

    if (landed(ship)) {
        p = planets[ship->storbits][ship->pnumorbits];
        caliber = current_caliber(ship);
        /* Check to see if missiles/mines are present */
        sh2 = p->ships;

        while (sh2 && ship->destruct) {
            if (!valid_ship(sh2)) {
                sh2 = 0;

                continue;
            }

            if (ships[sh2]->alive
                && ((ships[sh2]->type == STYPE_MISSILE)
                    || (ships[sh2]->type == STYPE_MINEF))
                && (ships[sh2]->owner != ship->owner)
                && !(isset(races[ship->owner - 1]->allied, ships[sh2]->owner)
                     && isset(races[ship->owner - 1]->allied, ship->owner))) {
                /*
                 * Added last two tests to prevent mutually allied missiles
                 * getting shot up
                 */
                /* Attack the missile/mine */
                numdest = retal_strength(ship);
                numdest = MIN(numdest, ship->destruct);
                numdest = MIN(numdest, ship->retaliate);
                ship->destruct -= numdest;
                shoot_ship_to_ship(ship,
                                   ships[sh2],
                                   numdest,
                                   0,
                                   0,
                                   long_buf,
                                   short_buf);

                push_telegram((int)ship->owner,
                              (int)ship->governr,
                              long_buf);

                push_telegram((int)ships[sh2]->owner,
                              (int)ships[sh2]->governor,
                              long_buf);

#ifndef LIMITED_COMBAT_MESSAGES
                post(short_buf, COMBAT);

#else

                warn_star(ship->owner,
                          ships[sh2]->owner,
                          (int)ship->storbits,
                          short_buf,
                          COMBAT);
#endif
            }

            sh2 = nextship(ships[sh2]);
        }
    }
}

void do_repair(shiptype *ship)
{
    int drep;
    int cost = 0;
    int plan_res;
    double maxrep;
    double rate = -1.0;
    sectortype *s;
    planettype *planet;

    maxrep = REPAIR_RATE / (double)segments;

    /* Stations repair for free, and ships docked with them */
    if (Shipdata[ship->type][ABIL_REPAIR]) {
        cost = 0;
    } else {
        if (ship->docked
            && (ship->whatdest == LEVEL_SHIP)
            && (ships[ship->destshipno]->type == STYPE_STATION)) {
            cost = 0;
        } else {
            if (ship->docked
                && (ship->whatorbits == LEVEL_SHIP)
                && (ships[ship->destshipno]->type == STYPE_STATION)) {
                cost = 0;
            } else {
                /*
                 * HUTm (kse) If ship is landed own civilians of that sector
                 * will help in repairing the ship
                 */
                if ((ship->whatorbits == LEVEL_PLAN) && landed(ship)) {
                    if (getsector(&s, planet[ship->storbits][ship->pnumorbits], (int)ship->land_x, (int)ship->land_y)) {
                        if (ship->max_crew < 1) { /* For VNs -mfw */
                            rate = 1;
                        } else {
                            if (s->owner == ship->owner) {
                                rate = (double)(ship->popn + s->popn) / (double)ship->max_crew;
                            } else {
                                rate = (double)ship->popn / (double)ship->max_crew;
                            }
                        }

                        if (rate <= 1) {
                            maxrep *= rate;
                        }

                        if (ship->wants_reports) {
                            sprintf(telegram_buf,
                                    "Ship %s owned by %d landed at %s sector (%d, %d): owned by %d, civilians %d (rate = %g, cost = %d",
                                    Ship(ship),
                                    ship->owner,
                                    prin_ship_orbits(ship),
                                    ship->land_x,
                                    ship->land_y,
                                    s->owner,
                                    s->popn,
                                    rate,
                                    cost);

                            push_telegram(ship->owner,
                                          (int)Stars[ship->storbits]->governor[ship->owner - 1],
                                          telegram_buf);
                        }

                        free(s);
                    } else {
                        maxrep = 0;
                    }
                } else {
                    if (ship->max_crew < 1) {
                        maxrep = 1;
                    } else {
                        maxrep *= ((double)ship->popn / (double)ship->max_crew);
                    }
                }

                /* -mfw */
                if (s->type == OTYPE_FACTORY) {
                    cost = (int)(0.005
                                 * maxrep
                                 * ((2
                                     * ship->build_cost
                                     * ship->on)
                                    + Shipdata[ship->type][ABIL_COST]));

                    if (ship->whatorbits == LEVEL_UNIV) {
                        cost += 1;
                    }
                } else {
                    cost = (int)(0.005 * maxrep * ship->build_cost);

                    if (ship->whatorbits == LEVEL_UNIV) {
                        cost += 1;
                    }
                }
            }
        }
    }

    /*
     * if (cost <= ship->resource) {
     *     use_resource(ship, cost);
     *     drep = (int)maxrep;
     *     ship->damage = MAX(0, (int)ship->damage - drep);
     * } else {
     *     drep = (int)(maxrep * ((double)ship->resource / (int)cost));
     *     use_resource(ship, ship->resource;
     *     ship->damage = MAX(0, (int)ship->damage - drep);
     * }
     */

    /* HUTm (kse) Landed ships use also planetary stockpile for repairs */
    if (ship->use_stock && (ship->whatorbits == LEVEL_PLAN) && landed(ship)) {
        getplanet(&planet, ship->storbits, ship->pnumorbits);
        plan_res = planet->info[ship->owner - 1].resource;

        if (plan_res >= cost) {
            planet->info[ship->owner - 1].resource = plan_res - cost;
        } else {
            planet->info[ship->owner - 1].resource = 0;
        }

        putplanet(planet, ship->storbits, ship->pnumorbits);
        free(planet);

        /*
         * if (cost) {
         *     sprintf(telegram_buf,
         *             "Ship %s used %d resources from planetary stockpile at %s (rate = %g, cost = %d)",
         *             Ship(ship),
         *             (plan_res >= cost)? cost : plan_res,
         *             prin_ship_orbits(ship),
         *             rate,
         *             cost);
         *
         *     push_telegram(ship->owner,
         *                   Stars[ship->storbits]->governor[ship->owner - 1],
         *                   telegram_buf);
         */
        if (plan_res >= cost) {
            cost = 0;
        } else {
            cost -= plan_res;
        }
    }

    if (cost <= ship->resource) {
        use_resource(ship, cost);
        drep = (int)maxrep;
    } else {
        /* Use up all of the ship's resources */
        drep = (int)(maxrep * ((double)ship->resource / (int)cost));
        use_resource(ship, ship->resource);
    }

    ship->damage = MAX(0, (int)ship->damage, drep);
}

void do_habitat(shiptype *ship)
{
    int sh;
    int add;
    int rate;
    int max_crew;
    double fuse;
    unsigned int wruse;
    unsigned int wfuse;

    /* In v5.0+ habitats make resources out of fuel */
    if (ship->on) {
        if (ship->whatorbits == LEVEL_PLAN) {
            if (planets[ship->storbits][ship->pnumorbits]->type == TYPE_GAS_GIANT) {
                fuse = (double)ship->fuel * ((double)ship->popn / (double)ship->max_crew) * (1.0 - (0.01 * (double)ship->damage));

                add = (int)fuse / 10;
                /* Was this in JH -mfw */
                /* add = (int)fuse / 50; */

                if ((ship->resource + add) > ship->max_resource) {
                    add = ship->max_resource - ship->resource;
                }

                /* HUT Gardan factor of 10 was before 20 */
                fuse = 10.0 * (double)add;
                rcv_resource(ship, add);
                use_fuel(ship, fuse);
            }
        }

        sh = ship->ships;

        while (sh) {
            if (!valid_ship(sh)) {
                sh = 0;

                continue;
            }

            if (ships[sh]->type == OTYPE_WPLANT) {
                /* HUTm (kse) rcv_destruct(ship, do_weapon_plant(ships[sh])); */
                wruse = ship->resource;
                wfuse = ship->fuel;
                rate = do_weapon_plant(ships[sh], &wruse, &wfuse);
                rcv_destruct(ship, rate);

                if (wruse) {
                    use_resource(ship, wruse);
                }

                if (wfuse) {
                    use_fuel(ship, wfuse);
                }
            }

            sh = nextship(ships[sh]);
        }
    }

    add = round_rand((double)ship->popn * races[ship->owner - 1]->birthrate);

    if (ship->type == OTYPE_FACTORY) {
        max_crew = Shipdata[ship->type][ABIL_MAX_CREW] - ship->troops;
    } else {
        max_crew = ship->max_crew - ship->troops;
    }

    if ((ship->popn + add) > max_crew) {
        add = max_crew - ship->popn;
    }

    rcv_popn(ship, add, races[ship->owner - 1]->mass);
}

void do_canister(shiptype *ship)
{
    if ((ship->whatorbits == LEVEL_PLAN) && !landed(ship)) {
        ++ship->special.timer.count;

        if (ship->special.timer.count < DISSIPATE) {
            if (Stinfo[ship->storbits][ship->pnumorbits].temp_add < -90) {
                Stinfo[ship->storbits][ship->pnumorbits].temp_add = -100;
            } else {
                Stinfo[ship->storbits][ship->pnumorbits].temp_add -= 10;
            }
        } else {
            /* Timer expired; destroy canister */
            int j = 0;

            kill_ship((int)ship->owner, ship);

            sprintf(telegram_buf,
                    "Canister of dust previously covering %s has dissipated.\n",
                    prin_ship_orbits(ship));

            for (j = 1; j <= Num_races; ++j) {
                if (planets[ship->storbits][ship->pnumorbits]->info[j - 1].numsectsowned) {
                    push_telegram(j,
                                  (int)Stars[ship->storbits]->governor[j - 1],
                                  telegram_buf);
                }
            }
        }
    }
}

void do_greenhouse(shipttype *ship)
{
    if ((ship->whatorbits == LEVEL_PLAN) && !landed(ship)) {
        ++ship->special.timer.count;

        if (ship->special.timer.count < DISSIPATE) {
            if (Stinfo[ship->storbits][ship->pnumorbits].temp_add > 90) {
                Stinfo[ship->storbits][ship->pnumorbits].temp_add = 100;
            } else {
                Stinfo[ship->storbits][ship->pnumorbits].temp_add += 10;
            }
        } else {
            /* Timer expired; destroy canister */
            int j = 0;

            kill_ship((int)ship->owner, ship);

            sprintf(telegram_buf,
                    "Greenhouse gases at %s have dissipated.\n",
                    print_ship_orbits(ship));

            for (j = 1; j <= Num_races; ++j) {
                if (planet[ship->storbits][ship->pnumorbits]->info[j - 1].numsectsowned) {
                    push_telegram(j,
                                  (int)Stars[ship->storbits]->governor[j - 1],
                                  telegram_buf);
                }
            }
        }
    }
}

void do_mirror(shiptype *ship)
{
    int i;
    double range;

    switch (ship->special.aimed_at.level) {
    case LEVEL_SHIP:
        /* Ship aimed at is a legal ship now if in the same system */
        if (((ship->whatorbits == LEVEL_STAR)
             || (ship->whatorbits == LEVEL_PLAN))
            && (ships[ship->special.aimed_at.shipno] != NULL)
            && ((ships[ship->special.aimed_at.shipno]->whatorbits == LEVEL_STAR)
                || (ships[ship->special.aimed_at.shipno]->whatorbits == LEVEL_PLAN))
            && (ship->storbits == ships[ship->special.aimed_at.shipno]->storbits)
            && ships[ship->special.aimed_at.shipno]->alive) {
            shiptype *s;

            s = ships[ship->special.aimed_at.shipno];
            range = sqrt(Distsq(ship->xpos, ship->ypos, s->xpos, x->ypos));
            i = int_rand(0,
                         round_rand(2.0
                                    / (((double)(s->size - s->max_hanger)
                                        * (double)ship->special.aimed_at.intensity)
                                       / ((range / PLORBITSIZE) + 1.0))));

            sprintf(telegram_buf, "%s aimed at %s\n", Ship(ship), Ship(s));
            s->damage += i;

            if (i) {
                sprintf(buf, "%d%% damage done.\n", i);
                strncat(telegram_buf,
                        buf,
                        AUTO_TELEG_SIZE - strlen(telegram_buf));
            }

            if (s->damage >= 100) {
                sprintf(buf, "%s DESTROYED!!!\n", Ship(s));
                strncat(telegram_buf,
                        buf,
                        AUTO_TELEG_SIZE - strlen(telegram_buf));

                kill_ship((int)ship->owner, s);
            }

            push_telegram((int)s->owner,
                          (int)s->governor,
                          telegram_buf);

            push_telegram((int)ship->owner,
                          (int)ship->governor,
                          telegram_buf);
        }

        break;
    case LEVEL_PLAN:
        range = sqrt(Distsq(ship->xpos,
                            ship->ypos,
                            Stars[ship->storbits]->xpos + planets[ship->storbits][ship->pnumorbits]->xpos,
                            Stars[ship->storbits]->ypos + planets[ship->storbits][ship->pnumorbits]->ypos));

        if (range > PLORBITSIZE) {
            i = (PLORBITSIZE * ship->special.aimed_at.intensity) / range;
        } else {
            i = ship->special.aimed_at.intensity;
        }

        i = round_rand(0.01 * (100.0 - (double)ship->damage) * (double)i);
        Stinfo[ship->storbits][ship->special.aimed_at.pnum].temp_add += i;

        break;
    case LEVEL_STAR:
        /*
         * Have to be in the same system as the star; otherwise it's not too
         * fair...
         */
        if ((ship->special.aimed_at.snum > 0)
            && (ship->special.aimed_at.snum < Sdata.numstars)
            && (ship->whatorbits > LEVEL_UNIV)
            && (ship->special.aimed_at.snum == ship->storbits)) {
            if (gb_rand() % 2 == 1) {
                Stars[ship->special.aimed_at.snum]->stability += 1;
            }
        }

        break;
    case LEVEL_UNIV:

        break;
    }
}

void do_god(shiptype *ship)
{
    /* Gods have infinite power...heh heh heh */

    /*
     * Hmmm...I think that if I worked at it, I could, given some time, capture
     * a GODSHIP. A bit less than 100 shuttles to drain off the destruct with
     * assaults, then mop up the crew. Maarten
     */
    if (races[ship->owner - 1]->God) {
        if (ship->type == OTYPE_FACTORY) {
            ship->fuel = Shipdata[ship->type][ABIL_FUELCAP];
            ship->destruct = Shipdata[ship->type][DESTCAP];
            ship->resource = Shipdata[ship->type][ABIL_CARGO];
        } else {
            ship->fuel = ship->max_fuel;
            ship->destruct = ship->max_destruct;
            ship->resource = ship->max_resource;
        }
    }
}

int do_ap(shiptype *ship)
{
    racetype *race;
    plaettype *planet;
    char rbuf[((INF_MAX_ATMO_SETTING * 4) + 4) * 80];
    char itemp[256];
    char gas[80];

    sprintf(buf, "do_ap(): in function.\n");
    debug(LEVEL_GENERAL, buf);

    /* If landed on planet, change conditions to be like race */
    if (landed(ship)) {
        /* Removed so damaged ships work too /HUT Gardan 23.1.97 */
        /* && ship->on) */
        int j;
        int change;
        int size;
        int sizefac;
        double difference;

        planet = planets[ship->storbits][ship->pnumorbits];
        race = races[ship->owner - 1];

        /*
         * HUTm (kse) Atmospheric process uses also planetary stockpile
         *
         * if (ship->fuel >= 3.0) {
         *     use_fuel(ship, 3.0);
         * }
         */
        if (((ship->use_stock * planet->info[ship->owner - 1].fuel) + ship->fuel) >= FUEL_COST_AP) {
            if (ship->use_stock && (ship->fuel < FUEL_COST_AP)) {
                if (planet->info[ship->owner - 1].fuel < (FUEL_COST_AP - ship->fuel)) {
                    planet->info[ship->owner - 1].fuel = 0;
                } else {
                    planet->info[ship->owner - 1].fuel -= (FUEL_COST_AP - ship->fuel);
                }

                use_fuel(ship, ship->fuel);
            } else {
                use_fuel(ship, FUEL_COST_AP);
            }

            size = planet->Maxx * planet->Maxy;

            if (size <= 32) {
                sizefac = 1;
            } else if (size <= 200) {
                sizefac = 2;
            } else {
                sizefac = 4;
            }

            sprintf(rbuf, "Atmospheric Process #%d Report:\n", ship->number);

            for (j = METHANE; j <= OTHER; ++j) {
                difference = race->conditions[j] - planet->conditions[j];

                if (difference) {
                    /* Ship damage factor added /HUT Gardan 23.1.97 */
                    change = round_rand((ap_planet_factor(planet) * crew_factor(ship) * ((100 - ship->damage) / 100) * (double)((1 / difference) * 100)) / sizefac);

                    if (change) {
                        if (change < 0) {
                            change = MAX(difference, change);
                        } else {
                            change = MIN(difference, change);
                        }
                    } else {
                        /*
                         * Above formula doesn't produce a change for high
                         * values of 'difference', we need to at least change by
                         * 1% per update -mfw
                         */
                        if (difference < 0) {
                            change = -1;
                        } else {
                            change = 1;
                        }
                    }

                    gastype(j, gas);
                    sprintf(buf,
                            "do_ap(): ship #%d changing %s by %d%%.\n",
                            ship->number,
                            gas,
                            change);

                    debug(LEVEL_GENERAL, buf);

                    planet->conditions[j] += change;
                } else {
                    change = 0;
                }

                /* Sanity check */
                if (planet->conditions[j] < 0) {
                    planet->conditions[j] = 0;
                }

                gastype(j, gas);

                if (change) {
                    sprintf(itemp,
                            "%15s|-> %s changed %d%% to %d%%\n",
                            " ",
                            gas,
                            change,
                            planet->conditions[j]);

                    strcat(rbuf, itemp);
                }
            }

            if (ship->inf.wants_reports) {
                push_telegram((int)ship->owner,
                              (int)ship->governor,
                              rbuf);
            }

            /*
             * HUTm (Gardan) 23.1.97 Calculate amount of 7 gases and set other
             * so that total is 100.
             *
             * Fixed to not freak out -mfw
             */
            if (race->conditions[OTHER]) {
                change = 0;

                for (j = METHANE; j < OTHER; ++j) {
                    change += planet->conditions[j];
                }

                if (change) {
                    planet->conditions[OTHER] = 100 - change;

                    if (planet->conditions[OTHER] < 0) {
                        planet->conditions[OTHER] = 0;
                    }
                } else {
                    /* No atmosphere */
                    planet->conditions[OTHER] = 0;
                }
            }

            return 1;
        } else {
            if (!ship->notified) {
                ship->notified = 1;
                ship->on = 0;
                msg_OOF(ship);
            }

            return 0;
        }
    }

    return 0;
}

void gastype(int num, char *gas)
{
    switch (num) {
    case METHANE:
        strcpy(gas, "Methane");

        break;
    case OXYGEN:
        strcpy(gas, "Oxygen");

        break;
    case CO2:
        strcpy(gas, "CO2");

        break;
    case HYDROGEN:
        strcpy(gas, "Hydrogen");

        break;
    case NITROGEN:
        strcpy(gas, "Nitrogen");

        break;
    case SULFUR:
        strcpy(gas, "Sulfur");

        break;
    case HELIUM:
        strcpy(gas, "Helium");

        break;
    case OTHER:
        strcpy(gas, "Other");

        break;
    default:
        strcpy(gas, "<unknown>");

        break;
    }
}

double crew_factor(shiptype *ship)
{
    int maxcrew;
    maxcrew = Shipdata[ship->type][ABIL_MAXCREW];

    if (!maxcrew) {
        return 0.0;
    }

    return ((double)ship->popn / (double)maxcrew);
}

double ap_planet_factor(planettype *p)
{
    double x;

    x = (double)p->Maxx * (double)p->Maxy;

    return (AP_FACTOR / (AP_FACTOR + x));
}

void do_oap(shiptype *ship)
{
    /* "Intimidate" the planet below, for enslavement purposes. */
    if (ship->whatorbits == LEVEL_PLAN) {
        Stinfo[ship->storbits][ship->pnumorbits].intimidated = 1;
    }
}

void do_omcl(shiptype *ship)
{
    /* Orbital mind control laser */
    if ((ship->special.aimed_at.level == LEVEL_PLAN)
        && ship->on
        && (ship->speed == 1)) {
        planets[ship->special.aimed_at.snum][ship->special.aimed_at.pnum]->sheep = 1;
    }
}

int do_weapon_planet(shiptype *ship, unsigned *reso, unsigned *fuel)
{
    int maxrate;
    int rate;

    maxrate = (int)(races[ship->owner - 1]->tech / 2.0);

    /*
     * HUTm (kse)
     *
     * rate = round_rand(MIN((double)ship->resource / (double)RES_COST_WPLANT, ship->fuel / FUEL_COST_WPLANT);
     */
    rate = round_rand((MIN((((double)*res * ship->use_stock) + ship->resource) / (double)RES_COST_WPLANT,
                           (((double)*fuel * ship->use_stock) + ship->fuel) / FUEL_COST_WPLANT)
                       * (1.0 - (0.01 * (double)ship->damage))
                       * (double)ship->popn) / (double)ship->max_crew);

    rate = MIN(race, maxrate);

    /*
     * HUTm (kse)
     *
     * use_resource(ship, (rate * RES_COST_WPLANT));
     * use_fuel(ship, (double)rate * FUEL_COST_WPLANT);
     */
    *reso = 0;

    if (ship->use_stock && (ship->resource < (rate * RES_COST_WPLANT))) {
        *reso = (rate * RES_COST_WPLANT) - ship->resource;
        use_resource(ship, ship->resource);
    } else {
        use_resource(ship, rate * RES_COST_WPLANT);
    }

    *fuel = 0;

    if (ship->use_stock && (ship->fuel < (rate * RES_COST_WPLANT))) {
        *fuel = (rate * FUEL_COST_WPLANT) - ship->fuel;
        use_fuel(ship, ship->fuel);
    } else {
        use_fuel(ship, rate * FUEL_COST_WPLANT);
    }

    return rate;
}

/* From HAP - mfw */
void do_seti(shiptype *ship)
{
    int cando;

    /* Report on ships now owned by us in the vicinity */
    if (ship->on && ship->alive) {
        cando = 1;
    } else {
        cando = 0;
    }

    if (ship->on && ship->alive) {
        if (SETI_FUEL_CONSUMPTION) {
            if (!ship->fuel) {
                cando = 0;
            }
        }

        if (SETI_RESOURCE_CONSUMPTION) {
            if (!ship->resource) {
                cando = 0;
            }
        }

        if (!cando) {
            sprintf(telegram_buf,
                    "%s: Science Station #%d has shutdown.\n",
                    prin_ship_orbits(ship),
                    ship->number);

            push_telegram((int)ship->owner,
                          (int)ship->governor,
                          telegram_buf);
        } else {
            use_fuel(ship, SETI_FUEL_CONSUMPTION);
            use_resource(ship, SETI_RESOURCE_CONSUMPTION);
        }
    }
}

int kill_ship(int playernum, shiptype *ship)
{
    planettype *planet;
    racetype *killer;
    racetype *victim;
    shiptype *s;
    int sh;

    ship->alive = 0;
    ship->notified = 0; /* Prepare the ship for recycling */

    /* Remove the ship from it's fleet (if any) -mfw */
    remove_sh_fleet(playernum, 0, ship);

    if (!ship->type && (ship->type != OTYPE_FACTORY)) {
        /* Pods don't do things to morale, ditto for factories */
        victim = races[ship->owner - 1];

        if (victim->Gov_ship == ship->number) {
            victim->Gov_ship = 0;
        }

        if (!victim->God && (playernum != ship->owner)) {
            killer = races[playernum - 1];
            adjust_morale(killer, victim, (int)ship->build_cost);
            putrace(killer);
        } else {
            /*
             * If the ship has crew that can't be recovered then morale suffers
             */
            if ((ship->owner == playernum)
                && !ship->docked
                && (ship->popn || ship->troops)) {
                victim->morale -= (2 * ship->build_cost); /* Shuttle/scrap */
            }
        }

        putrace(victim);
    }

#ifdef USE_VN
    if ((ship->type == OTYPE_VN) || (ship->type == OTYPE_BERS)) {
        ship->special.mind.who_killed = playernum;
        getsdata(&Sdata);

        /* Add killer race to VN shit list */
        Sdata.VN_hitlist[playernum - 1] += 1;

        /* Keep track of where these VN's were shot up */
        if (Sdata.VN_index1[playernum - 1] == -1) {
            /* There's no star in the first index store location here */
            Sdata.VN_index1[playernum - 1] = ship->storbits;
        } else if (Sdata.VN_index2[playernum - 1] == -1) {
            /* There's no star in the second index store location here */
            Sdata.VN_index2[playernum - 1] = ship->storbits;
        } else {
            /* Location indexes are full randomly pick an index to supplant */
            if (random() % 2 == 1) {
                Sdata.VN_index1[playernum - 1] = ship->storbits;
            } else {
                Sdata.VN_index2[playernum - 1] = ship->storbits;
            }
        }

        putsdata(&Sdata);
    }
#endif

    if ((ship->type == OTYPE_TOXWC) && (ship->whatorbits == LEVEL_PLAN)) {
        getplanet(&planet, (int)ship->storbits, (int)ship->pnumorbits);
        planet->conditions[TOXIC] = MIN(100, planet->conditions[TOXIC] + ship->special.waste.toxic);
        putplanet(planet, (int)ship->storbits, (int)ship->pnumorbits);
        free(planet);
    }

    /* Undock the stuff docked with it */
    if (ship->docked
        && (ship->whatorbits != LEVEL_SHIP)
        && (ship->whatdest == LEVEL_SHIP)) {
        getship(&s, (int)ship->destshipno);
        s->docked = 0;
        s->whatdest = LEVEL_UNIV;
        putship(s);
        free(s);
    }

    /* Landed ships are killed */
    if (ship->ships) {
        sh = ship->ships;

        while (sh) {
            if (!valid_ship(sh)) {
                sh = 0;

                continue;
            }

            getship(&s, sh);
            kill_ship(playernum, s);
            putship(s);
            sh = nextship(s);
            free(s);
        }
    }

    if (ship->docked && (ship->whatorbits == LEVEL_PLAN)) {
        /* Remove from list */
        remove_sh_plan(ship);
    }

    return 1;
}
