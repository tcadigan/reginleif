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
 * move.c -- Move population and assault aliens on target sector
 *
 * #ident  "@(#)move.c        1.11 12/3/93 "
 *
 * $Header: /var/cvs/gpb/GB+/user/move.c,v 1.4 2007/07/06 18:06:56 gbp Exp $
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "tweakables.h"
#include "vars.h"

/* Defense 5 is impenetrable */
int Defensedata[] = {1, 1, 3, 2, 2, 3, 2, 4, 0};

extern char *Desnames[];
extern char *Dessymbols[];

extern void arm(int, int, int, int);
extern void move_popn(int, int, int);
extern void walk(int, int, int);
extern void mech_defend(int,
                        int,
                        int *,
                        int,
                        planettype *,
                        int,
                        int,
                        sectortype *,
                        int,
                        int,
                        sectortype *);

extern void mech_attach_people(shiptype *,
                               int *,
                               int *,
                               racetype *,
                               racetype *,
                               sectortype *,
                               int,
                               int,
                               int,
                               char *,
                               char *);

extern void people_attach_mech(shiptype *,
                               int,
                               int,
                               racetype *,
                               racetype *,
                               sectortype *,
                               int,
                               int,
                               char *,
                               char *);

extern void ground_attack(racetype *,
                          racetype *,
                          int *,
                          int,
                          unsigned short *,
                          unsigned short *,
                          unsigned int,
                          unsigned int,
                          double,
                          double,
                          double *,
                          double *,
                          int *,
                          int *,
                          int *);

