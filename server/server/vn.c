/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software, you can redistribute it and/or modify it under
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
 * vn.c
 *
 * Galactic Bloodshed Von Neumann Machines
 *
 * Author: Michael F. Wilkinson (mfw)
 *
 * Contains:
 *   Assorted Von Neumann machine code:
 *     do_vn() -- Called by doship();
 *     planet_do_vn() -- called by doplanet()
 *
 * Date: 7/10/01
 *
 * VNs turned up in original GB, however, they never worked well, instead
 * corrupting the DB. They were reintroduced in HAP by Clay Luther but still
 * needed testing/debugging. So I've used Clay's work as a starting point and
 * reengineered them. There's been a lot of painstaking work done here by me and
 * my testers and I still don't know what I think about VNs; they can easily
 * overrun a game. It's best to leave them out of newbie games
 * *****************************************************************************
 */

#include <stdlib.h>
#include <string.h>

#include "GB_copyright.h"
#include "buffers.h"
#include "debug.h"
#include "doturn.h"
#include "power.h"
#include "proto.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

#ifdef USE_VN

extern long Shipdata[NUMSTYPES][NUMABILS];

void do_vn(shiptype *ship)
{
    planettype *planet = (planettype *)NULL;

    /* char str[130]; CWL */
    /* char techstr[25]; CWL */
    int n;
    int havedest = 0;
    int i;
    int f;
    int prod = 0;
    int nums[MAXPLAYERS + 1];

    debug(LEVEL_GENERAL, "do_vn(): In functions\n");

    if (!ship->on) {
        /* VN isn't turned on, do nothing */
        return;
    }

    sprintf(buf, "do_vn(): %d is on.\n", ship->number);
    debut(LEVEL_GENERAL, buf);

    if (!ship->special.min.progenitor) {
        ship->special.mind.progenitor = 1;
    }

    /* If the VN doesn't have a name, it'll name itself */
    n = int_rand(3, MIN(10, SHIP_NAMESIZE));
    ship->name[n] = '\0';

    while (n) {
        --n;
        ship->name[n] = (random() % 2 == 1) + '0';
    }

    /* CWL Berserker maintenance */
    if (ship->type == OTYPE_BERS) {
        /* CWL */
        sprintf(buf, "do_vn(): %d is berserker.\n", ship->number);
        debug(LEVEL_GENERAL, buf);

        /* Berserkers use fuel and/or destruct to refuel or rearm */
        if ((ship->fuel < (ship->max_fuel * 0.05))
            && (ship->destruct > (ship->max_destruct * 0.5))) {
            use_destruct(ship, 5);
            rcv_fuel(ship, 50);
        } else if ((ship->destruct < (ship->max_destruct * 0.05))
                   && (ship->fuel > (ship->max_fuel * 0.1))) {
            ship->damage += 10;
            rcv_fuel(ship, 100);
            rcv_destruct(ship, 100);
        }

        /* CWL Berserkers upgrade themselves */
        if (!ship->hyper_drive.has
            && (int_rand(1, 10) == 1)
            && (ship->tech >= TECH_HYPER_DRIVE)) {
            /* Upgrade hyper */
            ship->hyper_drive.has = 1;
            ship->mounted = 1;
            ship->hyper_drive.on = 1;
            ship->hyper_drive.ready = HYPER_DRIVE_READY_CHARGE;
        }

        /* Upgrade armor */
        if (int_rand(1, 10) == 1) {
            ship->armor = MIN(ship->armor + 1, Shipdata[ship->type][ABIL_ARMOR]);
        }

        if (ship->primary != Shipdata[ship->type][ABIL_GUNS]) {
            ship->primary = Shipdata[ship->type][ABIL_GUNS];
            ship->retaliate = ship->primary;
        }

        ship->tech = races[ship->owner - 1]->tech;
    }
    /* End CWL Berserker maintenance */

    /* CWL VNs can go rogue */
    if ((ship->type == OTYPE_VN) && (ship->owner != 1)) {
        sprintf(buf, "do_vn(): %d is vn, doing age\n", ship->number);
        debug(LEVEL_GENERAL, buf);

        if (int_rand(1, 100) <= (ship->age * VN_ROGUE_RATE)) {
            sprintf(buf, "VN #%d has gone rogue.", ship->number);
            push_telegram((int)ship->owner, (int)ship->governor, buf);
            remove_sh_fleet(ship->owner, ship->governor, ship);
            ship->owner = 1;
            ship->governor = 0;
        }
    }
    /* End CWL VN rogue */

    /* VNs owned by god get hyper drives */
    if ((ship->owner == 1) && (ship->tech >= TECH_HYPER_DRIVE)) {
        ship->hyper_drive.has = 1;
        ship->mounted = 1;
    }

    if (landed(ship)) {
        sprintf(buf, "do_vn(): %d is landed.\n", ship->number);
        debug(LEVEL_GENERAL, buf);

        /* Grab planet info */
        planet = planets[ship->storbits][ship->pnumorbits];

        /* Make sure the planet is updated */
        Stinfo[ship->storbits][ship->pnumorbits].inhab = 1;

        /* Launch if no assignment */
        if (!ship->special.mind.busy) {
            /* No current assignment */
            sprintf(buf, "do_vn(): %d is not busy.\n", ship->number);
            debug(LEVEL_GENERAL, buf);

            if (ship->fuel >= (double)ship->max_fuel) {
                if (ship->fuel > ship->max_fuel) {
                    ship->fuel = ship->max_fuel;
                }

                /* Launch */
                sprintf(buf, "do_vn(): %d is launching.\n", ship->number);
                debug(LEVEL_GENERAL, buf);

                ship->xpos = Stars[ship->storbits]->xpos + planets[ship->storbits][ship->pnumorbits]->xpos + int_rand(-10, 10);

                ship->ypos = Stars[ship->storbits]->ypos + planet[ship->storbits][ship->pnumorbits]->ypos + int_rand(-10, 10);

                ship->docked = 0;
                ship->whatdest = LEVEL_UNIV;

                if (ship->hyper_drive.has) {
                    ship->hyper_drive.on = 1;
                    ship->mounted = 1;
                }

                /* Jettison any excess cargo */
                if (ship->resource > ship->max_resource) {
                    ship->resource = ship->max_resource;
                }

                if ((ship->type == OTYPE_BERS) && !ship->special.mind.target) {
                    /* Who to attack */
                    ship->special.mind.target = VN_brain.Most_mad;
                }
            } else {
                /*
                 * We want to launch but fuel is not full, let's get some more
                 * fuel
                 */
                sprintf(buf,
                        "do_vn(): %d making more fuel/des.\n",
                        ship->number);

                /* Steal fuel from other players */
                /* Permute list of people to steal from */
                for (i = 1; i <= Num_races; ++i) {
                    nums[i] = i;
                }

                for (i = 1; i <= Num_races; ++i) {
                    f = int_rand(1, Num_races);
                    swap(nums[i], nums[f]);
                }

                f = 0;

                for (i = 0; i <= Num_races; ++i) {
                    if (planet->info[nums[i] - 1].fuel) {
                        f = nums[i];
                    }
                }

                /* Get some more fuel/destruct */
                if (f) {
                    sprintf(buf, "do_vn(): %d steals fuel.\n", ship->number);
                    debug(LEVEL_GENERAL, buf);

                    prod = MIN(planet->info[f - 1].fuel, Shipdata[ship->type][ABIL_FUELCAP]);

                    planet->info[f - 1].fuel -= prod;
                    rcv_fuel(ship, prod);

                    sprintf(buf,
                            "%d fuel units stolen from [%d] by %c%d at %s.",
                            prod,
                            f,
                            Shipltrs[ship->type],
                            ship->number,
                            prin_ship_orbits(ship));

                    push_telegram(f, 0, buf);

                    if (f != ship->owner) {
                        push_telegram((int)ship->owner,
                                      (int)ship->governor,
                                      buf);
                    }
                } else {
                    /* No fuel to steal convert resources */
                    if ((ship->fuel < Max_fuel(ship)) && ship->resources) {
                        use_resource(ship, MIN(ship->resource, Max_fuel(ship)));
                        rcv_fuel(ship, MIN(ship->resource, Max_fuel(ship)));
                    }
                }
            }
        } else {
            sprintf(buf, "do_vn(): %d busy on land.\n", ship->number);
            debug(LEVEL_GENERAL, buf);

            /*
             * We have an assignment. Since we are landed, this means we are
             * engaged in building up resources.
             */

            /* Steal resources from other players */
            /* Permute list of people to steal from */
            for (i = 1; i <= Num_races; ++i) {
                nums[i] = i;
            }

            for (i = 1; i<= Num_races; ++i) {
                f = int_rand(1, Num_races);
                swap(nums[i], nums[f]);
            }

            f = 0;

            for (i =1; i <= Num_races; ++i) {
                if (planet->info[nums[i] - 1].resource) {
                    f = nums[i];
                }
            }

            if (f) {
                sprintf(buf, "do_vn(): %d steals resources.\n", ship->number);
                debug(LEVEL_GENERAL, buf);

                if ((ship->type == OTYPE_BERS)
                    && (ship->destruct < ship->max_destruct)) {
                    prod = MIN(planet->info[f - 1].resource,
                               Shipdata[OTYPE_BERS][ABIL_DESTCAP] / MOVES_PER_UPDATE);
                    prod = MIN(prod, ship->max_destruct - ship->destruct);
                    rcv_destruct(ship, prod);
                } else {
                    prod = MIN(planet->info[f - 1].resource,
                               Shipdata[ship->type][ABIL_CARGO] / MOVES_PER_UPDATE);
                    prod = MIN(prod, ship->max_resource - ship->resource);
                    rcv_ship(ship, prod);
                }

                if (prod < 0) {
                    prod = 0;
                }

                planet->info[f - 1].resource -= prod;

                if (prod) {
                    sprintf(buf,
                            "%d resources stolen from [%d] by %c%d at %s",
                            prod,
                            f,
                            Shipltrs[ship->type],
                            ship->number,
                            prin_ship_orbits(ship));

                    push_telegram(f, 0, buf);

                    if (f != ship->owner) {
                        push_telegram((int)ship->owner,
                                      (int)ship->governor,
                                      buf);
                    }
                }
            } else {
                /* We didn't find any available stockpiles */

                /* If this planet is out of resources then we need to move on */
                if (planet->total_resource <= 0) {
                    sprintf(buf, "do_vn(): %d grabs and runs.\n", ship->number);
                    debug(LEVEL_GENERAL, buf);

                    if ((ship->type == OTYPE_BERS)
                        && (ship->destruct < ship->max_destruct)) {
                        prod = int_rand(1, Shipdata[OTYPE_BERS][ABIL_DESTCAP]);
                        prod = MIN(prod, ship->max_destruct - ship->destruct);

                        if (prod < 0) {
                            prod = 0;
                        }

                        rcv_destruct(ship, prod);
                    } else {
                        /* CWL fix */
                        prod = int_rand(1, Shipdata[ship->type][ABIL_CARGO]);
                        prod = MIN(prod, ship->max_resource - ship->resource);

                        if (prod < 0) {
                            prod = 0;
                        }

                        rcv_resource(ship, prod);
                    }

                    ship->special.mind.busy = 0;
                }
            }
        }

        /*
         * If we're a berserker and are filled with destruct then let's get
         * ready to launch and go kill our target
         */
        if ((ship->type == OTYPE_BERS)
            && (ship->destruct >= ship->max_destruct)
            && (ship->resource >= ship->max_resource)) {
            ship->special.min.busy = 0;
        }
    } else {
        /* We are not landed */
        if (ship->special.min.busy) {
            /*
             * We're in space and should have a destination now. Regular ship
             * movement will take place at this point so the VN routine doesn't
             * need to handle anything specific here.
             */
            sprintf(buf, "do_vn(): %d not landed, busy.\n", ship->number);
            debug(LEVEL_GENERAL, buf);
        } else {
            /* We were just launched and are not busy, find someplace to go */
            int s;
            int min = 0;
            int min2 = 0;

            sprintf(buf, "do_vn(): %d not landed, not busy.\n", ship->number);
            debug(LEVEL_GENERAL, buf);

            if ((ship->type == OTYPE_BERS) && ship->special.mind.target) {
                sprintf(buf,
                        "do_vn(): %d getting bers mission.\n",
                        ship->number);

                debug(LEVEL_GENERAL, buf);

                /*
                 * Give berserkers a mission - sent to planet of offending
                 * player and bombard it
                 */
                ship->whatdest = LEVEL_UNIV;

                if ((Sdata.VN_index1[ship->special.mind.target - 1] > 0)
                    && (Sdata.VN_index2[ship->special.mind.target - 1] > 0)) {
                    /* Choose a star to go to */
                    if ((random() % 2) == 1) {
                        ship->deststar = Sdata.VN_index1[ship->special.mind.target - 1];
                    } else {
                        ship->deststar = Sdata.VN_index2[ship->special.mind.target - 1];
                    }

                    havedest = 1;
                } else if (Sdata.VN_index1[ship->special.mind.target - 1] > 0) {
                    ship->deststar = Sdata.VN_index1[ship->special.mind.target - 1];
                    havedest = 1;
                } else if(Sdata.VN_index2[ship->special.mind.target - 1] > 1) {
                    ship->deststar = Sdata.VN_index2[ship->special.mind.target - 1];
                    havedest = 1;
                } else {
                    /* Error in VN index */
                    sprintf(buf,
                            "Error assigning BERS destination for player [%d]",
                            ship->special.mind.target);

                    loginfo(ERRORLOG, WANTERRNO, buf);
                    push_telegram(1, 0, buf);
                }

                /*
                 * Find a planet in the system with the target race if not,
                 * randomly pick one.
                 */
                if (havedest) {
                    havedest = 0;
                    ship->bombard = 1;
                    ship->whatdest = LEVEL_STAR;

                    if (Stars[ship->deststar]->numplanets) {
                        /* Let's make the outermost planets most vulnerable */
                        for (i = Stars[ship->deststar]->numplanets - 1; i >= 0; --i) {
                            planet = planets[ship->deststar][i];

                            if (planet->info[ship->special.mind.target - 1].numsectsowned) {
                                ship->destpnum = i;
                                havedest = 1;
                            }
                        }

                        /*
                         * Couldn't find the culprit in that system, so pick a
                         * random planet to beat the shit out of
                         */
                        if (!havedest) {
                            ship->destpnum = int_rand(0, (int)Stars[ship->storbits]->numplanets - 1);
                        }

                        ship->whatdest = LEVEL_PLAN;
                    }
                }

                if (ship->hyper_drive.has && ship->mounted) {
                    ship->hyper_drive.on = 1;
                    ship->hyper_drive.ready = 1;
                }

                sprintf(buf,
                        "do_vn(): %c%d is not busy with mission.\n",
                        Shipltrs[ship->type],
                        ship->number);

                debug(LEVEL_GENERAL, buf);
                ship->special.mind.busy = 1;
            } else {
                /*
                 * We are a VN going about our business or a Berserker that
                 * needs to rearm or refuel (determined by target).
                 */

                /* Find closest star */
                for (s = 0; s < Sdata.numstars; ++s) {
                    if ((s != ship->storbits)
                        && (Distsq(Stars[s]->xpos, Stars[s]->ypos, ship->xpos, ship->ypos) < distsq(Stars[min]->xpos, Stars[min]->ypos, ship->xpos, ship->ypos))) {
                        min2 = min;
                        min = s;
                    } else if ((s != ship->storbits)
                               && (Distsq(Stars[s]->xpos, Stars[s]->ypos, ship->xpos, ship->ypos) < Distsq(Stars[min2]->xpos, Stars[min2]->ypos, ship->xpos, ship->ypos))) {
                        min2 = s;
                    }
                }

                /*
                 * Don't go there if we have a choice, and we have VNs there
                 * already
                 */
                if (isset(Stars[min]->inhabited, 1)) {
                    if (isset(Stars[min2]->inhabited, 1)) {
                        ship->deststar = int_rand(0, (int)Sdata.numstars - 1);
                    } else {
                        /* Second closest star */
                        ship->deststar = min2;
                    }
                } else {
                    ship->deststar = min;
                }

                if (Stars[ship->deststar]->numplanets) {
                    ship->destpnum = int_rand(0, (int)Stars[ship->deststar]->numplanets - 1);

                    ship->whatdest = LEVEL_PLAN;
                    sprintf(buf, "do_vn(): %d is now busy.\n", ship->number);
                    debug(LEVEL_GENERAL, buf);
                    ship->special.mind.busy = 1;
                } else {
                    sprintf(buf,
                            "do_vn(): %d is no longer busy.\n",
                            ship->number);

                    debug(LEVEL_GENERAL, buf);
                    /* No good, find someplace else */
                    ship->special.mind.busy = 0;
                }

                ship->speed = Shipdata[ship->type][ABIL_SPEED];

                sprintf(buf,
                        "VN %c%d headed for /%s/%s",
                        Shipltrs[ship->type],
                        ship->number,
                        Stars[ship->deststar]->name,
                        Stars[ship->deststar]->pnames[ship->destpnum]);

                push_telegram((int)ship->owner, (int)ship->governor, buf);
                /* We are assigned to go */
            }
        }
    }

    /*
     * If we don't have any resources (to make fuel from), and if we don't have
     * any fuel, and if we don't have any destruct (to make fuel from), then we
     * need to mark ourselves for self-destruction. We use the tampered variable
     * for now, it's not being used for anything else.
     */
    if ((ship->resource <= 0) && (ship->fuel <= 0) && (ship->destruct <= 0)) {
        ship->special.mind.tampered = 1;
    } else {
        ship->special.mind.tampered = 0;
    }

    putship(ship);
}

