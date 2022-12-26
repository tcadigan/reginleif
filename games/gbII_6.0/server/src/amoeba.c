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
 * *****************************************************************************
 * Galactic Bloodshed Space Amoeba
 *
 * Hacker: Michael F. Wilkinson (mfw)
 *
 * Contains:
 *   amoeba-related functions
 *
 * Take from original GB, ported, and expanded for GB+. Way, WAY back when I
 * heard about this I loved the idea. Obviously, the code was started but never
 * finished back then. Here I've got it to a working state. However, this code
 * is still untested/buggy.
 *
 * *****************************************************************************
 *
 * Amoeba.c Space amoeba code
 *
 * What is a Space Ameoba? A Space Amoeba is a huge single-celled creature that
 * flies through the universe searching for food.
 *
 * What does it eat? Biomass. The Space Ameoba searches out planets of high
 * biomass.
 *
 * What does it do when if finds a planet of high biomass? It consumes. And
 * consumes. Each update, a space amoeba can consume it's "popn" in biomass. It
 * converts the bio,ass to fuel on the next update. A space ameoba will continue
 * to consume biomass from a planet until it is "full" (has filled its fuel
 * capacity). It will then leave for the depths of space again.
 *
 * But, where do they come from? An amoeba just appears, at some point in the
 * game. There is a chance per turn (defined bu AMOEBA_CHANCE) of one being
 * created. An amoeba will be created at a science research station.
 *
 * Another way for the Amoebas to be created is through cellular division. For
 * every amoeba in existence, there is AMOEBA_REPRODUCE% that it will divide
 * into two amoebas per update, if AMOEBAS_REPRODUCE is true.
 *
 * The final way for Amoebas to be created is if it has over AMOEBA_DIVIDE%
 * damage at the update. A damaged amoeba will split into two amoebas with half
 * the damage of the original amoeba. Thus, a player must be sure to *destroy*
 * an amoeba. Wounding an amoeba is not a Good Thing.
 */
#include "amoeba.h"

#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "config.h"
#include "debug.h"
#include "files_shl.h"
#include "log.h"
#include "max.h"
#include "rand.h"
#include "power.h"
#include "races.h"
#include "ships.h"
#include "tweakables.h"
#include "vars.h"

#include "build.h"
#include "load.h"
#include "tele.h"

#ifdef USE_AMOEBA

#define AMOEBA_DIVIDE 50
#define AMOEBAS_REPRODUCE 5
#define AMOEBA_CONVERSION_RATE 3

/* Max number of point amoeba heals per segment */
#define AMOEBA_HEAL_RATE 5

/* Number of popn consumed to heal one damage point */
#define AMOEBA_HEAL_POPN 10

#define WAITING 0
#define TRAVELING 1
#define JUST_GRAZED 2
#define GROWING 3