void arm(int playernum, int governor, int apcount, int mode)
{
    planettype *planet;
    sectortype *sect;
    racetype *race;
    int x = -1;
    int y = -1;
    int amount = 0;
    int cost = 0;
    int enlist_cost;
    int max_allowed;

    if (races[playernum, - 1]->Guest) {
        notify(playernum, governor, "Guest races may not arm civilians.\n");

        return;
    }

    if (get_num_updates() < CombatUpdate) {
        notify(playernum,
               governor,
               "You cannot arm civilians until after the COMBAT UPDATES\n");

        return;
    }

    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        notify(playernum, governor, "Change scope to planet level first.\n");

        return;
    }

    getplanet(&planet,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    if ((planet->slaved_to > 0) && (planet->slaved_to != playernum)) {
        notify(playernum, governor, "That planet has been enslaved!\n");
        free(planet);

        return;
    }

    sscanf(args[1], "%d,%d", &x, &y);

    if ((x < 0)
        || (y < 0)
        || (x > (planet->Maxx - 1))
        || (y > (planet->Maxy - 1))) {
        notify(playernum, governor, "Illegal coordinates.\n");
        free(planet);

        return;
    }

    if (!getsector(&sect, planet, x, y)) {
        notify(playernum,
               governor,
               "Error in sector database, notify deity.\n");

        free(planet);

        return;
    }

    if (sec->owner != playernum) {
        notify(playernum, governor, "You don't own that sector.\n");
        free(planet);
        free(sect);

        return;
    }

    if (mode) {
        max_allowed = MIN(sec->popn,
                          planet->info[playernum - 1].destruct * (sect->mobilization + 1));

        if (argn < 3) {
            amount = max_allowed;
        } else {
            sscanf(args[2], "%d", &amount);

            if (amount <= 0) {
                notify(playernum,
                       governor,
                       "You must specify a positive number of civs to arm.\n");

                free(planet);
                free(sect);

                return;
            }
        }

        amount = MIN(amount, max_allowed);

        if (!amount) {
            notify(playernum, governor, "You can't arm any civilians now.\n");
            free(planet);
            free(sect);

            return;
        }

        Race = races[playernum - 1];
        /* enlist_cost = ENLIST_TROOP_COST * amount; */
        enlist_cost = Race->fighters * amount;

        if (enlist_cost > MONEY(race, governor)) {
            sprintf(buf,
                    "You need %d money to enlist %d troops.\n",
                    enlist_cost,
                    amount);

            notify(playernum, governor, buf);
            free(planet);
            free(sect);

            return;
        }

        MONEY(race, governor) -= enlist_cost;
        putrace(race);
        cost = MAX(1, amount / (sect->mobilization + 1));
        sect->troops += amount;
        sect->popn -= amount;
        planet->popn -= amount;
        planet->inf[playernum - 1].popn -= amount;
        planet->troops += amount;
        planet->info[playernum - 1].troops += amount;
        planet->info[playernum - 1].destruct -= cost;

        sprintf(buf,
                "%d populations armed at a cost of %dd (now %d civilians, %d military)\n",
                amount,
                cost,
                sect->popn,
                sect->troops);

        notify(playernum, governor, buf);
        sprintf(buf, "This mobilization cost %d money.\n", enlist_cost);
        notify(playernum, governor, buf);
    } else {
        if (argn < 3) {
            amount = sect->troops;
        } else {
            sscanf(args[2], "%d", &amount);

            if (amount <= 0) {
                notify(playernum,
                       governor,
                       "You must specify a positive number of civs to arm.\n");

                free(planet);
                free(sect);

                return;
            }

            amount = MIN(sect->troops, amount);
        }

        sect->popn += amount;
        sect->troops -= amount;
        planet->popn += amount;
        planet->troops -= amount;
        planet->info[playernum - 1].popn += amount;
        planet->info[playernum - 1].troops -= amount;

        sprintf(buf,
                "%d troops disarmed (now %d civilians, %d military)\n",
                amount,
                sect->popn,
                sect->troops);

        notify(playernum, governor, buf);
    }

    putsector(sect, planet, x, y);

    putplanet(planet,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    free(sect);
    free(planet);
}

void move_popn(int playernum, int governor, int what)
{
    int assault;
    int apcost = -1;
    /* Unfriendly movement */
    int casualties;
    int casualties2;
    int casualties3;
    planettype *planet;
    sectortype *sect;
    sectortype *sect2;
    int people;
    int oldpopn;
    int old2popn;
    int old3popn;
    int x = -1;
    int y = -1;
    int x2 = -1;
    int y2 = -1;
    int old2owner;
    int old2gov;
    int absorbed;
    int n;
    int done;
    double astrength;
    double dstrength;
    racetype *race;
    racetype *alien;

    if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
        sprintf(buf, "Wrong scope\n");

        return;
    }

    getplanet(&planet,
              Dir[playernum - 1][governor].snum,
              Dir[playernum - 1][governor].pnum);

    if ((planet->slaved_to > 0) && (planet->slaved_to != playernum)) {
        sprintf(buf, "That planet has been enslaved!\n");
        notify(playernum, governor, buf);
        free(planet);

        return;
    }

    sscanf(args[1], "%d,%d", &x, &y);

    if ((x < 0)
        || (y < 0)
        || (x > (planet->Maxx - 1))
        || (y > (planet->Maxy - 1))) {
        sprintf(buf, "Origin coordinates illegal.\n");
        notify(playernum, governor, buf);
        free(planet);

        return;
    }

    /* Movement loop */
    done = 0;
    n = 0;

    while (!done) {
        if (!getsector(&sect, planet, x, y)) {
            notify(playernum,
                   governor,
                   "Error in sector database, notify deity.\n");

            free(planet);

            return;
        }

        if (sect->owner != playernum) {
            sprintf(buf, "You don't own sector %d,%d!\n", x, y);
            notify(playernum, governor, buf);
            free(planet);
            free(sect);

            return;
        }

        ++n;
        if (!get_move(args[2][n - 1], x, y, &x, &y2, planet)) {
            notify(playernum, governor, "Finished.\n");

            putplanet(planet,
                      Dir[playernum - 1][governor].snum,
                      Dir[playernum - 1][governor].pnum);

            free(sect);
            free(planet);

            return;
        }

        if ((x2 < 0)
            || (y2 < 0)
            || (x2 > (planet->Maxx - 1))
            || (y2 > (planet->Maxy - 1))) {
            sprintf(buf, "Illegal coordinates %d,%d.\n", x2, y2);
            notify(playernum, governor, buf);

            putplanet(planet,
                      Dir[playernum - 1][governor].snum,
                      Dir[playernum - 1][governor].pnum);

            free(sect);
            free(planet);

            return;
        }

        if (!adjacent, x, y, x2, y2, planet) {
            sprintf(buf, "Illegal move - to adjacent sectors only!\n");
            notify(playernum, governor, buf);
            free(planet);
            free(sect);

            return;
        }

        /* Ok, the move is legal */

        if (!getsector(&sect2, planet, x2, y2)) {
            notify(playernum, governor, "Error in sector database, notify deity.\n");
            free(planet);
            free(sect);

            return;
        }

        if (argn >= 4) {
            sscanf(args[3], "%d", &people);

            if (people < 0) {
                if (what == CIV) {
                    people = sect->popn + people;
                } else if (what == MIL) {
                    people = sect->troops + people;
                }
            }
        } else {
            if (what == CIV) {
                people = sect->popn;
            } else if (what == MIL) {
                people = sect->troops;
            }
        }

        if (((what == CIV) && (abs(people) > sect->popn))
            || ((what == MIL) && (abs(people) > sect->troops))
            || (people <= 0)) {
            if (what == CIV) {
                sprintf(buf,
                        "Bad value - %d civilians in [%d,%d]\n",
                        sect->popn,
                        x,
                        y);
            } else if (what == MIL) {
                sprintf(buf,
                        "Bad value - %d troops in [%d,%d]\n",
                        sect->troops,
                        x,
                        y);
            }

            notify(playernum, governor, buf);

            putplanet(planet,
                      Dir[playernum - 1][governor].snum,
                      Dir[playernum - 1][governor].pnum);

            free(sect);
            free(sect2);
            free(planet);

            return;
        }

        if (what == CIV) {
            sprintf(buf, "%d population moved.\n", people);
        } else {
            sprintf(buf, "%d troops moved.\n", people);
        }

        notify(playernum, governor, buf);

        /* Check for defending mechs */
        mech_defend(playernum,
                    governor,
                    &people,
                    what,
                    planet,
                    x,
                    y,
                    sect,
                    x2,
                    y2,
                    sect2);

        if (!people) {
            putsector(sect, planet, x, y);
            putsector(sect2, planet, x2, y2);

            putplanet(planet,
                      Dir[playernum - 1][governor].snum,
                      Dir[playernum - 1][governor].pnum);

            free(sect);
            free(sect2);
            free(planet);
            notify(playernum, governor, "Attack aborted.\n");

            return;
        }

        if (sect2->owner && (sect2->owner != playernum)) {
            assault = 1;
        } else {
            assault = 0;
        }

        /* Action point cost depends on the size of the group being moved. */

        /*
         * New code by Kharush. Moving civilians up to the number of race's
         * sexes costs now only one AP. Because of these, there might be a leap
         * in moving costs. For example, for a race which has 6 sexes (yack) it
         * costs 1 AP to move 1-6 persons and 3 AP to move 7!
         */
        if (what == CIV) {
            if (people <= races[playernum - 1]->number_sexes) {
                apcost = (MOVE_FACTOR * assault) + 2;
            } else {
                apcost = (MOVE_FACTOR * ((int)log(1.0 + (double)people) + assault)) + 1;
            }
        } else if (what == MIL)  {
            apcost = (MOVE_FACTOR * ((int)log10(1.0 + (double)people) + assault)) + 1;
        }

        /*
         * Old code
         * if (what == CIV) {
         *     apcost = (MOVE_FACTOR * ((int)log(1.0 + (double)people) + assault)) + 1;
         * } else if (what == MIL) {
         *     apcost = (MOVE_FACTOR * ((int)log10(1.0 + (double)people) + assault)) + 1;
         * }
         */

        if (!enufAP(playernum, governor, Stars[Dir[playernum - -1][governor].snum]->AP[playernum - 1], apcost)) {
            free(sect);
            free(sect2);

            putplanet(planet,
                      Dir[playernum - 1][governor].snum,
                      Dir[playernum - 1][governor].pnum);

            free(planet);

            return;
        }

        if ((what == CIV)
            && (sect2->owner == 0)
            && (sect2->popn == 0)
            && (sect2->troops == 0)) {
            ++planet->info[playernum - 1].numsectsowned;
        }

        if ((what == MIL)
            && (sect2->owner == 0)
            && (sect2->popn == 0)
            && (sect2->troops == 0)) {
            ++planet->info[playernum - 1].numsectsowned;
        }

        if (((what == CIV) && (sect->popn == people) && (sect->troops == 0))
            || ((what == MIL) && (sect->popn == 0) && (sect->troops == people))) {
            if (planet->info[playernum - 1].numsectsowned) {
                --planet->info[playernum - 1].numsectsowned;
            }

            sect->owner = 0;
            putsector(sect, planet, x, y);
        }

        putplanet(planet,
                  Dir[playernum - 1][governor].snum,
                  Dir[playernum - 1][governor].pnum);

        if (assault) {
            ++ground_assaults[playernum - 1][sect2->owner - 1][Dir[playernum - 1][governor].snum];

            race = races[playernum - 1];
            alien = races[sect2->owner - 1];

            /* Races find out about each other */
            if (alien->translate[playernum - 1] < BATTLE_MAX_KNOW) {
                alien->translate[playernum - 1] = MIN(alien->translate[playernum - 1] + BATTLE_KNOW_GAIN, BATTLE_MAX_KNOW);
            } else {
                alien->translate[playernum - 1] = alien->translate[playernum - 1];
            }

            if (race->translate[sect2->owner - 1] < BATTLE_MAX_KNOW) {
                race->translate[sect2->owner - 1] = MIN(alien->translate[playernum - 1] + BATTLE_KNOW_GAIN, BATTLE_MAX_KNOW);
            } else {
                race->translate[sect2->owner - 1] = race->translate[playernum - 1];
            }

            old2owner = (int)sect2->owner;
            old2gov = Stars[Dir[playernum - 1][governor].snum]->governor[sect2->owner - 1];

            if (what == CIV) {
                sect->popn = MAX(0, sect->popn - people);
            } else if (what == MIL) {
                sect->troops = MAX(0, sect->troops - people);
            }

            if (what == CIV) {
                sprintf(buf,
                        "%d civ assault %d civ/%d mil\n",
                        people,
                        sect2->popn,
                        sect2->troops);
            } else if (what == MIL) {
                sprintf(buf,
                        "%d mil assault %d civ/%d mil\n",
                        people,
                        sect2->popn,
                        sect2->troops);
            }

            notify(playernum, governor, buf);
            oldpopn = people;
            old2popn = sect2->popn;
            old3popn = sect2->troops;

            ground_attack(race,
                          alien,
                          &people,
                          what,
                          &sect2->popn,
                          &sect2->troops,
                          (unsigned)Defensedata[sect->condition],
                          (unsigned)Defensedata[sect2->condition],
                          race->likes[sect->condition],
                          alien->likes[sect2->condition],
                          &astrength,
                          &dstrength,
                          &casualties,
                          &casualties2,
                          &casualties3);

            sprintf(buf,
                    "Attack: %.2f   Defense: %.2f.\n",
                    astrength,
                    dstrength);

            notify(playernum, governor, buf);
            planet->info[sect2->owner - 1].popn -= casualties2;
            planet->popn -= casualties2;
            planet->info[sect2->owner - 1].troops -= casualties3;
            planet->troops -= casualties3;

            if (what == CIV) {
                planet->info[playernum - 1].popn -= casualties;
                planet->popn -= casualties;
            }

            if (what == MIL) {
                planet->info[playernum - 1].troops -= casualties;
                planet->troops -= casualties;
            }

            if (!(sect2->popn + sect2->troops)) {
                /* We got them */
                if (planet->info[sect2->owner - 1].numsectsowned) {
                    --planet->info[sect2->owner - 1].numsectsowned;
                }

                ++planet->info[playernum - 1].numsectsowned;
                sect2->owner = playernum;

                /* Mesomorphs absorb the bodies of their victims */
                absorbed = 0;

                if (race->absorb) {
                    absorbed = int_rand(0, old2popn + old3popn);
                    sprintf(buf, "%d alien bodies absorbed.\n", absorbed);
                    notify(playernum, governor, buf);

                    sprintf(buf,
                            "Metamorphs have absorbed %d bodies!!!\n",
                            absorbed);

                    notify(old2owner, old2gov, buf);
                }

                planet->info[playernum - 1].popn += absorbed;
                planet->popn += absorbed;

                if (what == CIV) {
                    sect2->popn = people + absorbed;
                } else if (what == MIL) {
                    sect2->popn = absorbed;
                    sect2->troops = people;
                }

                adjust_morale(race, alien, (int)alien->fighters);
            } else {
                /* Retreat */
                absorbed = 0;

                if (alien->absorb) {
                    absorbed = int_rand(0, oldpopn - people);
                    sprintf(buf, "%d alien bodies absorbed.\n", absorbed);
                    notify(old2owner, old2gov, buf);
                    sprintf(buf,
                            "Metamorphs have absorbed %d bodies!!!\n",
                            absorbed);

                    notify(playernum, governor, buf);
                    sect2->popn += absorbed;
                }

                planet->info[sect2->owner - 1].popn += absorbed;
                planet->popn += absorbed;

                if (what == CIV) {
                    sect->popn += people;
                } else if (what == MIL) {
                    sect->troops += people;
                }

                if (sect->troops + sect->popn) {
                    sect->owner = playernum;
                }

                adjust_morale(alien, race, (int)race->fighters);
            }

            if (sect2->owner == playernum) {
                sprintf(telegram_buf,
                        "/%s/%s: %s [%d] %c(%d,%d) assaults %s [%d] %c(%d,%d) VICTORY\n",
                        Stars[Dir[playernum - 1][governor].snum]->name,
                        Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                        race->name,
                        playernum,
                        Dessymbols[sect->condition],
                        x,
                        y,
                        alien->name,
                        alien->playernum,
                        Dessymbols[sect2->condition],
                        x2,
                        y2);
            } else {
                sprintf(telegram_buf,
                        "/%s/%s: %s [%d] %c(%d,%d) assaults %s [%d] %c(%d,%d) DEFEAT\n",
                        Stars[Dir[playernum - 1][governor].snum]->name,
                        Stars[Dir[playernum - 1][governor].snum]->pnames[Dir[playernum - 1][governor].pnum],
                        race->name,
                        playernum,
                        Dessymbols[sect->condition],
                        x,
                        y,
                        alien->name,
                        alien->playernum,
                        Dessymbols[sect2->condition],
                        x2,
                        y2);
            }

            if (sect2->owner == playernum) {
                sprintf(buf, "VICTORY! The sector is yours!\n");
                notify(playernum, governor, buf);
                sprintf(buf, "Sector CAPTURED\n");
                strcat(telegram_buf, buf);

                if (people) {
                    if (what == CIV) {
                        sprintf(buf, "%d civilians move in.\n", people);
                    } else {
                        sprintf(buf, "%d troops move in.\n", people);
                    }

                    notify(playernum, governor, buf);
                }

                planet->info[playernum - 1].mob_points += (int)sect2->mobilization;
                planet->info[old2owner - 1].mob_points -= (int)sect2->mobilization;
            } else {
                sprintf(buf, "The invasion was repulsed; try again.\n");
                notify(playernum, governor, buf);
                sprintf(buf, "You fought them off!\n");
                strcat(telegram_buf, buf);

                if (sect->popn) {
                    sect->owner = playernum;
                }

                /* End loop */
                done = 1;
            }

            if (!(sect->popn + sect->troops + people)) {
                sprintf(buf, "You killed all of them!\n");
                strcat(telegram_buf, buf);

                /* Increase modifier */
                if (race->translate[old2owner - 1] < BATTLE_MAX_KNOW) {
                    race->translate[old2owner - 1] = MIN(race->translate[old2owner - 1] + BATTLE_KNOW_GAIN, BATTLE_MAX_KNOW);
                } else {
                    race->translate[old2owner - 1] = race->translate[old2owner - 1];
                }

                if (planet->info[playernum - 1].numsectsowned) {
                    --planet->info[playernum - 1].numsectsowned;
                }
            }

            if (!people) {
                sprintf(buf,
                        "Oh no! They killed your party to the last man!\n");

                notify(playernum, governor, buf);

                if (alien->translate[playernum - 1] < BATTLE_MAX_KNOW) {
                    alien->translate[playernum - 1] = MIN(alien->translate[playernum - 1] + BATTLE_KNOW_GAIN, BATTLE_MAX_KNOW);
                } else {
                    alien->translate[playernum - 1] = alien->translate[playernum - 1];
                }
            }

            putrace(alien);
            putrace(race);

            if (what == CIV) {
                sprintf(buf,
                        "Casualties: You: %d civ/%d mil, them: %d civ\n",
                        casualties2,
                        casualties3,
                        casualties);
            } else {
                sprintf(buf,
                        "Casualties: You: %d civ/%d mil, them %d mil\n",
                        casualties2,
                        casualties3,
                        casualties);
            }

            strcat(telegram_bug, buf);
            warn(old2owner, old2gov, telegram_buf);

            if (what == CIV) {
                sprintf(buf,
                        "Casualties: You: %d civ, Them: %d civ/%d mil\n",
                        casualties,
                        casualties2,
                        casualties3);
            } else {
                sprintf(buf,
                        "Casualties: You: %d mil, Them: %d civ/%dmil\n",
                        casualties,
                        casualties2,
                        casualties3);
            }

            notify(playernum, governor, buf);
        } else {
            if (what == CIV) {
                sect->popn -= people;
                sect2->popn += people;
            } else if (what == MIL) {
                sect->troops -= people;
                sect2->troops += people;
            }

            if (!sect2->owner) {
                planet->info[playernum - 1].mob_points += (int)sect2->mobilization;
            }

            sect2->owner = playernum;
        }

        if (!(sect->popn + sect->troops)) {
            planet->info[playernum - 1].mob_points -= (int)sect->mobilization;
            sect->owner = 0;
        }

        if (!(sect2->popn + sect2->troops)) {
            sect2->owner = 0;
            done = 1;
        }

        putsector(sect, planet, x, y);
        putsector(sect2, planet, x2, y2);
        free(sect);
        free(sect2);

        deductAPs(playernum,
                  governor,
                  apcost,
                  Dir[playernum - 1][Governor].snum,
                  0);

        /* Get ready for the next round */
        x = x2;
        y = y2;
    }

    notify(playernum, governor, "Finished.\n");
    free(planet);
}

