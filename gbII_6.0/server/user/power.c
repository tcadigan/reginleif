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
 * power.c -- Display power report
 *
 * #ident  "@(#)power.c 1.10 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/power.c,v 1.3 2007/07/06 18:09:34 gbp Exp $
 */

#include <errno.h>
#include <string.h>
#include <time.h>

#include "buffers.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

extern int errno;
extern struct tm *update_tm;

extern void block(int, int, int);
extern void power(int, int, int);
extern void prepare_output_line(racetype *, racetype *, int, int);

void block(int playernum, int governor, int apcount)
{
    int i;
    int n;
    int p;
    racetype *r;
    racetype *race;
    int dummy_;
    int dummy[2];

    n = Num_races;
    race = races[playernum - 1];

    if ((argn == 3) && match(args[1], "player")) {
        p = GetPlayer(args[2]);

        if (!p) {
            notify(playernum, governor, "No such player.\n");

            return;
        }

        r = race[p - 1];
        /* Used as flag for finding a block */
        dummy_ = 0;
        sprintf(buf, "Race #%d [%s] is a member of ", p, r->name);
        notify(playernum, governor, buf);

        for (i = 1; i <= n; ++i) {
            if (isset(Blocks[i - 1].pledge, p)
                && isset(Blocks[i - 1].invite, p)) {

                if (dummy_ == 0) {
                    sprintf(buf, " %d", i);
                } else {
                    sprintf(buf, ",%d", i);
                }

                notify(playernum, governor, buf);
                dummy_ = 1;
            }
        }

        if (dummy_ == 0) {
            notify(playernum, governor, "no blocks\n");
        } else {
            notify(playernum, governor, "\n");
        }

        /* Used for flag for finding a block */
        sprintf(buf, "Race #%d [%s] has been invited to join ", p, r->name);
        notify(playernum, governor, buf);

        for (i = 1; i <= n; ++i) {
            if(!isset(Blocks[i - 1].pledge, p)
               && isset(Blocks[i - 1].invite, p)) {
                if (dummy_ == 0) {
                    sprintf(buf, " %d", i);
                } else {
                    sprintf(buf, ",%d", i);
                }

                notify(playernum, governor, buf);
                dummy_ = 1;
            }
        }

        if (dummy_ == 0) {
            notify(playernum, governor, "no blocks\n");
        } else {
            notify(playernum, governor, "\n");
        }

        /* Used as flag for finding block */
        dummy_ = 0;
        sprintf(buf, "Race #%d [%s] has pledged ", p, r->name);
        notify(playernum, governor, buf);

        for (i = 1; i <= n; ++i) {
            if (isset(Blocks[i - 1].pledge, p)
                && !isset(Blocks[i - 1].invite, p)) {
                if (dummy_ == 0) {
                    sprintf(buf, " %d", i);
                } else {
                    sprintf(buf, ",%d", i);
                }

                notify(playernum, governor, buf);
                dummy_ = 1;
            }
        }

        if (!dummy_) {
            notify(playernum, governor, "no blocks\n");
        } else {
            notify(playernum, governor, "\n");
        }
    } else if (argn > 1) {
        p = GetPlayer(args[1]);

        if (!p) {
            notify(playernum, governor, "No such player.\n");

            return;
        }

        r = races[p - 1];

        /* List the player who are in this alliance block */
        dummy[0] = Blocks[p - 1].invite[0] & Blocks[p - 1].pledge[0];
        dummy[1] = Blocks[p - 1].invite[1] & Blocks[p - 1].pledge[1];
        sprintf(buf,
                "         ========== %s Power Report ============\n",
                Blocks[p - 1].name);

        notify(playernum, governor, buf);
        sprintf(buf,
                "                     %-64.04s\n",
                Blocks[p - 1].motto);

        notify(playernum, governor, buf);
        sprintf(buf,
                "  #  Name              troops  pop  money ship  plan  res fuel dest know\n");

        notify(playernum, governor, buf);

        for (i = 1; i <= n; ++i) {
            if (isset(dummy, i)) {
                r = races[i - 1];

                if (!r->dissolved) {
                    sprintf(buf, "%2d %-20.20s ", i, r->name);
                    sprintf(temp,
                            "%5s",
                            Estimate_i((int)Power[i - 1].troops, race, i));

                    strcat(buf, temp);
                    sprintf(temp,
                            "%5s",
                            Estimate_i((int)Power[i - 1].popn, race, i));

                    strcat(buf, temp);
                    sprintf(temp,
                            "%5s",
                            Estimate_i((int)Power[i - 1].money, race, i));

                    strcat(buf, temp);
                    sprintf(temp,
                            "%5s",
                            Estimate_i((int)Power[i - 1].ship_owned, race, i));

                    strcat(buf, temp);
                    sprintf(temp,
                            "%5s",
                            Estimate_i((int)Power[i - 1].planets_owned, race, i));

                    strcat(buf, temp);
                    sprintf(temp,
                            "%5s",
                            Estimate_i((int)Power[i - 1].resource, race, i));

                    strcat(buf, temp);
                    sprintf(temp,
                            "%5s",
                            Estimate_i((int)Power[i - 1].fuel, race, i));

                    strcat(buf, temp);
                    sprintf(temp,
                            "%5s",
                            Estimate_i((int)Power[i - 1].destruct, race, i));

                    strcat(buf, temp);
                    sprintf(temp, " %3d%%\n", race->translate[i - 1]);
                    strcat(buf, temp);
                    notify(playernum, governor, buf);
                }
            }
        }
    } else {
        /*
         * List power report for all the alliance blocks (as of the last update)
         */

        sprintf(buf, "                    ========== Alliance Blocks ==========\n");
        notify(playernum, governor, buf);
        sprintf(buf,
                " # Name             memb money popn ship  sys  res fuel dest  VPs know\n");

        notify(playernum, governor, buf);

        for (i = 1; i <= n; ++i) {
            if (Power_blocks.members[i - 1] > 1) {
                sprintf(buf,
                        "%2d %-19.19s%3ld",
                        i,
                        Blocks[i - 1].name,
                        Power_blocks.members[i - 1]);

                sprintf(temp,
                        "%5s",
                        Estimate_i((int)Power_blocks.money[i - 1], race, i));

                strcat(buf, temp);
                sprintf(temp,
                        "%5s",
                        Estimate_i((int)Power_blocks.popn[i - 1], race, i));

                strcat(buf, temp);
                sprintf(temp,
                        "%5s",
                        Estimate_i((int)Power_blocks.ships_owned[i - 1], race, i));

                strcat(buf, temp);
                sprintf(temp,
                        "%5s",
                        Estimate_i((int)Power_blocks.systems_owned[i - 1], race, i));

                strcat(buf, temp);
                sprintf(temp,
                        "%5s",
                        Estimate_i((int)Power_blocks.resource[i - 1], race, i));

                strcat(buf, temp);
                sprintf(temp,
                        "%5s",
                        Estimate_i((int)Power_blocks.fuel[i - 1], race, i));

                strcat(buf, temp);
                sprintf(temp,
                        "%5s",
                        Estimate_i((int)Power_blocks.destruct[i - 1], race, i));

                strcat(buf, temp);
                sprintf(temp,
                        "%5s",
                        Estimate_i((int)Power_blocks.VPs[i - 1], race, i));

                strcat(buf, temp);
                sprintf(temp, " %3s%%\n", race->translate[i - 1]);
                strcat(buf, temp);
                notify(playernum, governor, buf);
            }
        }
    }
}

