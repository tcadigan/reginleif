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
 * dock.c - Dock a ship and...assault -- A very un-PC version of dock
 *
 * #ident  "@(#)dock.c  1.12 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/dock.c,v 1.5 2007/07/06 18:09:34 gbp Exp $
 */
#include "dock.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "../server/buffers.h"
#include "../server/doship.h"
#include "../server/files_shl.h"
#include "../server/GB_server.h"
#include "../server/lists.h"
#include "../server/max.h"
#include "../server/misc.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/rand.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/shlmisc.h"
#include "../server/vars.h"

#include "capture.h"
#include "fire.h"
#include "load.h"
#include "tele.h"

void dock(int playernum, int governor, int apcount, int assault, orbitinfo *unused5)
{
    int boarders = 0;
    int dam = 0;
    int dam2 = 0;
    int booby = 0;
    int ship2no;
    int shipno;
    int what = -1;
    int nextshipno;
    int old2owner = -1;
    int old2gov = -1;
    int casualties = 0;
    int casualties2 = 0;
    int casualties3 = 0;
    int casualty_scale = 0;
    double fuel;
    double bstrength = -1.0;
    double b2strength = -1.0;
    double Dist;
    char dfire[MAXARGS][COMMANDSIZE];
    shiptype *s;
    shiptype *s2;
    shiptype *s3;
    shiptype ship;
    racetype *race = NULL;
    racetype *alien = NULL;

    if (argn < 3) {
        notify(playernum, governor, "Dock with what?\n");

        return;
    }

    if (argn < 5) {
        what = MIL;
    } else if (assault) {
        if (match(args[4], "civilians")) {
            what = CIV;
        } else if (match(args[4], "military")) {
            what = MIL;
        } else {
            notify(playernum, governor, "Assault with what?\n");

            return;
        }
    }

    nextshipno = start_shiplist(playernum, governor, args[1]);
    shipno = do_shiplist(&s, &nextshipno);

    while (shipno) {
        if (in_list(playernum, args[1], s, &nextshipno)) {
            if (!authorized(governor, s)) {
                sprintf(buf,
                        "You are not authorized to control ship %s...\n",
                        Ship(s));

                notify(playernum, governor, buf);
                free(s);
                shipno = do_shiplist(&s, &nextshipno);

                continue;
            } else {
                if (assault && s->type) {
                    notify(playernum,
                           governor,
                           "Sorry. Pods cannot be used to assault.\n");

                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (assault && (s->type == OTYPE_TERRA)) {
                    notify(playernum,
                           governor,
                           "Sorry. TFD's cannot be used to assault.\n");

                    notify(playernum, governor, "Nice try though!\n");
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (!assault) {
                    if (s->docked || (s->whatorbits == LEVEL_SHIP)) {
                        sprintf(buf, "%s is already docked.\n", Ship(s));
                        notify(playernum, governor, buf);
                        free(s);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }
                } else if (s->docked) {
                    notify(playernum, governor, "Your ship is already docked.\n");
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                } else if (s->whatorbits == LEVEL_SHIP) {
                    notify(playernum,
                           governor,
                           "Your ship is landed on another ship.\n");

                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (s->whatorbits == LEVEL_UNIV) {
                    if (!enufAP(playernum, governor, Sdata.AP[playernum - 1], apcount)) {
                        free(s);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }
                } else if (!enufAP(playernum, governor, Stars[s->storbits]->AP[playernum - 1], apcount)) {
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (assault && (what == CIV) && !s->popn) {
                    notify(playernum,
                           governor,
                           "You have no crew on this ship to assault with.\n");

                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                } else if (assault && (what == MIL) && !s->troops) {
                    notify(playernum,
                           governor,
                           "You have no troops on this ship to assault with.\n");

                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (args[2][0] == '#') {
                    sscanf(args[2] + 1, "%d", &ship2no);
                } else {
                    sscanf(args[2], "%d", &ship2no);
                }

                if (shipno == ship2no) {
                    notify(playernum, governor, "You can't dock with yourself!\n");
                    free(s);
                    shipno = do_shiplist(&s, &nextshipno);

                    continue;
                }

                if (!getship(&s2, ship2no)) {
                    notify(playernum, governor, "The ship wasn't found.\n");
                    free(s);

                    return;
                }

                /* Check if ships are on same scope level. Maarten */
                if (s->whatorbits != s2->whatorbits) {
                    notify(playernum,
                           governor,
                           "Those ships are not in the same scope.\n");

                    free(s);
                    free(s2);

                    return;
                }

#ifdef USE_VN
                /* CWL */
                if (assault && ((s2->type == OTYPE_VN) || (s2->type == OTYPE_BERS))) {
                    notify(playernum,
                           governor,
                           "You can't assault Von Neumann machines.\n");

                    free(s);
                    free(s2);

                    return;
                }
#endif

                /* Remove docked status? */
                if (s2->docked || (s2->whatorbits == LEVEL_SHIP)) {
                    sprintf(buf, "%s is already docked.\n", Ship(s2));
                    notify(playernum, governor, buf);
                    free(s);
                    free(s2);

                    return;
                }

                Dist = sqrt((double)Distsq(s2->xpos, s2->ypos, s->xpos, s->ypos));

                if (assault) {
                    fuel = 0.05 + (Dist * 0.025 * 2.0 * sqrt((double)s->mass));
                } else {
                    fuel = 0.05 + (Dist * 0.025 * 1.0 * sqrt((double)s->mass));
                }

                if (Dist > DIST_TO_DOCK) {
                    sprintf(buf,
                            "%s must be %.2f or closer to %s.\n",
                            Ship(s),
                            DIST_TO_DOCK,
                            Ship(s2));

                    notify(playernum, governor, buf);
                    free(s);
                    free(s2);

                    shipno = do_shiplist(&s, &nextshipno);
                } else if (s->docked && assault) {
                    /* First undock the target ship */
                    s->docked = 0;
                    s->whatdest = LEVEL_UNIV;
                    getship(&s3, (int)s->destshipno);
                    s3->docked = 0;
                    s3->whatdest = LEVEL_UNIV;
                    putship(s3);
                    free(s3);
                }

                if (fuel > s->fuel) {
                    sprintf(buf, "Not enough fuel.\n");
                    notify(playernum, governor, buf);
                    free(s);
                    free(s2);

                    continue;
                }

                sprintf(buf, "Distance to %s: %.2f.\n", Ship(s2), Dist);
                notify(playernum, governor, buf);

                sprintf(buf,
                        "This maneuver will take %.2f fuel (of %.2f.)\n\n",
                        fuel,
                        s->fuel);

                notify(playernum, governor, buf);

                if (s2->docked && !assault) {
                    sprintf(buf, "%s is already docked.\n", Ship(s2));
                    notify(playernum, governor, buf);
                    free(s);
                    free(s2);

                    return;
                }

                /* Defending fire gets defensive fire */
                memcpy(&ship, s2, sizeof(shiptype)); /* For reports */

                if (assault) {
                    int oldargn = argn;

                    argn = 3;
                    strcpy(dfire[0], args[0]);
                    strcpy(dfire[1], args[1]);
                    strcpy(dfire[2], args[2]);
                    sprintf(args[0], "fire");
                    sprintf(args[1], "#%d", ship2no);
                    sprintf(args[2], "#%d", shipno);
                    fire((int)s2->owner, (int)s2->governor, 0, 0, NULL);
                    argn = oldargn;
                    strcpy(args[0], dfire[0]);
                    strcpy(args[1], dfire[1]);
                    strcpy(args[2], dfire[2]);

                    /* Retrieve ships again, since battle may change ship stats */
                    free(s);
                    free(s2);
                    getship(&s, shipno);
                    getship(&s2, ship2no);

                    if (!s->alive) {
                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    } else if (!s2->alive) {
                        free(s);
                        free(s2);

                        return;
                    }
                }

                if (assault) {
                    alien = races[s2->owner - 1];
                    race = races[playernum - 1];

                    if (argn >= 4) {
                        sscanf(args[3], "%d", & boarders);

                        if ((what == MIL) && (boarders > s->troops)) {
                            boarders = s->troops;
                        } else if ((what == CIV) && (boarders > s->popn)) {
                            boarders = s->popn;
                        }
                    } else {
                        if (what == CIV) {
                            boarders = s->popn;
                        } else if (what == MIL) {
                            boarders = s->troops;
                        }
                    }

                    if (boarders > s2->max_crew) {
                        boarders = s2->max_crew;
                    }

                    /* Allow assault of crewless ships. */
                    if (s2->max_crew && (boarders <= 0)) {
                        sprintf(buf,
                                "Illegal number of boarders (%d).\n",
                                boarders);

                        notify(playernum, governor, buf);
                        free(s);
                        free(s2);
                        shipno = do_shiplist(&s, &nextshipno);

                        continue;
                    }

                    old2owner = s2->owner;
                    old2gov = s2->governor;

                    if (what == MIL) {
                        s->troops -= boarders;
                    } else if (what == CIV) {
                        s->popn -= boarders;
                    }

                    s->mass -= (boarders * race->mass);

                    if (what == MIL) {
                        bstrength = boarders * 10 * race->fighters * 0.01 * race->tech * morale_factor((double)(race->morale - alien->morale));
                    } else {
                        bstrength = boarders * 1 * 0.01 * race->tech * morale_factor((double)(race->morale - alien->morale));
                    }

                    b2strength = (s2->popn + (10 * s2->troops * alien->fighters)) * 0.01 * alien->tech * morale_factor((double)(alien->morale - race->morale));

                    sprintf(buf,
                            "Boarding strength: %.2f       Defense strength: %.2f.\n",
                            bstrength,
                            b2strength);

                    notify(playernum, governor, buf);
                }

                /* The ship moves into position, regardless of success of attack */
                use_fuel(s, fuel);
                s->xpos = s2->xpos + int_rand(-1, 1);
                s->ypos = s2->ypos + int_rand(-1, 1);

                if (s->hyper_drive.on) {
                    s->hyper_drive.on = 0;
                    notify(playernum, governor, "Hyper-drive deactivated.\n");
                }

                if (assault) {
                    /* If the assaulted ship is docked, undock it first */
                    if (s2->docked && (s2->whatdest == LEVEL_SHIP)) {
                        getship(&s3, (int)s2->destshipno);
                        s2->docked = 0;
                        s3->whatdest = LEVEL_UNIV;
                        s3->destshipno = 0;
                        putship(s3);
                        free(s3);

                        s2->docked = 0;
                        s2->whatdest = LEVEL_UNIV;
                        s2->destshipno = 0;
                    }

                    /* Nuke both populations, ships */
                    casualty_scale = MIN(boarders, s2->troops + s2->popn);

                    /* Otherwise the ship surrenders */
                    if (b2strength) {
                        casualties = int_rand(0, round_rand(((double)casualty_scale * (b2strength + 1.0)) / (bstrength + 1.0)));

                        casualties = MIN(boarders, casualties);
                        boarders -= casualties;

                        dam = int_rand(0, round_rand((25.0 * (b2strength + 1.0)) / (bstrength + 1.0)));

                        dam = MIN(100, dam);
                        s->damage = MIN(100, s->damage + dam);

                        if (s->damage >= 100) {
                            kill_ship(playernum, s);
                        }

                        casualties2 = int_rand(0, round_rand(((double)casualty_scale * (bstrength + 1.0)) / (b2strength + 1.0)));

                        casualties2 = MIN(s2->popn, casualties2);

                        casualties3 = int_rand(0, round_rand(((double)casualty_scale * (bstrength + 1.0)) / (b2strength + 1.0)));

                        casualties3 = MIN(s2->troops, casualties3);
                        s2->popn -= casualties2;
                        s2->mass -= (casualties2 * alien->mass);
                        s2->troops -= casualties3;
                        s2->mass -= (casualties3 * alien->mass);

                        /* (Their mass) */
                        dam2 = int_rand(0, round_rand((25.0 * (bstrength + 1.0)) / (b2strength + 1.0)));

                        dam2 = MIN(100, dam2);
                        s2->damage = MIN(100, s2->damage + dam2);

                        if (s2->damage >= 100) {
                            kill_ship(playernum, s2);
                        }
                    } else {
                        s2->popn = 0;
                        s2->troops = 0;
                        booby = 0;
                        /* Do booby traps, check for boobytrapping */
                        if (!s2->max_crew && s2->destruct) {
                            booby = int_rand(0, 10 * (int)s2->destruct);
                        }

                        booby = MIN(100, booby);
                    }

                    if ((!s2->popn && !s2->troops)
                        && s->alive
                        && s2->alive
                        && !s2->type) {
                        /* We got them */
                        s->docked = 1;
                        s->whatdest = LEVEL_SHIP;
                        s->destshipno = ship2no;

                        s2->docked = 1;
                        s2->whatdest = LEVEL_SHIP;
                        s2->destshipno = shipno;
                        old2gov = s2->governor;

                        /* Remove ship ship from fleet -mfw */
                        remove_sh_fleet(old2owner, old2gov, s2);
                        s2->owner = s->owner;
                        s2->governor = s->governor;

                        if (what == MIL) {
                            s2->troops = boarders;
                        } else {
                            s2->popn = boarders;
                        }

                        s2->mass += (boarders * race->mass); /* Our mass */

                        if (casualties2 + casualties3) {
                            /* You must kill to get morale */
                            adjust_morale(race, alien, (int)s2->build_cost);
                        }
                    } else {
                        /* Retreat */
                        if (what == MIL) {
                            s->troops += boarders;
                        } else if (what == CIV) {
                            s->popn += boarders;
                        }

                        s->mass += (boarders * race->mass);
                        adjust_morale(alien, race, (int)race->fighters);
                    }

                    /* Races find out about each other */
                    if (alien->translate[playernum - 1] < BATTLE_MAX_KNOW) {
                        alien->translate[playernum - 1] = MIN(alien->translate[playernum - 1] + BATTLE_KNOW_GAIN, BATTLE_MAX_KNOW);
                    } else {
                        alien->translate[playernum - 1] = alien->translate[playernum - 1];
                    }

                    if (race->translate[old2owner - 1] < BATTLE_MAX_KNOW) {
                        race->translate[old2owner - 1] = MIN(race->translate[old2owner - 1] + BATTLE_KNOW_GAIN, BATTLE_MAX_KNOW);
                    } else {
                        race->translate[old2owner - 1] = race->translate[old2owner - 1];
                    }

                    /* Boarding party killed */
                    if (!boarders && (s2->popn + s2->troops)) {
                        alien->translate[playernum - 1] = MIN(alien->translate[playernum - 1] + 25, 100);
                    }

                    /* Captured ship */
                    if (s2->owner == playernum) {
                        race->translate[old2owner - 1] = MIN(race->translate[old2owner - 1] + 25, 100);
                    }

                    putrace(race);
                    putrace(alien);
                } else {
                    s->docked = 1;
                    s->whatdest = LEVEL_SHIP;
                    s->destshipno = ship2no;

                    s2->docked = 1;
                    s2->whatdest = LEVEL_SHIP;
                    s2->destshipno = shipno;
                }

                if (assault) {
                    sprintf(telegram_buf,
                            "%s ASSAULTED by %s at %s\n",
                            Ship(&ship),
                            Ship(s),
                            prin_ship_orbits(s2));

                    sprintf(buf,
                            "Your damage: %d%%, theirs: %d%%.\n",
                            dam2,
                            dam);

                    strcat(telegram_buf, buf);

                    if (!s2->max_crew && s2->destruct) {
                        sprintf(buf,
                                "(Your boobytrap gave them %d%% damage.)\n",
                                booby);

                        strcat(telegram_buf, buf);
                        sprintf(buf,
                                "(Their boobytrap gave you %d%% damage!)\n",
                                booby);

                        notify(playernum, governor, buf);
                    }

                    sprintf(buf,
                            "Damage taken:  You: %d%% (now %d%%)\n",
                            dam,
                            s->damage);

                    notify(playernum, governor, buf);

                    if (!s->alive) {
                        sprintf(buf,
                                "              YOUR SHIP WAS DESTROYED!!!\n");

                        notify(playernum, governor, buf);

                        sprintf(buf,
                                "              Their ship DESTROYED!!!\n");

                        strcat(telegram_buf, buf);
                    }

                    sprintf(buf,
                            "              Them: %d %% (now %d%%)\n",
                            dam2,
                            s2->damage);

                    notify(playernum, governor, buf);

                    if (!s2->alive) {
                        sprintf(buf,
                                "              Their ship DESTROYED!!! Boarders are dead.\n");

                        notify(playernum, governor, buf);
                        sprintf(buf,
                                "              YOUR SHIP WAS DESTROYED!!!\n");

                        strcat(telegram_buf, buf);
                    }

                    if ((!s2->popn && !s2->troops)
                        && s->alive
                        && s2->alive
                        && s2->type) {
                        sprintf(buf,
                                "Boarders are unable to control the pod.\n");

                        notify(playernum, governor, buf);
                    }

                    if (s->alive) {
                        if (s2->owner == playernum) {
                            sprintf(buf, "CAPTURED!\n");
                            strcat(telegram_buf, buf);
                            sprintf(buf, "VICTORY! The ship is yours!\n");
                            notify(playernum, governor, buf);

                            if (boarders) {
                                sprintf(buf,
                                        "%d boarders move in.\n",
                                        boarders);

                                notify(playernum, governor, buf);
                            }

                            capture_stuff(s2);
                        } else if(s2->popn + s2->troops) {
                            sprintf(buf,
                                    "The boarding was repulsed; try again.\n");

                            notify(playernum, governor, buf);
                            sprintf(buf, "You fought them off!\n");
                            strcat(telegram_buf, buf);
                        }
                    } else {
                        sprintf(buf,
                                "The assault was too much for your bucket of bolts.\n");

                        notify(playernum, governor, buf);
                        sprintf(buf,
                                "The assault was too much for their ship...\n");

                        strcat(telegram_buf, buf);
                    }

                    if (s2->alive) {
                        if (s2->max_crew && !boarders) {
                            sprintf(buf,
                                    "Oh no! They killed your boarding party to the last man!\n");

                            notify(playernum, governor, buf);
                        }

                        if (!s->popn && !s->troops) {
                            sprintf(buf, "You killed all their crew!\n");
                            strcat(telegram_buf, buf);
                        }
                    } else {
                        sprintf(buf,
                                "The assault weakened their ship too much!\n");

                        notify(playernum, governor, buf);
                        sprintf(buf, "You ship was weakened too much!\n");
                        strcat(telegram_buf, buf);
                    }

                    if (what == MIL) {
                        sprintf(buf,
                                "Casualties: Yours: %d mil/%d civ    Theirs: %d mil\n",
                                casualties3,
                                casualties2,
                                casualties);
                    } else {
                        sprintf(buf,
                                "Casualties: Yours: %d mil/%d civ    Theirs: %d civ\n",
                                casualties3,
                                casualties2,
                                casualties);
                    }

                    strcat(telegram_buf, buf);

                    if (what == MIL) {
                        sprintf(buf,
                                "Crew casualties: Yours: %d mil    Theirs: %d mil/%d civ\n",
                                casualties,
                                casualties3,
                                casualties2);
                    } else {
                        sprintf(buf,
                                "Crew casualties: Yours: %d civ    Theirs: %d mil/%d civ\n",
                                casualties,
                                casualties3,
                                casualties2);
                    }

                    notify(playernum, governor, buf);
                    warn(old2owner, old2gov, telegram_buf);

                    sprintf(buf,
                            "%s %s %s at %s.\n",
                            Ship(s),
                            s2->alive ? (s2->owner == playernum ? "CAPTURED" : "assualted") : "DESTROYED",
                            Ship(&ship),
                            prin_ship_orbits(s));

                    if ((s2->owner == playernum) || !s2->alive) {
                        post(buf, COMBAT);
                    }

                    notify_star(playernum,
                                governor,
                                old2owner,
                                (int)s->storbits,
                                buf);
                } else {
                    sprintf(buf,
                            "%s docked with %s.\n",
                            Ship(s),
                            Ship(s2));

                    notify(playernum, governor, buf);
                }

                if (Dir[playernum - 1][governor].level == LEVEL_UNIV) {
                    deductAPs(playernum, governor, apcount, 0, 1);
                } else {
                    deductAPs(playernum,
                              governor,
                              apcount,
                              Dir[playernum - 1][governor].snum,
                              0);
                }

                s2->notified = 0;
                s->notified = s2->notified;
                putship(s);
                putship(s2);
                free(s2);
                free(s);
            }
        }

        shipno = do_shiplist(&s, &nextshipno);
    }
}
