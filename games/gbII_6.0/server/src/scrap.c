/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (C) 1989-90 by Robert P. Chansky, et al.
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
 * scrap.c -- Turn a ship to junk
 *
 * #ident  "@(#)scrap.c  1.10 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/scrap.c,v 1.4 2007/07/06 18:07:58 gbp Exp $
 */
#include "scrap.h"

#include <stdlib.h>

#include "buffers.h"
#include "doship.h"
#include "files_shl.h"
#include "GB_server.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "shipdata.h"
#include "ships.h"
#include "shlmisc.h"
#include "vars.h"

#include "build.h"
#include "fire.h"
#include "land.h"
#include "load.h"

void scrap(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    planettype *planet;
    sectortype *sect;
    shiptype *s;
    shiptype *s2;
    int shipno;
    int nextshipno;
    int scrapval = 0;
    int destval = 0;
    int crewval = 0;
    int max_crew = 0;
    int xtalval = 0;
    int troopval = 0;
    int max_resource = 0;
    int max_mil = 0;
    int max_fuel = 0;
    int max_destruct = 0;
    int cost = 0;
    double fuelval = 0.0;
    racetype *race;

    if (argn < 2) {
        notify(playernum, governor, "Scrap what?\n");

        return;
    }

    nextshipno = start_shiplist(playernum, governor, args[1]);
    race = races[playernum - 1];

    if (race->Guest) {
        notify(playernum, governor, "Guest races cannot scrap ships\n");

        return;
    }

    shipno = do_shiplist(&s, &nextshipno);

    while (shipno) {
        if (in_list(playernum, args[1], s, &nextshipno)) {
#ifdef USE_VN
            if ((s->type == OTYPE_VN) || (s->type == OTYPE_BERS)) {
                notify(playernum, governor, "VNs will not scrap themselves.\n");
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }
#endif

            if (s->max_crew && !s->popn) {
                notify(playernum,
                       governor,
                       "Can't scrap that ship - no crew.\n");

                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if (s->whatorbits == LEVEL_UNIV) {
                /*
                 * Used to not allow scrapping at the UNIV level for
                 * anything. However, I'm going to permit pods. This is so pod
                 * races can clean up their messes. I'm not going to charge APs
                 * at the UNIV scope either. -mfw
                 */
                if (s->type) {
                    apcount = 0;
                } else {
                    notify(playernum,
                           governor,
                           "Can't scrap at the ship's scope.\n");

                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }
            } else if (!enufAP(playernum, governor, Stars[s->storbits]->AP[playernum - 1], apcount)) {
                notify(playernum, governor, "Not enough APs to scrap.\n");
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            /* HUTm (kse) wc's can't be scrapped inside of ship anymore */
            if (inship(s) && (s->type == OTYPE_TOXIC)) {
                sprintf(buf,
                        "Can't scrap waste canisters inside of other ship.\n");

                notify(playernum, governor, buf);
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            /* Ships that have other ships in the hangar can't scrap *mfw */
            if (s->ships) {
                sprintf(buf,
                        "There are other ships in the hangar; scrap those first.\n");

                notify(playernum, governor, buf);
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            }

            if ((s->whatorbits == LEVEL_PLAN) && (s->type == OTYPE_TOXIC)) {
                sprintf(buf,
                        "WARNING: This releases %d toxin points back into the atmosphere!!!\n",
                        s->special.waste.toxic);

                notify(playernum, governor, buf);
            }

            if (!s->docked) {
                sprintf(buf,
                        "%s is not landed or docked.\nNo resources can be reclaimed.\n",
                        Ship(s));

                notify(playernum, governor, buf);
            }

            if (s->whatorbits == LEVEL_PLAN) {
                /* wc's release poison */
                getplanet(&planet, (int)s->storbits, (int)s->pnumorbits);

                if (landed(s)) {
                    if (!getsector(&sect, planet, (int)s->land_x, (int)s->land_y)) {
                        notify(playernum,
                               governor,
                               "Error in sector database, notify deity.\n");

                        free(s);

                        return;
                    }
                }
            }

            if (docked(s) || inship(s)) {
                if (!getship(&s2, (int)s->destshipno)) {
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if ((!s2->docked || (s2->destshipno != s->number))
                    && (!(s->whatorbits == LEVEL_SHIP))) {
                    sprintf(buf, "Warning, other ship not docked...\n");
                    notify(playernum, governor, buf);
                    free(s);
                    free(s2);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }
            }

            if (s->type == OTYPE_FACTORY) {
                cost = (2 * s->build_cost * s->on) + Shipdata[s->type][ABIL_COST];
            } else {
                cost = s->build_cost;
            }

            scrapval = (cost / 2) + s->resource;

            if (s->docked) {
                sprintf(buf, "%s: original cost: %ld\n", Ship(s),
                        s->type == OTYPE_FACTORY ? 2 * s->build_cost * s->on : Shipdata[s->type][ABIL_COST]);
                notify(playernum, governor, buf);

                sprintf(buf,
                        "         scrap value%s: %d rp's.\n",
                        s->resource ? "(with stockpile) " : "",
                        scrapval);

                notify(playernum, governor, buf);

                /* New code by Kharush. check for STYPE_DHUTTLE added. */
                /* I've removed it, Dhuttle was silly -mfw */

                if (s2->type == OTYPE_FACTORY) {
                    max_resource = Shipdata[s2->type][ABIL_CARGO];
                    max_fuel = Shipdata[s2->type][ABIL_FUELCAP];
                    max_destruct = Shipdata[s2->type][ABIL_DESTCAP];
                } else {
                    max_resource = s2->max_resource;
                    max_fuel = s2->max_fuel;
                    max_destruct = s2->max_destruct;
                }

                if ((s->whatdest == LEVEL_SHIP)
                    && ((s2->resource + scrapval) > max_resource)
                    && (s2->type != STYPE_SHUTTLE)) {
                    scrapval = max_resource - s2->resource;
                    sprintf(buf,
                            "(There is only room for %d resources.)\n",
                            scrapval);

                    notify(playernum, governor, buf);
                }

                if (s->fuel) {
                    sprintf(buf, "Fuel recover: %.0f.\n", s->fuel);
                    notify(playernum, governor, buf);
                    fuelval = s->fuel;

                    if ((s->whatdest == LEVEL_SHIP)
                        && ((s2->fuel + fuelval) > max_fuel)) {
                        fuelval = max_fuel - s2->fuel;
                        sprintf(buf,
                                "(There is only room for %.2f fuel.)\n",
                                fuelval);

                        notify(playernum, governor, buf);
                    }
                } else {
                    fuelval = 0.0;
                }

                if (s->destruct) {
                    sprintf(buf, "Armament recovery: %d.\n", s->destruct);
                    notify(playernum, governor, buf);
                    destval = s->destruct;

                    if ((s->whatdest == LEVEL_SHIP)
                        && ((s2->destruct + destval) > max_destruct)) {
                        destval = max_destruct - s2->destruct;
                        sprintf(buf,
                                "(There is only room for %d destruct.)\n",
                                destval);

                        notify(playernum, governor, buf);
                    }
                } else {
                    destval = 0;
                }

                if (s->popn + s->troops) {
                    if ((s->whatdest == LEVEL_PLAN)
                        && (sect->owner > 0)
                        && (sect->owner != playernum)) {
                        sprintf(buf,
                                "You don't own this sector; no crew can be recovered.\n");

                        notify(playernum, governor, buf);
                    } else {
                        troopval = s->troops;

                        if (s2->type == OTYPE_FACTORY) {
                            max_mil = Shipdata[s2->type][ABIL_MAXCREW] - s->popn;
                        } else {
                            max_mil = s2->max_crew - s2->popn;
                        }

                        if ((s->whatdest == LEVEL_SHIP)
                            && ((s2->troops + troopval) > max_mil)) {
                            troopval = max_mil - s2->troops;
                            sprintf(buf,
                                    "(There is only room for %d troops.)\n",
                                    troopval);

                            notify(playernum, governor, buf);
                        }

                        crewval = s->popn;

                        if (s2->type == OTYPE_FACTORY) {
                            max_crew = Shipdata[s2->type][ABIL_MAXCREW] - s2->troops;
                        } else {
                            max_crew = s2->max_crew - s2->troops;
                        }

                        if ((s->whatdest == LEVEL_SHIP)
                            && ((s2->popn + crewval) > max_crew)) {
                            crewval = max_crew - s2->popn;
                            sprintf(buf,
                                    "(There is only room for %d crew.)\n",
                                    crewval);

                            notify(playernum, governor, buf);
                        }

                        sprintf(buf,
                                "Population/Troops recover: %d/%d.\n",
                                crewval,
                                troopval);

                        notify(playernum, governor, buf);
                    }
                } else {
                    crewval = 0;
                    troopval = 0;
                }

                if (s->crystals + s->mounted) {
                    if ((s->whatdest == LEVEL_PLAN)
                        && (sect->owner > 0)
                        && (sect->owner != playernum)) {
                        sprintf(buf,
                                "You don't own this sector; no crystals can be recovered.\n");

                        notify(playernum, governor, buf);
                    } else {
                        xtalval = s->crystals + s->mounted;

                        if ((s->whatdest == LEVEL_SHIP)
                            && ((s2->crystals + xtalval) > 127)) {
                            xtalval = 127 - s2->crystals;
                            sprintf(buf,
                                    "(There is only room for %d crystals.)\n",
                                    xtalval);

                            notify(playernum, governor, buf);
                        }

                        sprintf(buf, "Crystal recover: %d.\n", xtalval);
                        notify(playernum, governor, buf);
                    }
                } else {
                    xtalval = 0;
                }
            }

            /* More adjustments needed here for hangar. Maarten */
            if (s->whatorbits == LEVEL_SHIP) {
                s2->hanger -= (unsigned short)s->size;
            }

            if (s->whatorbits == LEVEL_UNIV) {
                deductAPs(playernum, governor, apcount, 0, 1);
            } else {
                deductAPs(playernum, governor, apcount, (int)s->storbits, 0);
            }

            if (docked(s) || inship(s)) {
                s2->crystals += xtalval;
                rcv_fuel(s2, (double)fuelval);
                rcv_destruct(s2, destval);
                rcv_resource(s2, scrapval);
                rcv_troops(s2, troopval, race->mass);
                rcv_popn(s2, crewval, race->mass);

                /*
                 * Check for docking status in case scrapped ship is
                 * landed. Maarten
                 */
                if (s->whatorbits != LEVEL_SHIP) {
                    s2->docked = 0; /* Undock the surviving ship */
                    s2->whatdest = LEVEL_UNIV;
                    s2->destshipno = 0;
                }

                putship(s2);
                free(s2);
            }

            if (s->whatorbits == LEVEL_PLAN) {
                free(planet); /* this has already been allocated */
                getplanet(&planet, (int)s->storbits, (int)s->pnumorbits);

                if (landed(s) || inship(s)) {
                    /*
                     * If colonizing the sector, set sector owner and give a
                     * message it's also nice to check if there is anyone to
                     * colonize
                     */
                    if ((sect->owner == 0)
                        && ((troopval > 0) || (crewval > 0))) {
                        sect->owner = playernum;
                        ++planet->info[playernum - 1].numsectsowned;
                        sprintf(buf,
                                "Sector %d,%d Colonized.\n",
                                s->land_x,
                                s->land_y);

                        notify(playernum, governor, buf);
                    }

                    /* Increase sector's crew and troop count */
                    sect->troops += troopval;
                    sect->popn += crewval;

                    /*
                     * Increase planet's crew, troop, res, tec. count for this
                     * player
                     */
                    planet->info[playernum - 1].popn += crewval;
                    planet->info[playernum - 1].troops += troopval;

                    /*
                     * New code by Kharush. Scrapping does not anymore overflow
                     * stockpiles.
                     */
                    if ((planet->info[playernum - 1].resource + scrapval) <= USHRT_MAX) {
                        planet->info[playernum - 1].resource += scrapval;
                    } else {
                        planet->info[playernum - 1].resource = USHRT_MAX;
                        sprintf(buf,
                                "Planet has room for only %d resources.\n",
                                USHRT_MAX - planet->info[playernum - 1].resource);

                        notify(playernum, governor, buf);
                    }

                    if ((planet->info[playernum - 1].fuel + fuelval) <= USHRT_MAX) {
                        planet->info[playernum - 1].fuel += fuelval;
                    } else {
                        planet->info[playernum - 1].fuel = USHRT_MAX;
                        sprintf(buf,
                                "Planet has room for only %d fuel.\n",
                                USHRT_MAX - planet->info[playernum - 1].fuel);

                        notify(playernum, governor, buf);
                    }

                    if ((planet->info[playernum - 1].destruct + destval) <= USHRT_MAX) {
                        planet->info[playernum - 1].destruct += destval;
                    } else {
                        planet->info[playernum - 1].destruct = USHRT_MAX;
                        sprintf(buf,
                                "Planet has room for only %d destruct.\n",
                                USHRT_MAX - planet->info[playernum - 1].destruct);

                        notify(playernum, governor, buf);
                    }

                    /*
                     * Old code
                     *
                     * planet->info[playernum - 1].resource += scrapval;
                     * planet->info[playernum - 1].destruct += destval;
                     * planet->info[playernum - 1].fuel += (int)fuelval;
                     */

                    planet->popn += crewval;
                    planet->info[playernum - 1].crystals += (int)xtalval;
                    putsector(sect, planet, (int)s->land_x, (int)s->land_y);
                    free(sect);
                }

                putplanet(planet, (int)s->storbits, (int)s->pnumorbits);
            }

            kill_ship(playernum, s);
            putship(s);
            free(s);

            if (landed(s)) {
                sprintf(buf, "\nScrapped.\n");
            } else {
                sprintf(buf, "\nDestroyed.\n");
            }

            notify(playernum, governor, buf);
        } else {
            free(s);
        }

        shipno = do_shiplist(&s, &nextshipno);
    }
}