void walk(int playernum, int governor, int apcount)
{
    shiptype *ship;
    shiptype *ship2;
    shiptype dummy;
    planettype *p;
    sectortype *sect;
    int shipno;
    int x;
    int y;
    int i;
    int sh;
    int succ = 0;
    int civ;
    int mil;
    int damage;
    int oldowner;
    int oldgov;
    int strength;
    int strength1;
    racetype *race;
    racetype *alien;

    if (argn < 2) {
        notify(playernum, governor, "Walk what?\n");

        return;
    }

    if (args[1][0] == '#') {
        sscanf(args[1] + 1, "%d", &shipno);
    } else {
        sscanf(args[1], "%d", &shipno);
    }

    if (!getship(&ship, shipno)) {
        notify(playernum, governor, "No such ship.\n");

        return;
    }

    if (testship(playernum, governor, ship)) {
        /* The error message is handled by testship */
        free(ship);

        return;
    }

    if (ship->type != OTYPE_AFV) {
        notify(playernum, governor, "This ship doesn't walk!\n");
        free(ship);

        return;
    }

    if (!landed(ship)) {
        notify(playernum, governor, "This ship is not landed on a planet.\n");
        free(ship);

        return;
    }

    if (!ship->popn) {
        notify(playernum, governor, "No crew.\n");
        free(ship);

        return;
    }

    if (ship->fuel < AFV_FUEL_COST) {
        sprintf(buf, "You don't have %.1f fuel to move it.\n", AFV_FUEL_COST);
        notify(playernum, governor, buf);
        free(ship);

        return;
    }

    if (!enufAP(playernum, governor, Stars[ship->storbits]->AP[playernum - 1], apcount)) {
        free(ship);

        return;
    }

    getplanet(&p, (int)ship->storbits, (int)ship->pnumorbits);
    race = races[playernum - 1];

    if (!get_move(args[2][0], (int)ship->land_x, (int)ship->land_y, &x, &y, p)) {
        notify(playernum, governor, "Illegal move.\n");
        free(p);
        free(ship);

        return;
    }

    if ((x < 0)
        || (y < 0)
        || (x > (p->Maxx - 1))
        || (y > (p->Maxy - 1))) {
        sprintf(buf, "Illegal coordinates %d,%d.\n", x, y);
        notify(playernum, governor, buf);
        free(ship);

        putplanet(p,
                  Dir[playernum - 1][governor].snum,
                  Dir[playernum - 1][governor].pnum);

        free(p);

        return;
    }

    if (!getsector(&sect, p, x, y)) {
        notify(playernum,
               governor,
               "Error in sector database, notify deity.\n");

        free(ship);
        free(p);

        return;
    }

#if 0
    /* Check to see if player is permitted on the sector type */
    if (!race->likes[sect->condition]) {
        notify(playernum,
               governor,
               "You ships cannot walk into that sector type!\n");

        free(ship);
        free(p);
        free(sect);

        return;
    }

#endif

    /* If the sector is occupied by non-aligned AFVs, each one will attack */
    sh = p->ships;

    while (sh && ship->alive) {
        getship(&ship2, sh);

        if ((ship2->owner != playernum)
            && (ship2->type == OTYPE_AFV)
            && landed(ship2)
            && retal_strength(ship2)
            && (ship2->land_x == x)
            && (ship2->land_y == y)) {
            alien = races[ship2->owner - 1];

            if (!isset(race->allied, (int)ship2->owner)
                || !isset(alien->allied, playernum)) {
                strength = retal_strength(ship2);
                strength1 = retal_strength(ship);

                while (strength && strength1) {
                    memcpy(&dummy, ship, sizeof(shiptype));

                    damage = shoot_ship_to_ship(ship2,
                                                ship,
                                                strength,
                                                0,
                                                0,
                                                long_buf,
                                                short_buf);

                    use_destruct(ship2, strength);
                    notify(playernum, governor, long_buf);
                    warn((int)ship2->owner, (int)ship2->governor, long_buf);

                    if (!ship2->alive) {
                        post(short_buf, COMBAT);
                    }

                    notify_star(playernum,
                                governor,
                                (int)ship2->owner,
                                (int)ship->storbits,
                                short_buf);

                    if (strength1) {
                        damage = shoot_ship_to_ship(&dummy,
                                                    ship2,
                                                    strength1,
                                                    0,
                                                    1,
                                                    long_buf,
                                                    short_buf);

                        use_destruct(ship, strength1);
                        notify(playernum, governor, long_buf);
                        warn((int)ship2->owner, (int)ship2->governor, long_buf);

                        if (!ship2->alive) {
                            post(short_buf, COMBAT);
                        }

                        notify_star(playernum,
                                    governor,
                                    (int)ship2->owner,
                                    (int)ship->storbits,
                                    short_buf);
                    }

                    strength = retal_strength(ship2);
                    strength1 = retal_strength(ship);
                }

                putship(ship2);
            }
        }

        sh = nextship(ship2);
        free(ship2);
    }

    /* If the sector is occupied by non-aligned player, attack them first */
    if (ship->popn
        && ship->alive
        && sect->owner
        && (sect->owner != playernum)) {
        oldowner = sect->owner;
        oldgov = Stars[ship->storbits]->governor[sect->owner - 1];
        alien = races[oldowner - 1];

        if (!isset(race->allied, oldowner)
            || !isset(alien->allied, playernum)) {
            if (!retal_strength(ship)) {
                notify(playernum,
                       governor,
                       "You have nothing to attack with!\n");

                free(ship);
                free(p);
                free(sect);

                return;
            }

            while ((sect->popn + sect->troops) && retal_strength(ship)) {
                civ = (int)sect->popn;
                mil = (int)sect->troops;

                mech_attack_people(ship,
                                   &civ,
                                   &mil,
                                   race,
                                   alien,
                                   sect,
                                   x,
                                   y,
                                   0,
                                   long_buf,
                                   short_buf);

                notify(playernum, governor, long_buf);
                warn(alien->playernum, oldgov, long_buf);

                notify_star(playernum,
                            governor,
                            oldowner,
                            (int)ship->storbits,
                            short_buf);

                post(short_buf, COMBAT);

                people_attack_mech(ship,
                                   (int)sect->popn,
                                   (int)sect->troops,
                                   alien,
                                   race,
                                   sect,
                                   x,
                                   y,
                                   long_buf,
                                   short_buf);

                notify(playernum, governor, long_buf);
                warn(alien->playernum, oldgov, long_buf);

                notify_star(playernum,
                            governor,
                            oldowner,
                            (int)ship->storbits,
                            short_buf);

                if (!ship->alive) {
                    post(short_buf, COMBAT);
                }

                sect->popn;
                sect->troops = mil;

                if (!(sect->popn + sect->troops)) {
                    p->info[sect->owner - 1].mob_points -= (int)sect->mobilization;
                    sect->owner = 0;
                }
            }
        }

        putrace(alien);
        putrace(race);

        putplanet(p,
                  Dir[playernum - 1][governor].snum,
                  Dir[playernum - 1][governor].pnum);

        putsector(sect, p, x, y);
    }

    if (((sect->owner == playernum)
         || isset(race->allied, (int)sect->owner)
         || !sect->owner)
        && ship->alive) {
        succ = 1;
    }

    if (ship->alive && ship->popn && succ) {
        sprintf(buf,
                "%s moving from %d,%d to %d,%d on %s.\n",
                Ship(ship),
                (int)ship->land_x,
                (int)ship->land_y,
                x,
                y,
                Dispshiploc(ship));

        ship->land_x = x;
        ship->land_y = y;
        use_fuel(ship, AFV_FUEL_COST);

        for (i = 1; i <= Num_races; ++i) {
            if ((i != playernum) && p->info[i - 1].numsectsowned) {
                notify(i,
                       (int)Stars[Dir[playernum - 1][governor].snum]->governor[i - 1],
                       buf);
            }
        }
    }

    putship(ship);
    deductAPs(playernum, governor, apcount, (int)ship->storbits, 0);
    free(ship);
    free(p);
    free(sect);
}