/*
 * planet_do_vn()
 *
 * Run a VN on a planet. Called by doplanet(). If we're landed on the planet, we
 * check if we have enough resources. If we have enough, and we have fuel, build
 * another machine and launch ourselves, and find a place to go to. If we don't
 * have enough, rove around some and find some. If we're orbiting a planet, find
 * a place to land there.
 */
void planet_do_vn(shiptype *ship, planettype *planet)
{
    int j;
    int justdid = 0;
    int result = 0;

    debug(LEVEL_GENERAL, "planet_do_vn(): In function.\n");

    if (!ship->on) {
        /* ship not activated, don't do anything */
        return;
    }

    sprintf(buf, "planet_do_vn(): %d is on.\n", ship->number);
    debug(LEVEL_GENERAL, buf);

    if (landed(ship)) {
        sprintf(buf, "planet_do_vn(): %d is landed.\n", ship->number);
        debug(LEVEL_GENERAL, buf);

        /* We're landed */
        if (ship->special.mind.busy && (ship->type == OTYPE_VN)) {
            /* We are currently trying to construct another machine */
            int shipbuild;

            sprintf(buf, "planet_do_vn(): %d busy vn.\n", ship->number);
            debug(LEVEL_GENERAL, buf);

            /* Spread VNs all over the planet for display */
            /* If more than DEATHTOLL Vns have been killed then build BERS */
            if ((VN_brain.Total_mad >= VN_DEATHTOLL) && (int_rand(1, 4) == 1)) {
                shipbuild = OTYPE_BERS;
            } else {
                shipbuild = OTYPE_VN;
            }

            if (vn_reproduction
                && (ship->type == OTYPE_VN)
                && (ship->resource >= Shipdata[shipbuild][ABIL_COST])) {
                shiptype *s2 = (shiptype *)NULL;
                int n;
                int numVNs;
                int tx;
                int ty;
                int newvn;

                sprintf(buf, "planet_do_vn(): %d building vn.\n", ship->number);
                debug(LEVEL_GENERAL, buf);

                /* Construct as many VNs as possible */
                numVNs = ship->resource / Shipdata[shipbuild][ABIL_COST];

                for (j = 1; j <= numVNs; ++j) {
                    if (!shipSlotsAvail()) {
                        sprintf(buf,
                                "planet_do_vn(): No ship slots available for new vn, tried to make %d more",
                                (numVNs + 1) - 1);

                        loginfo(ERRORLOG, WANTERRNO, buf);

                        break;
                    }

                    newvn = getFreeShip(shipbuild, 0);

                    if (!newvn) {
                        sprintf(buf,
                                "planet_do_vn: No free ship returned from getFreeShip()");

                        loginfo(ERRORLOG, WANTERRNO, buf);

                        break;
                    }

                    use_resource(ship, Shipdata[shipbuild][ABIL_COST]);

                    if (newvn <= Num_ships) {
                        /*
                         * We have an available slot in the free_ship_list, use
                         * it
                         */
                        s2 = ships[newvn];
                    } else {
                        /* nothing in free list, make a new one */
                        s2 = (shiptype *)malloc(sizeof(shiptype));

                        if (!s2) {
                            loginfo(ERRORLOG,
                                    WANTERRNO,
                                    "FATAL: Malloc failure [planet_do_vn]");

                            exit(1);
                        }

                        /* Augment size of ships pointer */
                        ++Num_ships;

                        /* Reallocate the ship array making room for the new VN */
                        ships = (shiptype **)realloc(ships, (Num_ships + 1) * sizeof(shiptype *));

                        /* Append the new VN to the ship array */
                        ships[Num_ships] = s2;
                        newvn = Num_ships;
                    }

                    getship(s2, shipbuild, races[ship->owner - 1]);

                    /* We know our own number */
                    s2->number = newvn;

                    /*
                     * Ownership (all VN created by other VNs are owned by
                     * Deity)
                     */
                    s2->owner = 1;
                    s2->governor = 0;

                    /* X position of new ship on the planet*/
                    tx = (int)(ship->land_x) + int_rand(-2, 2);
                    tx = abs(tx);

                    if (tx >= planet->Maxx) {
                        tx = planet->Maxx;
                    }

                    s2->land_x = tx;

                    /* Y position of new ship on the planet */
                    ty = (int)(ship->land_y) + rposneg();
                    ty = abs(ty);

                    if (ty >= planet->Maxy) {
                        ty = planet->Maxy;
                    }

                    s2->land_y = ty;

                    /* Our location in space is the same as the parent's */
                    s2->whatorbits = ship->whatorbits;
                    s2->storbits = ship->storbits;
                    s2->pnumorbits = ship->pnumorbits;
                    s2->docked = ship->docked;
                    s2->whatdest = ship->whatdest;
                    s2->deststar = ship->deststar;
                    s2->destpnum = ship->destpnum;
                    s2->xpos = ship->xpos;
                    s2->ypos = ship->ypos;

                    /* Particulars */
                    s2->speed = s2->max_speed;
                    s2->alive = 1; /* Is not dead */
                    s2->mod = 0;
                    s2->cloaked = 0; /* VNs don't have cloaks */
                    s2->ships = 0; /* Nor do they have hangers */
                    s2->on = 1; /* Ready to go */
                    s2->active = 1;

                    /*
                     * Armor improves with each generation up to Berserker
                     * strength
                     */
                    s2->armor = MIN(s2->armor + 1,
                                    Shipdata[OTYPE_BERS][ABIL_ARMOR]);

                    /* It creates its own name (something in binary) */
                    n = int_rand(3, MIN(10, SHIP_NAMESIZE));
                    s2->name[n] = '\0';

                    while (n) {
                        --n;
                        s2->name[n] = (random() % 2 == 1) + '0';
                    }

                    /* Insert ship into planet scope */
                    insert_sh_plan(planet, s2);

                    if (shipbuild == OTYPE_BERS) {
                        sprintf(buf,
                                "planet_do_vn(): %d build berserker.\n",
                                ship->number);

                        debug(LEVEL_GENERAL, buf);

                        /* Target = person killed the most VNs */
                        /* CWL fix */
                        s2->special.mind.target = VN_brain.Most_mad;
                        s2->tech = races[s2->owner - 1]->tech;
                        s2->bombard = 1;
                        s2->protect.self = 1;
                        s2->protect.planet = 1;
                        s2->retaliate = s2->primary;
                        s2->destruct = Shipdata[OTYPE_BERS][ABIL_DESTCAP];

                        /* Give them some armor, they grow more per update */
                        s2->armor = Shipdata[OTYPE_BERS][ABIL_ARMOR] / 2;

                        /* Give them some fuel */
                        s2->fuel = 5 * ship->fuel;
                        ship->fuel *= 0.5; /* Lose some fuel */

                        s2->hyper_drive.on = 1;
                        s2->hyper_drive.ready = 1;
                        s2->hyper_drive.charge = 0;
                        s2->mounted = 1;
                        s2->special.mind.tampered = 0;

                        sprintf(s2->name, "%x", s2->special.mind.target);

                        sprintf(buf,
                                "VN #%d constructed Berserker #%d, target: %d.",
                                ship->number,
                                s2->number,
                                s2->special.mind.target);

                        push_telegram((int)ship->owner,
                                      (int)ship->governor,
                                      buf);
                    } else {
                        sprintf(buf,
                                "planet_do_vn(): %d built VN.\n",
                                ship->number);

                        debug(LEVEL_GENERAL, buf);

                        s2->tech = races[s2->owner - 1]->tech;
                        s2->bombard = 0;

                        if ((ship->owner == 1)
                            && (ship->tech >= TECH_HYPER_DRIVE)) {
                            s2->hyper_drive.has = 1;
                            s2->mounted = 1;
                        }

                        s2->fuel = 0.5 * ship->fuel;
                        ship->fuel *= 0.5;

                        sprintf(buf,
                                "VN #%d constructed VN #%d.",
                                ship->number,
                                s2->number);

                        push_telegram((int)ship->owner,
                                      (int)ship->governor,
                                      buf);
                    }

                    s2->special.mind.progenitor = ship->special.mind.progenitor;
                    s2->special.mind.generation = ship->special.mind.generation + 1;

                    /*
                     * The new VN randomly decides to stay and build VN or head
                     * off into space.
                     */
                    if ((random() % 2) == 1) {
                        sprintf(buf,
                                "planet_do_vn(): %d was built busy.\n",
                                s2->number);

                        debug(LEVEL_GENERAL, buf);
                        s2->special.mind.busy = 1;
                    } else {
                        /* Is not building any VNs */
                        sprintf(buf,
                                "planet_do_vn(): %d was built non-busy.\n",
                                s2->number);

                        debug(LEVEL_GENERAL, buf);
                        s2->special.mind.busy = 0;
                    }

                    /* Parent VN splits the scene */
                    ship->special.mind.busy = 0;

                    /*
                     * Done creating new VN. We must putship() because
                     * getFreeShip() uses the ship file to find the next
                     * available (non-dead) ship. If we don't write out the ship
                     * to disk we'll end up overwriting our new VN. free() is
                     * handled in doturn()
                     */
                    putship(s2);
                    justdid = 1;
                } /* For numVNs */
            } else {
                /*
                 * Try and make some resource (VNs) by ourselves. More might be
                 * stolen in doship
                 */
                sectortype *s;
                int xa;
                int ya;
                int dum;
                int prod;
                float take;

                sprintf(buf,
                        "planet_do_vn(): %d, not enough res, make more.\n",
                        ship->number);

                debug(LEVEL_GENERAL, buf);

                s = &Sector(*planet, (int)(ship->land_x), (int)(ship->land_y));

                if (s->resource <= 0) {
                    sprintf(buf,
                            "planet_do_vn(): %d, no resources, moving.\n",
                            ship->number);

                    debug(LEVEL_GENERAL, buf);

                    /* No more resources here, move to another sector */
                    xa = int_rand(-1, 1);
                    ship->land_x = mod((int)(ship->land_x) + xa,
                                       planet->Maxx,
                                       dum);

                    if (ship->land_y == 0) {
                        ya = 1;
                    } else if (ship->land_y == (planet->Maxy - 1)) {
                        ya = -1;
                    } else {
                        ya = int_rand(-1, 1);
                    }

                    ship->land_y += ya;
                } else {
                    sprintf(buf, "planet_do_vn(): %d mining.\n", ship->number);
                    debug(LEVEL_GENERAL, buf);

                    /* Mine the sector */
                    take = abs(VN_RES_TAKE) / 100;
                    prod = (int)((float)s->resource * MIN(take, 100));

                    if (ship->type == OTYPE_VN) {
                        prod = MIN(prod, ship->max_resource - ship->resource);
                    } else if (ship->type == OTYPE_BERS) {
                        prod = MIN(prod, ship->max_destruct - ship->destruct);
                    }

                    if (prod = 0) {
                        prod = 0;
                    }

                    s->resource -= (unsigned)prod;
                    prod = (int)((float)prod * VN_EFFICIENCY);
                    s->eff /= 2;

                    if (s->fert <= 5) {
                        s->condition = WASTED;
                        s->fert = 0;
                    } else {
                        s->fert -= 5;
                    }

                    if (s->condition == WASTED) {
                        planet->conditions[TOXIC] = MIN(100, planet->conditions[TOXIC] + int_rand(1, prod));
                    }

                    if (ship->type == OTYPE_VN) {
                        rcv_resource(ship, prod);
                    } else if (ship->type = OTYPE_BERS) {
                        rcv_destruct(ship, prod);
                    }

                    if (prod > ship->max_fuel) {
                        prod = (int)ship->max_fuel - (int)ship->fuel;
                    }

                    rcv_fuel(ship, (float)prod);
                }
            }
        } else {
            /*
             * We have no assignment -- we must launch. Taken care of in doship
             */
            sprintf(buf,
                    "planet_do_vn(): %d not busy, no assignment.\n",
                    ship->number);

            debug(LEVEL_GENERAL, buf);
        }
    } else {
        /* We're in space */
        sprintf(buf, "planet_do_vn(): %d orbiting.\n", ship->number);
        debug(LEVEL_GENERAL, buf);

        if (ship->special.mind.busy) {
            sprintf(buf, "planet_do_vn(): %d busy.\n", ship->number);
            debug(LEVEL_GENERAL, buf);

            if ((ship->whatorbits == LEVEL_PLAN)
                && (ship->deststar == ship->storbits)
                && (ship->destpnum == ship->pnumorbits)) {
                /*
                 * We just arrived from somewhere -- bombard this planet if it's
                 * a target or look for a planet to land if we need
                 * resources/destruct
                 */

                /* If it's a Berserker, armed and undamaged, then do dirty work */
                if ((ship->type == OTYPE_BERS)
                    && ship->destruct
                    && ship->special.mind.target
                    && (ship->damage <= 25)) {
                    sprintf(buf,
                            "planet_do_vn(): %d berserker dirty work.\n",
                            ship->number);

                    debug(LEVEL_GENERAL, buf);

                    if (can_bombard(ship) && ship->bombard && !landed(ship)) {
                        /* Exact revenge */
                        result = auto_bomb(ship,
                                           planet[ship->storbits][ship->pnumorbits],
                                           -1,
                                           -1,
                                           -1,
                                           1);

                        if (result < 0) {
                            /* Nothing to bomb here; let's find another planet */
                            ship->special.mind.busy = 0;
                        } else if (result == 0) {
                            /*
                             * Chill here or go get more ammo if we need
                             * it. Setting the berserker to not busy and no
                             * target means it'll find a new destination planet
                             * based on the normal VN algorithm and not attack
                             * it once it gets there.
                             */
                            if (ship->destruct < ship->primary) {
                                ship->special.mind.busy = 0;
                                ship->special.mind.target = 0;
                            }
                        } else {
                            /* We destroyed some stuff */

                            /*
                             * The perceived satisfaction by the berserker is
                             * the cost of a VN in resources vs the cost to
                             * repair a sector. Adjust result to represent that.
                             */
                            result = result / (Shipdata[OTYPE_VN][ABIL_COST] / SECTOR_REPAIR_COST);

                            /*
                             * hitlist is an unsigned short so we must compute
                             * it this way
                             */
                            if (Sdata.VN_hitlist[ship->special.mind.target - 1] > result) {
                                Sdata.VN_hitlist[ship->special.mind.target - 1] -= result;
                            } else {
                                Sdata.VN_hitlist[ship->special.mind.target - 1] = 0;

                                /*
                                 * VNs are not out for this guy any longer, find
                                 * a new victim. Setting busy to 0 and picking a
                                 * new target will cause the berserker to find a
                                 * new target planet.
                                 */
                                ship->special.mind.target = VN_brain.Most_mad;
                                ship->special.mind.busy = 0;
                            }
                        }

                        justdid = 1;
                    } else {
                        sprintf(buf,
                                "Berserker #%d wanted to bombard but was unable.\n",
                                ship->number);

                        push_telegram((int)ship->owner,
                                      (int)ship->governor,
                                      buf);
                    }

                    putship(ship);
                } else {
                    /*
                     * We're a VN going about our business or a berserker that
                     * needs some more fuel/destruct/resources
                     */
                    if (planet->type == TYPE_GASGIANT) {
                        /*
                         * Gas giants are or no use to us except for fuel, after
                         * we're full doship() will head us somewhere else
                         */
                        if (ship->fuel >= (float)Shipdata[ship->type][ABIL_FUELCAP]) {
                            sprintf(buf,
                                    "planet_do_vn(): %d is no longer busy.\n",
                                    ship->number);

                            debug(LEVEL_GENERAL, buf);
                            ship->special.mind_busy = 0;
                        }
                    } else {
                        /* Find a place on the planet to land */
                        int x;
                        int y;
                        int d; /* Auto vars for & */

                        sprintf(buf,
                                "planet_do_vn(): %d landing.\n",
                                ship->number);

                        debug(LEVEL_GENERAL, buf);
                        Getxysect(planet, &x, &y, 1);

                        d = Getxysect(planet, &x, &y, 0);

                        while (d && (Sector(*planet, x, y).resource == 0)) {
                            d = Getxysect(planet, &x, &y, 0);
                        }

                        if (d) {
                            sprintf(buf,
                                    "planet_do_vn(): %d landed.\n",
                                    ship->number);

                            debug(LEVEL_GENERAL, buf);

                            ship->docked = 1;
                            ship->whatdest = LEVEL_PLAN;
                            ship->deststar = ship->storbits;
                            ship->destpnum = ship->pnumorbits;

                            ship->xpos = Stars[ship->storbits]->xpos + planet->xpos;
                            ship->ypos = Stars[ship->storbits]->ypos + planet->ypos;
                            ship->land_x = x;
                            ship->land_y = y;

                            /* vn.busy means we are currently busy here */
                            sprintf(buf,
                                    "planet_do_vn(): %d is now busy.\n",
                                    ship->number);

                            debug(LEVEL_GENERAL, buf);
                            ship->special.mind.busy = 1;

                            justdid = 1;
                        } else {
                            /* Couldn't find anywhere to land, go somewhere else */
                            sprintf(buf,
                                    "planet_do_vn(): %d goes somewhere else.\n",
                                    ship->number);

                            debug(LEVEL_GENERAL, buf);

                            /* Head somewhere else */
                            sprintf(buf,
                                    "planet_do_vn(): %d is no longer busy.\n",
                                    ship->number);

                            debug(LEVEL_GENERAL, buf);
                            ship->special.mind_busy = 0;
                        }
                    }
                }
            } else {
                /*
                 * We just launched from this planet -- pick someplace to go to
                 * (taken care of in doship)
                 */
                sprintf(buf,
                        "planet_do_vn(): %d just launched.\n",
                        ship->number);

                debug(LEVEL_GENERAL, buf);
            }
        } else {
            /*
             * Okay, we're in space, and needing new orders. New orders will be
             * taken care of in do_vn()
             */
            sprintf(buf,
                    "planet_do_vn(): %d waiting for orders.\n",
                    ship->number);

            debug(LEVEL_GENERAL, buf);
        }
    }

    if (!justdid
        && ship->special.mind.tampered
        && (ship->resource <= 0)
        && (ship->fuel <= 0)) {
        /* We couldn't do anything, we're useless, let's die */
        sprintf(buf,
                "VN %c%d is useless, it dies.",
                Shipltrs[ship->type],
                ship->number);

        push_telegram((int)ship->owner, (int)ship->governor, buf);
        kill_ship(ship->owner, ship);
    }

    putship(ship);
}