void do_amoeba(shiptype *a)
{
    /*
     * General algorithm:
     *   If amoeba is damaged over AMOEBA_DIVIDE, make an amoeba. If the amoeba
     *   is full and at a planet, then set dest to another star. If amoeba is
     *   full and at a star, then set dest to another star. If amoeba does not
     *   have a destination and is at universal scope, set dest to a star. If
     *   amoeba is hungry and at a star, set dest to a planet. If amoeba is
     *   hungry and at a planet, well, doplanet.c will call eat_amoeba.
     */
    sprintf(buf, "do_amoeba(): In function.\n");
    debug(LEVEL_GENERAL, buf);

    if (a->alive) {
        if (!a->fuel) {
            /* The amoeba expires if we run out of gas */
            sprintf(buf, "do_amoeba(): #%d expires!\n", a->number);
            debug(LEVEL_GENERAL, buf);
            a->alive = 0;

            return;
        }

        if (!a->hyper_drive.has) {
            /* Upgrade hyper */
            a->hyper_drive.has = 1;
            a->mounted = 1;
        }

        if (!a->deststar || !a->destpnum) {
            /* We've lost our way... */
            sprintf(buf, "do_amoeba(): #%d got lost.\n", a->number);
            debug(LEVEL_GENERAL, buf);
            a->special.amoeba.doing = WAITING;
        }

        /*
         * The smallest (starting amoeba) has a max_fuel capacity of 1000, we
         * don't what to create on smaller than that. When the amoeba divides it
         * halves its capacity (popn + fuel) with the 'baby'
         */
        if ((a->max_fuel >= 2000)
            && ((a->damage >= AMOEBA_DIVIDE)
                || (int_rand(1, 100) <= AMOEBAS_REPRODUCE))) {
            /* Split into a new pair of amoeba */
            float f; /* Fuel to give */
            int p; /* Popn to give */
            int d; /* Damage to give */
            int newam; /* Number of new ameoba */
            shiptype *baby;

            if (!shipSlotsAvail()) {
                sprintf(buf,
                        "do_amoeba(): No ship slots available for new amoeba.");
                loginfo(ERRORLOG, WANTERRNO, buf);
            } else {
                sprintf(buf, "do_amoeba(): #%d divides.\n", a->number);
                debug(LEVEL_GENERAL, buf);

                /*
                 * Fuel is relative to size with the amoeba, because it halves its
                 * mass it's going to keep the same amount of fuel
                 */
                f = a->fuel;

                /* But the popn is divided between the two */
                p = a->popn / 2;

                /* Damage is halved, this causes healing */
                d = a->damage / 2;

                a->max_fuel /= 2;
                a->max_crew /= 2;

                use_popn(a, p, 1);
                a->damage -= d;

                newam = getFreeShip(OTYPE_AMOEBA, 0);

                if (!newam) {
                    sprintf(buf, "do_amoeba(): No free ship returned from getFreeShip()");
                    loginfo(ERRORLOG, WANTERRNO, buf);
                } else {
                    if (newam <= Num_ships) {
                        /*
                         * We have an available slot in the free_ship_list, use
                         * it
                         */
                        baby = ships[newam];
                    } else {
                        /* Nothing in the free list, make a new one */
                        baby = (shiptype *)malloc(sizeof(shiptype));

                        if (!baby) {
                            loginfo(ERRORLOG,
                                    WANTERRNO,
                                    "FATAL: Malloc failure [do_amoeba]");

                            exit(1);
                        }

                        /* Augment size of ships pointer */
                        ++Num_ships;

                        /*
                         * Reallocate the ship array making room for the new
                         * amoeba
                         */
                        ships = (shiptype **)realloc(ships, (Num_ships + 1) * sizeof(shiptype *));

                        /* Append the baby amoeba to the ship array */
                        ships[Num_ships] = baby;

                        newam = Num_ships;
                    }

                    Getship(baby, OTYPE_AMOEBA, races[a->owner - 1]);

                    /* We know our own number */
                    baby->number = newam;

                    /* Ownership */
                    baby->owner = a->owner;
                    baby->governor = a->governor;

                    /* Our location in space is the same as the parent's */
                    baby->whatorbits = a->whatorbits;
                    baby->storbits = a->storbits;
                    baby->pnumorbits = a->pnumorbits;
                    baby->docked = a->docked;
                    baby->whatdest = a->whatdest;
                    baby->deststar = a->deststar;
                    baby->destpnum = a->destpnum;
                    baby->land_x = a->land_x;
                    baby->land_y = a->land_y;
                    baby->xpos = a->xpos;
                    baby->ypos = a->ypos;

                    /* Particulars */
                    baby->speed = baby->max_speed;
                    baby->alive = 1; /* Is not dead */
                    baby->mode = 0;
                    baby->cloaked = a->cloaked;
                    baby->ships = 0; /* Nor do they have hangers */
                    baby->on = 1; /* Ready to go */
                    baby->active = 1;

                    baby->damage = a->damage;
                    baby->max_fuel = a->max_fuel;
                    baby->max_crew = a->max_crew;

                    rcv_fuel(baby, f);
                    rcv_popn(baby, p, 1);

                    /* Insert the baby into the ship list */
                    baby->nextship = nextship(a);
                    a->nextship = nextship(baby); /* Num_ships points to baby */

                    putship(baby);
                }
            }
        } /* Reproduction */

        if (a->special.amoeba.doing == GROWING) {
            /* Should happen after a jump */
            a->max_fuel *= 2;
            a->max_crew *= 2;
            a->size = ship_size(a);
            a->base_mass = getmass(a);
            a->mass = a->base_mass;
            a->special.amoeba.doing = TRAVELING;
            sprintf(buf, "do_amoeba(): Grew to %d\n", a->max_fuel);
            debug(LEVEL_GENERAL, buf);
        }

        if (a->damage) {
            /* Use food stores to repair damage */
            int heal = 0;

            sprintf(buf, "do_amoeba(): Has %d damage.\n", a->damage);
            debug(LEVEL_GENERAL, buf);
            heal = MIN(a->damage, AMOEBA_HEAL_RATE);

            if ((heal * AMOEBA_HEAL_POPN) > a->popn) {
                heal = a->popn / AMOEBA_HEAL_POPN;
            }

            a->damage -= heal;
            use_popn(a, (heal * AMOEBA_HEAL_POPN), 1);
            sprintf(buf, "do_amoeba(): Heals %d damage.\n", heal);
            debug(LEVEL_GENERAL, buf);
        }

        if ((a->popn > 1)
            && (((a->whatorbits == LEVEL_PLAN) && (a->fuel < a->max_fuel))
                || ((a->whatorbits != LEVEL_PLAN)
                    && (a->fuel < (a->max_fuel * 0.8))))) {
            /* Convert food stores to fuel */
            int p2f;
            int needed;
            int popnused;

            needed = a->max_fuel - a->fuel;
            popnused = a->popn - 1;
            p2f = (int)((float)popnused * AMOEBA_CONVERSION_RATE);

            if (needed < p2f) {
                popnused = needed / AMOEBA_CONVERSION_RATE;
                p2f = popnused * AMOEBA_CONVERSION_RATE;
            }

            use_popn(a, popnused, 1);

            /* Just in case */
            if ((p2f + a->fuel) > a->max_fuel) {
                p2f = a->max_fuel - a->fuel;
            }

            // rcv_fuel(a, p2f);
            a->fuel += p2f;
            a->mass += (p2f * MASS_FUEL);

            sprintf(buf,
                    "do_amoeba(): Converting %d population to %d fuel.\n",
                    popnused,
                    p2f);

            debug(LEVEL_GENERAL, buf);
        }

        /*
         * We must always have at least one crew otherwise the ship will drift
         */
        if (!a->popn) {
            rcv_popn(a, 1, 1);
        }

        if ((a->special.amoeba.doing = JUST_GRAZED)
            && (a->fuel >= (a->max_fuel * 0.95))) {
            /* We are full. Go to another place. */
            sprintf(buf, "do_amoeba(): Fuel is full.\n");
            debug(LEVEL_GENERAL, buf);
            a->special.amoeba.doing = WAITING;
        }

        /* Is the amoeba in need of a place to go? */
        if (a->special.amoeba.doing == WAITING) {
            /* Find the next planet or star */
            int pug;
            int bog;
            int start;

            sprintf(buf, "do_amoeba(): Find new destination.\n");
            debug(LEVEL_GENERAL, buf);
            pug = int_rand(0, Sdata.numstars - 1);
            start = pug;

            while (!(Stars[pug]->inhabited[0] + Stars[pug]->inhabited[1])) {
                ++pug;

                /* Skip where we are currently */
                if (pug == a->storbits) {
                    ++pug;
                }

                if (pug == Sdata.numstars) {
                    pug = 0;
                }

                if (pug == start) {
                    pug = -1;

                    break;
                }
            }

            if (pug >= 0) {
                bog = int_rand(0, Stars[pug]->numplanets - 1);
                start = bog;

                while (!planets[pug][bog]->popn) {
                    ++bog;

                    if (bog == Stars[pug]->numplanets) {
                        bog = 0;
                    }

                    if (bog == start) {
                        bog = -1;

                        break;
                    }
                }
            }

            if ((pug < 0)
                || (bog < 0)
                || ((a->storbits == pug) && (a->pnumorbits == bog))) {
                /*
                 * We didn't find an inhabited star/planet, or we chose the same
                 * one we're at. Let's pause for now and try again the next time
                 * around.
                 */

                a->whatdest = LEVEL_UNIV;
                a->special.amoeba.doing = WAITING;
                a->speed = 0;
                sprintf(buf, "do_amoeba: No destination %d/%d.\n", pug, bog);
                debug(LEVEL_GENERAL, buf);
            } else {
                a->whatdest = LEVEL_PLAN;
                a->deststar = pug;
                a->destpnum = bog;
                a->docked = 0;
                a->speed = Shipdata[OTYPE_AMOEBA][ABIL_SPEED];
                /* Flag that we have been set on course */
                a->special.amoeba.doing = TRAVELING;
                sprintf(buf, "do_amoeba(): Destination set %d/%d.\n", pug, bog);
                debug(LEVEL_GENERAL, buf);
            }
        } /* Course set */

        if ((a->whatorbits == LEVEL_STAR)
            || (a->special.amoeba.doing == WAITING)) {
            a->cloak = 1;
        } else if (a->whatorbits == LEVEL_UNIV) {
            if (a->hyper_drive.has && a->mounted) {
                a->hyper_drive.on = 1;
                a->hyper_drive.ready = 1;
            }

            /* A chance that the amoeba will grow in deep space */
            int min_fuel;

            if (a->max_fuel < 1000) {
                min_fuel = 0;
            } else {
                min_fuel = 1;
            }

            if ((a->max_fuel < 8000) && (int_rand(0, min_fuel))) {
                a->special.amoeba.doing = GROWING;
                sprintf(buf, "do_amoeba(): Gonna grow\n");
                debug(LEVEL_GENERAL, buf);
            }

            a->cloak = 0;
        } else {
            a->cloak = 0;
        }
    } /* Amoeba was alive */

    putship(a);
}