int get_move(char direction, int x, int y, int *x2, int *y2, planettype *planet)
{
    switch (direction) {
    case '1':
    case 'b':
        *x2 = x - 1;
        *y2 = y + 1;

        if (*x2 == -1) {
            *x2 = planet->Maxx - 1;
        }

        return 1;
    case '2':
    case 'k':
        *x2 = x;
        *y2 = y + 1;

        return 1;
    case '3':
    case 'n':
        *x2 = x + 1;
        *y2 = y + 1;

        if (*x2 == planet->Maxx) {
            *x2 = 0;
        }

        return 1;
    case '4':
    case 'h':
        *x2 = x - 1;
        *y2 = y;

        if (*x2 == -1) {
            *x2 = planet->Maxx - 1;
        }

        return 1;
    case '6':
    case 'l':
        *x2 = x + 1;
        *y2 = y;

        if (*x2 == planet->Maxx) {
            *x2 = 0;
        }

        return 1;
    case '7':
    case 'y':
        *x2 = x - 1;
        *y2 = y - 1;

        if (*x2 == -1) {
            *x2 = planet->Maxx - 1;
        }

        return 1;
    case '8':
    case 'j':
        *x2 = x;
        *y2 = y - 1;

        return 1;
    case '9':
    case 'u':
        *x2 = x + 1;
        *y2 = y - 1;

        if (*x2 == planet->Maxx) {
            *x2 = 0;
        }

        return 1;
    default:
        *x2 = x;
        *y2 = y;

        return 0;
    }
}