void power(int playernum, int governor, int apcount)
{
    int i;
    int n;
    int p;
    racetype *r;
    racetype *race;
    struct vic vic[MAXPLAYERS];

    n = Num_races;
    p = -1;

    if (argn >= 2) {
        p = GetPlayer(args[1]);

        if (!p) {
            notify(playernum, governor, "No such player.\n");

            return;
        }

        r = races[p - 1];
    }

    race = races[playernum - 1];

    sprintf(buf,
            "         ========== Galactic Bloodshed Power Report ==========\n");

    notify(playernum, governor, buf);

    if (race->god) {
#ifdef USE_VN
        sprintf(buf,
                "%s  #  Name          VP  mil  civ cash ship pl  res fuel dest morl  VNhl\n",
                argn < 2 ? "rank" : "");

#else
        sprintf(buf,
                "%s  #  Name          VP  mil  civ cash ship pl  res fuel dest morl\n",
                argn < 2 ? "rank" : "");
#endif
    } else {
        sprintf(buf,
                "%s  #  Name          VP  mil  civ cash ship pl  res fuel dest morl know\n",
                argn < 2 ? "rank" : "");
    }

    notify(playernum, governor, buf);

    if (argn < 2) {
        create_victory_list(vic);

        for (i = 1; i <= n; ++i) {
            p = vic[i - 1].racenum;
            r = races[p - 1];

            if (!r->dissolved
                && (race->translate[p - 1] >= 10)
                && !r->God
                && !r->Guest) {
                prepare_output_line(race, r, p, i);
                notify(playernum, governor, buf);
            }
        }
    } else {
        r = races[p - 1];
        prepare_output_line(race, r, p, 0);
        notify(playernum, governor, buf);
    }
}

void prepare_output_line(racetype *race, racetype *r, int i, int rank)
{
    if (rank) {
        sprintf(buf, "%2d ", rank);
    } else {
        buf[0] = '\0';
    }

    sprintf(temp,
            "[%2d]%s%s%-10.10s %5s",
            i,
            isset(race->allied, i) ? "+"
            : isset(race->atwar, i) ? "-" : " ",
            isset(r->allied, race->playernum) ? "+"
            : isset(r->atwar, race->playernum) ? "-" : " ",
            r->name,
            Estimate_i((int)r->victory_score, race, i));

    strcat(buf, temp);
    sprintf(temp, "%5s", Estimate_i((int)Power[i - 1].troops, race, i));
    strcat(buf, temp);
    sprintf(temp, "%5s", Estimate_i((int)Power[i - 1].popn, race, i));
    strcat(buf, temp);
    sprintf(temp, "%5s", Estimate_i((int)Power[i - 1].money, race, i));
    strcat(buf, temp);
    sprintf(temp, "%5s", Estimate_i((int)Power[i - 1].ships_owned, race, i));
    strcat(buf, temp);
    sprintf(temp, "%5s", Estimate_i((int)Power[i - 1].planets_owned, race, i));
    strcat(buf, temp);
    sprintf(temp, "%5s", Estimate_i((int)Power[i - 1].resource, race, i));
    strcat(buf, temp);
    sprintf(temp, "%5s", Estimate_i((int)Power[i - 1].fuel, race, i));
    strcat(buf, temp);
    sprintf(temp, "%5s", Estimate_i((int)Power[i - 1].destruct, race, i));
    strcat(buf, temp);
    sprintf(temp, "%5s", Estimate_i((int)r->morale, race, i));
    strcat(buf, temp);

#ifdef USE_VN
    if (race->God) {
        sprintf(temp, " %4d", Sdata.VN_hitlist[i - 1]);
        strcat(buf, temp);
    }
#endif

    if (!race->God) {
        sprintf(temp, " %3d%%\n", race->translate[i - 1]);
        strcat(buf, temp);
    } else {
        strcat(buf, "\n");
    }
}
