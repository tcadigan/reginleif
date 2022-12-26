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
 * capture.c -- Capture a ship on the ground
 *
 * #Ident  "@(#)capture.c       1.10 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/capture.c,v 1.6 2007/07/06 18:09:34 gbp Exp $
 */
#include "capture.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "doship.h"
#include "files_shl.h"
#include "GB_server.h"
#include "getplace.h"
#include "lists.h"
#include "misc.h"
#include "power.h"
#include "races.h"
#include "rand.h"
#include "ranks.h"
#include "shlmisc.h"
#include "ships.h"
#include "vars.h"

#include "fire.h"
#include "tele.h"

extern int Defensedata[];
void capture_stuff(shiptype *);

void capture(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    shiptype *ship;
    shiptype s;
    planettype *p;
    sectortype *sect;
    int boarders = -1;
    int olddpopn;
    int oldowner;
    int oldgov;
    int shipdam = 0;
    int booby = 0;
    int shipno;
    int nextshipno;
    int x = -1;
    int y = -1;
    int what;
    int olddtroops;
    int i;
    int snum;
    int pnum;
    int armor;
    int casualties = 0;
    int casualties1 = 0;
    int casualties2 = 0;
    int casualty_scale = 0;
    double astrength;
    double dstrength;
    racetype *race;
    racetype *alien;

    if (argn < 2) {
        notify(playernum, governor, "Capture what?\n");

        return;
    }

    snum = Dir[playernum - 1][governor].snum;
    pnum = Dir[playernum - 1][governor].pnum;
    nextshipno = start_shiplist(playernum, governor, args[1]);
    shipno = do_shiplist(&ship, &nextshipno);

    while (shipno) {
        if ((ship->owner != playernum)
            && in_list((int)ship->owner, args[1], ship, &nextshipno)) {
            if (!landed(ship)) {
                sprintf(buf,
                        "%s #%d is not landed on a planet.\n",
                        Shipnames[ship->type],
                        shipno);

                notify(playernum, governor, buf);
                free(ship);

                continue;
            }

            if ((ship->storbits != snum) || (ship->pnumorbits != pnum)) {
                notify(playernum,
                       governor,
                       "Change scope to the planet this ship is landed on.\n");

                free(ship);

                continue;
            }

#ifdef USE_VN
            if ((ship->type == OTYPE_VN) || (ship->type == OTYPE_BERS)) {
                notify(playernum,
                       governor,
                       "You can't capture Von Neumann machines.\n");

                /* free(ship); */
                /* return; */
            }
#endif

            if (!enufAP(playernum, governor, Stars[ship->storbits]->AP[playernum - 1], apcount)) {
                free(ship);

                continue;
            }

            x = ship->land_x;
            y = ship->land_y;

            getplanet(&p, (int)ship->storbits, (int)ship->pnumorbits);

            if (!getsector(&sect, p, x, y)) {
                notify(playernum,
                       governor,
                       "Error in sector database, notify deity.\n");

                return;
            }

            if (sect->owner != playernum) {
                sprintf(buf,
                        "You don't own the sector where the ship is landed [%d].\n",
                        sect->owner);

                notify(playernum, governor, buf);
                free(sect);
                free(p);
                free(ship);

                continue;
            }

            if (argn < 4) {
                what = CIV;
            } else if (match(args[3], "civilians")) {
                what = CIV;
            } else if (match(args[3], "military")) {
                what = MIL;
            } else {
                notify(playernum, governor, "Capture with what?\n");
                free(sect);
                free(p);
                free(ship);

                continue;
            }

            if (argn < 3) {
                if (what == CIV) {
                    boarders = sect->popn;
                } else {
                    boarders = sect->troops;
                }
            } else {
                boarders = atoi(args[2]);
            }

            if ((boarders > sect->popn) && (what == CIV)) {
                boarders = sect->popn;
            } else if ((boarders > sect->troops) && (what == MIL)) {
                boarders = sect->troops;
            }

            if (boarders <= 0) {
                sprintf(buf, "Illegal number of boarders %d.\n", boarders);
                notify(playernum, governor, buf);
                free(ship);
                free(sect);
                free(p);

                continue;
            }

            race = races[playernum - 1];
            alien = races[ship->owner - 1];

            if (isset(race->allied, (int)ship->owner)) {
                sprintf(buf,
                        "Boarding the ship of your ally, %s\n",
                        alien->name);

                notify(playernum, governor, buf);
            }

            olddpopn = ship->popn;
            olddtroops = ship->troops;
            oldowner = ship->owner;
            oldgov = ship->governor;
            memcpy(&s, ship, sizeof(shiptype));
            shipdam = 0;
            casualties = 0;
            casualties1 = 0;
            casualties2 = 0;

            if (what == CIV) {
                sect->popn -= boarders;
            } else if (what == MIL) {
                sect->troops -= boarders;
            }

#ifdef USE_VN
            if ((olddpopn + olddtroops)
                || (ship->type == OTYPE_VN)
                || (ship->type == OTYPE_BERS)) {
                /* CWL Allow vns to be attacked on the ground */
                float vn_ghosts;

                if (ship->type == OTYPE_VN) {
                    vn_ghosts = 1000.0;
                } else if (ship->type == OTYPE_BERS) {
                    vn_ghosts = 10000.0;
                } else {
                    vn_ghosts = 0.0;
                }
                /* End CWL */

                if (what == MIL) {
                    astrength = (double)race->fighters * 10.0 * 0.01 * race->tech * (race->likes[sect->condition] + 0.01) * ((double)Defensedata[sect->condition] + 1.0) * morale_factor((double)(race->morale - alien->morale));
                } else {
                    astrength = 1.0 * 0.01 * race->tech * (race->likes[sect->condition] + 0.01) * ((double)Defensedata[sect->condition] + 1.0) * morale_factor((double)(race->morale - alien->morale));
                }

                if (ship->type == OTYPE_FACTORY) {
                    armor = Shipdata[ship->type][ABIL_ARMOR];
                } else {
                    armor = (ship->armor * (100 - ship->damage)) / 100;
                }

                dstrength = (vn_ghosts
                             + (double)ship->popn
                             + ((double)ship->troops
                                * 10.0
                                * (double)alien->fighters))
                    * 0.01
                    * alien->tech
                    * ((double)armor + 0.01)
                    * 0.01
                    * (100.0 - (double)ship->damage)
                    * morale_factor((double)(alien->morale - race->morale));

                sprintf(buf,
                        "Attack strength: %.2f     Defense strength: %.2f\n",
                        astrength,
                        dstrength);

                notify(playernum, governor, buf);
                casualty_scale = MIN(boarders, ship->popn + ship->troops);

                if (astrength > 0.0) {
                    casualties = int_rand(0, round_rand(((double)casualty_scale * (dstrength + 1.0)) / (astrength + 1.0)));
                }

                if (dstrength > 0.0) {
                    casualties1 = int_rand(0, round_rand(((double)casualty_scale * (astrength + 1.0)) / (dstrength + 1.0)));

                    casualties2 = int_rand(0, round_rand(((double)casualty_scale * (astrength + 1.0)) / (dstrength + 1.0)));

                    shipdam = int_rand(0, round_rand((25.0 * (astrength + 1.0)) / (dstrength + 1.0)));

                    ship->damage = MIN(100, ship->damage + shipdam);
                }

                casualties = MIN(boarders, casualties);
                boarders -= casualties;

                casualties1 = MIN(olddpopn, casualties);
                ship->popn -= casualties;
                ship->mass -= (casualties1 * alien->mass);

                casualties2 = MIN(olddtroops, casualties2);
                ship->troops -= casualties;
                ship->mass -= (casualties * alien->mass);

            } else if (ship->destruct) {
                /* Booby trapped robot ships */
                booby = int_rand(0, 10 * (int)ship->destruct);
                booby = MIN(100, booby);
                casualties2 = 0;
                casualties = casualties2;

                for (i = 0; i < boarders; ++i) {
                    if (int_rand(1, 100) < booby) {
                        ++casualties;
                    }
                }

                boarders -= casualties;
                shipdam += booby;
                ship->damage += booby;
            }
#else

            if (olddpopn + olddtroops) {
                if (what == MIL) {
                    astrength = (double)boarders * (double)race->fighters * 10.0 * 0.01 * race->tech * (race->likes[sect->condition] + 0.01) * ((double)Defensedata[sect->condition] + 1.0) * morale_factor((double)(race->morale - alien_morale));
                } else {
                    astrength = (double)boarders * 1.0 * 0.01 * race->tech * (race->likes[sect->condition] + 0.01) * ((double)Defensedata[sect->condition] + 1.0) * morale_factor((double)(race->morale - alien->morale));
                }

                if (ship->type == OTYPE_FACTORY) {
                    armor = Shipdata[ship->type][ABIL_ARMOR];
                } else {
                    armor = (ship->armor * (100 - ship->damage)) / 100;
                }

                dstrength = ((double)ship->popn
                             + ((double)ship->troops
                                * 10.0
                                * (double)alien->fighters))
                    * 0.01
                    * alien->tech
                    * ((double)armor + 0.01)
                    * 0.01
                    * (100.0 - (double)ship->damage)
                    * morale_factor((double)(alien->morale - race->morale));

                sprintf(buf,
                        "Attack strength: %.2f     Defense strength: %.2f\n",
                        astrength,
                        dstrength);

                notify(playernum, governor, buf);
                casualty_scale = MIN(boarders, ship->popn + ship->troops);

                if (astrength > 0.0) {
                    casualties = int_rand(0, round_rand(((double)casualty_scale * (dstrength + 1.0)) / (astrength + 1.0)));
                }

                if (dstrength > 0.0) {
                    casualties1 = int_rand(0, round_rand(((double)casualty_scale * (astrength + 1.0)) / (dstrength + 1.0)));

                    casualties2 = int_rand(0, round_rand(((double)casualty_scale * (astrength + 1.0)) / (dstrength + 1.0)));

                    shipdam = int_rand(0, round_rand((25.0 * (astrength + 1.0)) / (dstrength + 1.0)));

                    ship->damage = MIN(100, ship->damage + shipdam);
                }

                casualties = MIN(boarders, casualties);
                boarders -= casualties;

                casualties1 = MIN(olddpopn, casualties1);
                ship->popn -= casualties1;
                ship->mass -= (casualties1 * alien->mass);

                casualties2 = MIN(olddtroops, casualties2);
                ship->troops -= casualties2;
                ship->mass -= (casualties * alien->mass);
            } else if (ship->destruct) {
                /* Booby trapped robot ships */
                booby = int_rand(0, 10 * (int)ship->destruct);
                booby = MIN(100, booby);
                casualties2 = 0;
                casualties = casualties2;

                for (i = 0; i < boarders; ++i) {
                    if (int_rand(1, 100) < booby) {
                        ++casualties;
                    }
                }

                boarders -= casualties;
                shipdam += booby;
                ship->damage += booby;
            }
#endif

            shipdam = MIN(100, shipdam);

            if (ship->damage >= 100) {
                kill_ship(playernum, ship);
            }

#ifdef USE_VN
            if (!(ship->popn + ship->troops)
                && ship->alive
                && !ship->type
                && (ship->type != OTYPE_VN)
                && (ship->type != OTYPE_BERS)) {
                /* We got them */
                /* Remove the ship from any fleet -mfw */
                remove_sh_fleet(ship->owner, ship->governor, ship);
                ship->owner = playernum;
                ship->governor = governor;

                if (what == CIV) {
                    if (ship->type == OTYPE_FACTORY) {
                        ship->popn = MIN(boarders,
                                         Shipdata[ship->type][ABIL_MAXCREW]
                                         - ship->troops);
                    } else {
                        ship->popn = MIN(boarders,
                                         ship->max_crew - ship->troops);
                    }

                    sect->popn += (boarders - ship->popn);
                    ship->mass += (ship->popn * race->mass);
                } else if (what == MIL) {
                    if (ship->type == OTYPE_FACTORY) {
                        ship->troops = MIN(boarders,
                                           Shipdata[ship->type][ABIL_MAXCREW]
                                           - ship->popn);
                    } else {
                        ship->troops = MIN(boarders,
                                           ship->max_crew - ship->popn);
                    }

                    sect->troops += (boarders - ship->troops);
                    ship->mass += (ship->troops * race->mass);
                }

                /* Unoccupied ships and factories don't count */
                if ((olddpopn + olddtroops) && (ship->type != OTYPE_FACTORY)) {
                    adjust_morale(race, alien, (int)ship->build_cost);
                }
            } else {
                /* Retreat */
                if (what == CIV) {
                    sect->popn += boarders;
                } else if (what == MIL) {
                    sect->troops += boarders;
                }
            }

#else
            if (!(ship->popn + ship->troops) && ship->alive && !ship->type) {
                /* We got them. */
                /* Remove the ship from any fleet -mfw */
                remove_sh_fleet(ship->owner, ship->governor, ship);
                ship->owner = playernum;
                ship->governor = governor;

                if (what == CIV) {
                    if (ship->type == OTYPE_FACTORY) {
                        ship->popn = MIN(boarders,
                                         Shipdata[ship->type][ABIL_MAXCREW]
                                         - ship->troops);
                    } else {
                        ship->popn = MIN(boarders,
                                         ship->max_crew - ship->troops);
                    }

                    sect->popn += (boarders - ship->popn);
                    ship->mass += (ship->popn - race->mass);
                } else if (what == MIL) {
                    if (ship->type == OTYPE_FACTORY) {
                        ship->troops = MIN(boarders,
                                           Shipdata[ship->type][ABIL_MAXCREW]
                                           - ship->popn);
                    } else {
                        ship->troops = MIN(boarders,
                                           ship->max_crew - ship->popn);
                    }
                    sect->troops += (boarders - ship->troops);
                    ship->mass += (ship->troops * race->mass);
                }

                /* Unoccupied ships and factories don't count */
                if ((olddpopn + olddtroops) && (ship->type != OTYPE_FACTORY)) {
                    adjust_morale(race, alien, (int)ship->build_cost);
                }
            } else {
                /* Retreat */
                if (what == CIV) {
                    sect->popn += boarders;
                } else if (what == MIL) {
                    sect->troops += boarders;
                }
            }
#endif

            if (!(sect->popn + sect->troops)) {
                sect->owner = 0;
            }

            sprintf(buf,
                    "BULLETIN from %s/%s!!\n",
                    Stars[ship->storbits]->name,
                    Stars[ship->storbits]->pnames[ship->pnumorbits]);

            strcpy(telegram_buf, buf);

            sprintf(buf,
                    "You are being attacked by %s Player #%d (%s)!!!\n",
                    isset(alien->allied, playernum) ? "your ally"
                    : isset(alien->atwar, playernum) ? "your enemy"
                    : "neutral",
                    playernum,
                    race->name);

            strcat(telegram_buf, buf);

            sprintf(buf,
                    "%s at sector %d,%d [owner %d]!\n",
                    Ship(ship),
                    x,
                    y,
                    sect->owner);

            strcat(telegram_buf, buf);

            if (booby) {
                sprintf(buf,
                        "Booby trap triggered causing %d%% damage.\n",
                        booby);

                strcat(telegram_buf, buf);
                notify(playernum, governor, buf);
            }

            if (shipdam) {
                sprintf(buf,
                        "This attack caused %d%% damage, now at %d%% total damage.\n",
                        shipdam,
                        ship->damage);

                strcat(telegram_buf, buf);

                sprintf(buf,
                        "Inflicted %d%% damage, not at %d%% total damage.\n",
                        shipdam,
                        ship->damage);

                notify(playernum, governor, buf);

                sprintf(short_buf,
                        "%s: %s [%d] attacked %s\n",
                        Dispshiploc(ship),
                        race->name,
                        playernum,
                        Ship(&s));
            }

            if (!ship->alive) {
                sprintf(buf, "              YOUR SHIP WAS DESTROYED!!!\n");
                strcat(telegram_buf, buf);
                sprintf(buf, "              Their ship DESTROYED!!!\n");
                notify(playernum, governor, buf);

                sprintf(short_buf,
                        "%s: %s [%d] DESTROYED %s\n",
                        Dispshiploc(ship),
                        race->name,
                        playernum,
                        Ship(&s));
            }

            if (ship->owner == playernum) {
                sprintf(buf, "%s CAPTURED!\n", Ship(&s));
                strcat(telegram_buf, buf);
                sprintf(buf, "VICTORY! The ship is yours!\n");
                notify(playernum, governor, buf);

                if (what == CIV) {
                    sprintf(buf,
                            "%d boarders move in.\n",
                            MIN(boarders, ship->popn));
                } else if (what == MIL) {
                    sprintf(buf,
                            "%d troops move in.\n",
                            MIN(boarders, ship->troops));
                }

                notify(playernum, governor, buf);
                capture_stuff(ship);

                sprintf(short_buf,
                        "%s: %s [%d] CAPTURED %s\n",
                        Dispshiploc(ship),
                        race->name,
                        playernum,
                        Ship(&s));
            } else if (ship->popn + ship->troops) {
                sprintf(buf, "You fought them off!\n");
                strcat(telegram_buf, buf);
                sprintf(buf, "The boarding was repulsed; try again.\n");
                notify(playernum, governor, buf);

                sprintf(short_buf,
                        "%s: %s [%d] assaults %s\n",
                        Dispshiploc(ship),
                        race->name,
                        playernum,
                        Ship(&s));
            }

            if (ship->alive) {
                if (!(sect->popn + sect->troops + boarders)) {
                    sprintf(buf, "You killed all the aliens in this sector!\n");
                    strcat(telegram_buf, buf);
                    p->info[playernum - 1].mob_points -= sect->mobilization;
                }

                if (!boarders) {
                    sprintf(buf,
                            "Oh no! They killed your party to the last man!\n");

                    notify(playernum, governor, buf);
                } else if (ship->type && ((ship->popn + ship->troops) == 0)) {
                    sprintf(buf, "Boarders are unable to control the pod.\n");
                    notify(playernum, governor, buf);
                }
            } else {
                sprintf(buf, "You ship was weakened too much!\n");
                strcat(telegram_buf, buf);
                sprintf(buf, "The assault weakened their ship too much!\n");
                notify(playernum, governor, buf);
            }

            if (casualties || casualties1 || casualties2) {
                if (what == CIV) {
                    sprintf(buf,
                            "Casualties: Yours: %d civ/%d mil, Theirs: %d civ\n",
                            casualties1,
                            casualties2,
                            casualties);
                } else {
                    sprintf(buf,
                            "Casualties: Yours: %d civ/%d mil, Theirs: %d mil\n",
                            casualties1,
                            casualties2,
                            casualties);
                }

                strcat(telegram_buf, buf);

                if (what == CIV) {
                    sprintf(buf,
                            "Casualties: Yours: %d civ, Theirs: %d civ/%d mil\n",
                            casualties,
                            casualties1,
                            casualties2);
                } else {
                    sprintf(buf,
                            "Casualties: Yours: %d mil, Theirs: %d civ/%d mil\n",
                            casualties,
                            casualties1,
                            casualties2);
                }

                notify(playernum, governor, buf);
            }

            warn(oldowner, oldgov, telegram_buf);

            if ((ship->owner != oldowner) || !ship->alive) {
                post(short_buf, COMBAT);
            }

            notify_star(playernum,
                         governor,
                         oldowner,
                         (int)ship->storbits,
                         short_buf);

            putship(ship);
            putsector(sect, p, x, y);
            putplanet(p, snum, pnum);
            putrace(race);
            putrace(alien);
            deductAPs(playernum, governor, apcount, (int)ship->storbits, 0);
            free(p);
            free(ship);
        } else {
            free(ship);
        }
    }
}

void capture_stuff(shiptype *ship)
{
    int sh;
    shiptype *s;

    sh = ship->ships;

    while (sh) {
        getship(&s, sh);
        /* Recursive call. Make sure he gets all of the ships landed on it. */
        capture_stuff(s);
        s->owner = ship->owner;
        s->governor = ship->governor;
        putship(s);
        sprintf(buf, "%s CAPTURED!\n", Ship(s));
        notify((int)s->owner, (int)s->governor, buf);
        sh = nextship(s);
        free(s);
    }
}