void mech_defend(int playernum,
                 int governor,
                 int *people,
                 int type,
                 planettype *p,
                 int x,
                 int y,
                 sectortype *s,
                 int x2,
                 int y2,
                 sectortype *s2)
{
    int sh;
    shiptype *ship;
    int civ = 0;
    int mil = 0;
    int oldgov;
    racetype *race;
    racetype *alien;

    if (type == CIV) {
        civ = *people;
    } else   {
        mil = *people;
    }

    sh = p->ships;
    race = races[playernum - 1];

    while (sh && (civ + mil)) {
        if (getship(&ship, sh)) {
            if ((ship->owner != playernum)
                && (ship->type == OTYPE_AFV)
                && landed(ship)
                && retal_strength(ship)
                && (ship->land_x == x2)
                && (ship->land_y == y2)) {
                alien = races[ship->owner - 1];

                if (!isset(race->allied, (int)ship->owner)
                    || !isset(alien->allied, playernum)) {
                    while ((civ + mil) && retal_strength(ship)) {
                        oldgov = Stars[ship->storbits]->governor[alien->Playernum - 1];

                        mech_attack_people(ship,
                                           &civ,
                                           &mil,
                                           alien,
                                           race,
                                           s2,
                                           x2,
                                           y2,
                                           1,
                                           long_buf,
                                           short_buf);

                        notify(playernum, governor, long_buf);
                        warn(alien->playernum, oldgov, long_buf);

                        if (civ + mil) {
                            people_attack_mech(ship,
                                               civ,
                                               mil,
                                               race,
                                               alien,
                                               s2,
                                               x2,
                                               y2,
                                               long_buf,
                                               short_buf);

                            notify(playernum, governor, long_buf);
                            warn(alien->playernum, oldgov, long_buf);
                        }
                    }
                }

                putship(ship);
            }

            sh = nextship(ship);
            free(ship);
        }
    }

    *people = civ + mil;
}