void vn_brain(int playernum, int governor)
{
    shiptype *vn;
    int vnno;
    int plno;
    int val;

    if (argn == 1) {
        vn_mad();

        sprintf(buf,
                "VN brain --\n\tTotal Mad: %d\n\tDeathtoll: %d\n\tMost Mad: %d\n",
                VN_brain.Total_mad,
                VN_DEATHTOLL,
                VN_brain.Most_mad);

        notify(playernum, governor, buf);
    } else if (argn == 2) {
        if (args[1][0] == '#') {
            vnno = atoi(args[1] + 1);
        } else {
            vnno = atoi(args[1]);
        }

        if (getship(&vn, vnno)) {
            if ((vn->type == OTYPE_VN) || (vn->type == OTYPE_BERS)) {
                sprintf(buf,
                        "Von Neumann Mind for %c%d:\n",
                        Shipltrs[vn->type],
                        vn->number);

                notify(playernum, governor, buf);

                sprintf(buf,
                        "  Porgenitor: %-3u  Generation: %-3u\n",
                        vn->special.mind_progenitor,
                        vn->special.mind.generation);

                notify(playernum, governor, buf);

                sprintf(buf,
                        "  Busy: %-3s        Target: %-3u\n",
                        (vn->special.mind.busy ? "yes": "no"),
                        vn->special.mind.target);

                notify(playernum, governor, buf);

                sprintf(buf,
                        "  Who Killed: %-3u  Tampered: %-3s\n",
                        vn->special.mind.who.killed,
                        (vn->special.mind.tampered ? "yes" : "no"));

                notify(playernum, governor, buf);
            } else {
                notify(playernum, governor, "Not a VN type ship.\n");
            }

            free(vn);
        } else {
            notify(playernum, governor, "Invalid ship number.\n");
        }
    } else if (argn == 3) {
        plno = atoi(args[1]);
        val = atoi(args[2]);

        if ((plno > MAXPLAYERS) || (plno < 1)) {
            notify(playernum, governor, "Invalid player number.\n");

            return;
        }

        getsdata(&Sdata);
        Sdata.VN_hitlist[plno - 1] = val;
        putsdata(&Sdata);
        notify(playernum, governor, "Set.\n");
    } else {
        notify(playernum, governor, "Incorrect usage, see 'help'\n");
    }

    return;
}

void vn_mad(void)
{
    int i;

    VN_brain.Total_mad = 0;
    VN_brain.Most_mad = 0;

    for (i = 1; i <= Num_races; ++i) {
        /* Add VN program */
        VN_brain.Total_mad += Sdata.VN_hitlist[i - 1];

        /* Find out who they're most mad at */
        if (VN_brain.Total_mad > 0) {
            /* We're mad at someone, is it this race? */
            if (VM_brain.Most_mad > 0) {
                if (Sdata.VN_hitlist[VN_brain.Most_mad - 1] <= Sdata.VN_hitlist[i - 1]) {
                    VN_brain.Most_mad = i;
                }
            } else if (Sdata.VN_hitlist[i - 1] > 0) {
                VN_brain.Most_mad = i;
            }
        }
    }
}

#endif /* USE_VN */