void amoeba_planet(shiptype *a, planettype *planet)
{
    int px;
    int py;
    int max;
    int atx;
    int aty;
    sectortype *s;

    max = 0;
    atx = 0;
    aty = 0;

    sprintf(buf, "amoeba_planet(): In function.\n");
    debug(LEVEL_GENERAL, buf);

    if (a->alive
        && a->special.amoeba.doing
        && (a->whatorbits == LEVEL_PLAN)
        && (a->storbits == a->deststar)
        && (a->pnumorbits == a->destpnum)) {
        /* We're here */
        sprintf(buf, "amoeba_planet(): We've arrived.\n");
        debug(LEVEL_GENERAL, buf);

        /* Find the heaviest populated sector */
        for (px = 0; px < planet->Maxx; ++px) {
            for (py = 0; py < planet->Maxy; ++py) {
                s = &Sector(*planet, px, py);

                if (s->popn > max) {
                    atx = px;
                    aty = py;
                    max = s->popn;
                }
            }
        }

        if (max && ((a->fuel < a->max_fuel) || (a->popn < a->max_crew))) {
            /* GRAZE!!! */

            /* Get that sector */
            s = &Sector(*planet, atx, aty);

            if (max > s->popn) {
                max = s->popn; /* Just *checking* */
            }

            if (max >= (a->max_crew - a->popn)) {
                max = a->max_crew - a->popn; /* As much as it can swallow */
            }

            a->land_x = atx;
            a->land_y = aty;
            a->docked = 1;

            sprintf(buf,
                    "A dreaded space amoeba consumed %d of your population at %s!\n",
                    max,
                    prin_ship_orbits(a));

            push_telegram(s->owner, 0, buf);
            s->popn -= max;
            planet->popn -= max;

            if (!s->popn) {
                s->owner = 0;
            }

            if ((a->popn + max) > a->max_crew) {
                max = a->max_crew - a->popn;
            }

            sprintf(buf, "amoeba_planet(): Grazing %d people!!!\n", max);
            debug(LEVEL_GENERAL, buf);
            rcv_popn(a, max, 1);
            a->special.amoeba.doing = JUST_GRAZED;
            putsector(s, planet, atx, aty);
            putplanet(planet, (int)a->storbits, (int)a->pnumorbits);
        } else {
            sprintf(buf, "amoeba_planet(): Empty planet, move on.\n");
            debug(LEVEL_GENERAL, buf);

            /*
             * This planet is empty, set special.amoeba.doing off so we can move
             * on
             */
            a->special.amoeba.doing = WAITING;
        }
    } else {
        /*
         * Continue on our way, this is taken care of automatically in doship()
         */
        sprintf(buf, "amoeba_planet(): Continue on our way.\n");
        debug(LEVEL_GENERAL, buf);
    }

    putship(a);
}

#endif /* USE_AMOEBA */