void mech_attack_people(shiptype *ship,
                        int *civ,
                        int *mil,
                        racetype *race,
                        racetype *alien,
                        sectortype *sect,
                        int x,
                        int y,
                        int ignore,
                        char *long_msg,
                        char *short_msg)
{
    int strength;
    int oldciv;
    int oldmil;
    double astrength;
    double dstrength;
    int cas_civ;
    int cas_mil;
    int ammo;

    oldciv = *civ;
    oldmil = *mil;

    strength = retal_strength(ship);

    astrength = MECH_ATTACK
        * ship->tech
        * (double)strength
        * ((double)ship->armor + 1.0)
        * 0.01
        * (100.0 - (double)ship->damage)
        * 0.01
        * (race->likes[sect->condition] + 1.0)
        * morale_factor((double)(race->morale - alien->morale));

    dstrength = (double)((10 * oldmil * alien->fighters) + oldciv)
        * 0.01
        * alien->tech
        * 0.01
        * (alien->likes[sect->condition] + 1.0)
        * ((double)Defensedata[sect->condition] + 1.0)
        * morale_factor((double)(alien->morale - race->morale));

    if (ignore) {
        ammo = (int)log10((double)dstrength + 1.0) - 1;
        ammo = MIN(MAX(ammo, 0), strength);
        use_destruct(ship, ammo);
    } else {
        use_destruct(ship, strength);
    }

    cas_civ = int_rand(0, round_rand(((double)oldciv * astrength) / dstrength));
    cas_civ = MIN(oldciv, cas_civ);
    cas_mil = int_rand(0, round_rand(((double)oldmil * astrength) / dstrength));
    cas_mil = MIN(oldmil, cas_mil);
    *civ -= cas_civ;
    *mil -= cas_mil;

    if (*civ + *mil) {
        sprintf(short_msg,
                "%s: %s attacked %s [%d]\n",
                Dispshiploc(ship),
                Ship(ship),
                alien->name,
                alien->Playernum);
    } else {
        sprintf(short_msg,
                "%s: %s slaughtered %s [%d]\n",
                Dispshiploc(ship),
                Ship(ship),
                alien->name,
                alien->Playernum);
    }

    strcpy(long_msg, short_msg);

    sprintf(buf,
            "\tBattle at %d,%d %s: %d guns fired on %d civ/%d mil\n",
            x,
            y,
            Desnames[sect->condition],
            strength,
            oldciv,
            oldmil);

    strcat(long_msg, buf);
    sprintf(buf, "\tAttack: %.3f   Defense: %.3f.\n", astrength, dstrength);
    strcat(long_msg, buf);
    sprintf(buf, "\t%d civ/%d mil killed.\n", cas_civ, cas_mil);
    strcat(long_msg, buf);
}

void people_attack_mech(shiptype *ship,
                        int civ,
                        int mil,
                        racetype *race,
                        racetype *alien,
                        sectortype *sect,
                        int x,
                        int y,
                        char *long_msg,
                        char *short_msg)
{
    int strength;
    double astrength;
    double dstrength;
    int cas_civ;
    int cas_mil;
    int pdam;
    int sdam;
    int damage;
    int ammo;

    strength = retal_strength(ship);

    dstrength = MECH_ATTACK
        * ship->tech
        * (double)strength
        * ((double)ship->armor + 1.0)
        * 0.01
        * (100.0 - (double)ship->damage)
        * 0.01
        * (alien->likes[sect->condition] + 1.0)
        * morale_factor((double)(alien->morale - race->morale));

    astrength = (double)((10 * mil * race->figthers) + civ)
        * 0.01
        * race->tech
        * 0.01
        * (race->likes[sect->condition] + 1.0)
        * ((double)Defensedata[sect->condition] + 1.0)
        * moral_factor((double)(race->morale - alien->morale));

    ammo = (int)log10((double)astrength + 1.0) - 1;
    ammo = MIN(strength, MAX(0, ammo));
    use_destruct(ship, ammo);
    damage = int_rand(0, round_rand((100.0 * astrength) / dstrength));
    damage = MIN(100, damage);
    ship->damage += damage;

    if (ship->damage >= 100) {
        ship->damage = 100;
        kill_ship(race->Playernum, ship);
    }

    do_collateral(ship, damage, &cas_civ, &cas_mil, &pdam, &sdam);

    if (ship->alive) {
        sprintf(short_msg,
                "%s: %s [%d] attacked %s\n",
                Dispshiploc(ship),
                race->name,
                race->Playernum,
                Ship(ship));
    } else {
        sprintf(short_msg,
                "%s: %s [%d] DESTROYED %s\n",
                Dispshiploc(ship),
                race->name,
                race->Playernum,
                Ship(ship));
    }

    strcpy(long_msg, short_msg);

    sprintf(buf,
            "\tBattle at %d,%d %s: %d civ/%d mil assault %s\n",
            x,
            y,
            Desnames[sect->condition],
            civ,
            mil,
            Shipnames[ship->type]);

    strcat(long_msg, buf);
    sprintf(buf, "\tAttack: %.3f   Defense: %.3f.\n", astrength, dstrength);
    strcat(long_msg, buf);

    sprintf(buf,
            "\t%d%% damage inflicted for a total of %d%%\n",
            damage,
            ship->damage);

    strcat(long_msg, buf);

    sprintf(buf,
            "\t%d civ/%d mil killed   %d prim/%d sec guns knocked out\n",
            cas_civ,
            cas_mil,
            pdam,
            sdam);

    strcat(long_msg, bug);
}

void ground_attack(racetype *race,
                   racetype *aliend,
                   int *people,
                   int what,
                   unsigned short *civ,
                   unsigned short *mil,
                   unsigned int def1,
                   unsigned int def2,
                   double alikes,
                   double dlikes,
                   double *astrength;
                   double *dstrength;
                   int *casualties,
                   int *casualties2,
                   int *casualties3)
{
    int casualty_scale;

    if (what == MIL) {
        *astrength = (double)(*people * race->fighters * 10)
            * (alikes + 1.0)
            * ((double)def1 + 1.0)
            * morale_factor((double)(race->morale - alien->morale));
    } else {
        *astrength = (double)(*people * race->fighters)
            * (alikes + 1.0)
            * ((double)def1 + 1.0)
            * morale_factor((double)(race->morale - alien->morale));
    }

    *dstrength = (double)((*civ + (*mil * 10)) * alien->fighters)
        * (dlikes + 1.0)
        * ((double)def2 + 1.0)
        * morale_factor((double)(alien->morale - race->morale));

    /* Nuke both populations */
    if (what == MIL) {
        casualty_scale = MIN(*people * 10 * race->fighers,
                             (*civ + (*mil * 10)) * alien->fighters);
    } else {
        casualty_scale = MIN(*people * race->fighers,
                             (*civ + (*mil * 10)) * alien->fighers);
    }

    if (what == MIL) {
        *casualties = int_rand(0, round_rand((double)(((casualty_scale / 10) * dstrength) / *astrength)));
    } else {
        *casualties = int_rand(0, round_rand((double)((casualty_scale * dstrength) / *astrength)));
    }

    *casualties = MIN(*people, *casualties);
    *people -= *casualties;

    *casualties2 = int_rand(0, round_rand(((double)casualty_scale * *astrength) / *dstrength));

    *casualties2 = MIN(*civ, *casualties2);
    *civ -= *casualties2;

    /* And for troops */
    *casualties3 = int_rand((double)(((casualty_scale / 10) * *astrength) / *dstrength));

    *casualties3 = MIN(*mil, *casualties3);
    *mil -= *casualties3;
}
