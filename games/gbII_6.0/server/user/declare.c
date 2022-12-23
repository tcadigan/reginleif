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
 * Franklin Street, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * declare.c -- Declare alliance, neutrality, war, the basic things.
 *
 * #ident  "@(#)declare.c       1.9 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/declare.c,v 1.4 2007/07/06 18:06:56 gbp Exp $
 */
#include "declare.h"

#include <ctype.h>

#include "../server/buffers.h"
#include "../server/files_shl.h"
#include "../server/first.h"
#include "../server/GB_server.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/rand.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/shlmisc.h"
#include "../server/vars.h"

#include "tele.h"

void show_votes(int, int);

/* Invite people to join your alliance block */
void invite(int playernum, int governor, int apcount, int mode)
{
    int n;
    racetype *race;
    racetype *alien;

    n = GetPlayer(args[1]);

    if (!n) {
        sprintf(buf, "No such player.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (n == playernum) {
        notify(playernum, governor, "No needed, you are the leader.\n");

        return;
    }

    race = races[playernum - 1];
    alien = races[n - 1];

    if (mode) {
        setbit(Blocks[playernum - 1].invite, n);

        sprintf(buf,
                "%s [%d] has invited you to join %s\n",
                race->name,
                playernum,
                Blocks[playernum - 1].name);

        warn_race(n, buf);

        sprintf(buf,
                "%s [%d] has been invited to join %s [%d]\n",
                alien->name,
                n,
                Blocks[playernum - 1].name,
                playernum);

        warn_race(playernum, buf);
    } else {
        clrbit(Blocks[playernum - 1].invite, n);

        sprintf(buf,
                "You have been blackballed from %s [%d]\n",
                Blocks[playernum - 1].name,
                playernum);

        warn_race(n, buf);

        sprintf(buf,
                "%s [%d] has been blackballed from %s [%d]\n",
                alien->name,
                n,
                Blocks[playernum - 1].name,
                playernum);

        warn_race(playernum, buf);
    }

    post(buf, DECLARATION);
    Putblock(Blocks);
}

/* Declare that you wish to be included in the alliance block */
void pledge(int playernum, int governor, int apcount, int mode)
{
    int n;
    racetype *race;

    n = GetPlayer(args[1]);

    if (n) {
        sprintf(buf, "No such player.\n");
        notify(playernum, governor, buf);

        return;
    }

    if (n == playernum) {
        notify(playernum, governor, "Not needed, you are the leader.\n");

        return;
    }

    race = races[playernum - 1];

    if (mode) {
        setbit(Blocks[n - 1].pledge, playernum);
        sprintf(buf,
                "%s [%d] has pledged %s.\n",
                race->name,
                playernum,
                Blocks[n - 1].name);

        warn_race(n, buf);
        sprintf(buf,
                "You have pledged allegiance to %s.\n",
                Blocks[n - 1].name);

        warn_race(playernum, buf);

        switch (int_rand(1, 20)) {
        case 1:
            sprintf(buf,
                    "%s [%d] joins the band wagon and pledges allegiance to %s [%d]!\n",
                    race->name,
                    playernum,
                    Blocks[n - 1].name,
                    n);

            break;
        default:
            sprintf(buf,
                    "%s [%d] pledges allegiance to %s [%d].\n",
                    race->name,
                    playernum,
                    Blocks[n - 1].name,
                    n);

            break;
        }
    } else {
        clrbit(Blocks[n - 1].pledge, playernum);
        sprintf(buf,
                "%s [%d] has quit %s [%d].\n",
                race->name,
                playernum,
                Blocks[n - 1].name,
                n);

        warn_race(n, buf);
        sprintf(buf, "You have quit %s\n", Blocks[n - 1].name);
        warn_race(playernum, buf);

        switch (int_rand(1, 20)) {
        case 1:
            sprintf(buf,
                    "%s [%d] calls %s [%d] a bunch of geeks and QUITS!\n",
                    race->name,
                    playernum,
                    Blocks[n - 1].name,
                    n);

            break;
        default:
            sprintf(buf,
                    "%s [%d] has QUIT %s [%d]!\n",
                    race->name,
                    playernum,
                    Blocks[n - 1].name,
                    n);

            break;
        }
    }

    post(buf, DECLARATION);
    compute_power_blocks();
    Putblock(Blocks);
}

void declare(int playernum, int governor, int apcount)
{
    int n;
    int d_mod;
    racetype *race;
    racetype *alien;

    n = GetPlayer(args[1]);

    if (!n) {
        sprintf(buf, "No such player.\n");
        notify(playernum, governor, buf);

        return;
    }

    /* Look in sdata for APs first, enufAPs would print something */
    if ((int)Sdata.AP[playernum - 1] >= apcount) {
        deductAPs(playernum, governor, apcount, 0, 1);
        /* Otherwise use current star */
    } else if (((Dir[playernum - 1][governor].level == LEVEL_STAR)
                || (Dir[playernum - 1][governor].level == LEVEL_PLAN))
               && enufAP(playernum, governor, Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1], apcount)) {
        deductAPs(playernum,
                  governor,
                  apcount,
                  Dir[playernum - 1][governor].snum,
                  0);
    } else {
        sprintf(buf, "You don't have enough APs (%d)\n", apcount);
        notify(playernum, governor, buf);

        return;
    }

    race = races[playernum - 1];
    alien = races[n - 1];

    if (alien->Guest) {
        notify(playernum,
               governor,
               "You may not change your relationship with a Guest race.\n");

        return;
    }

    switch (*args[2]) {
    case 'a':
        setbit(race->allied, n);
        clrbit(race->atwar, n);

        if (success(5)) {
            sprintf(buf, "But would you want your sister to marry one?\n");
            notify(playernum, governor, buf);
        } else {
            sprintf(buf, "Good for you.\n");
            notify(playernum, governor, buf);
        }

        sprintf(buf,
                " Player #%d (%s) has declared an alliance with you!\n",
                playernum,
                race->name);

        warn_race(n, buf);
        sprintf(buf,
                "%s [%d] declares ALLIANCE with %s [%d].\n",
                race->name,
                playernum,
                alien->name,
                n);

        d_mod = DECLARE_KNOW_GAIN;

        if (argn > 3) {
            sscanf(args[3], "%d", &d_mod);
        }

        d_mod = MAX(d_mod, DECLARE_KNOW_GAIN);

        break;
    case 'n':
        clrbit(race->allied, n);
        clrbit(race->atwar, n);
        sprintf(buf, "Done.\n");
        notify(playernum, governor, buf);

        sprintf(buf,
                " Player #%d (%s) has declared neutrality with you!\n",
                playernum,
                race->name);

        warn_race(n, buf);

        sprintf(buf,
                "%s [%d] declares a state of neutrality with %s [%d].\n",
                race->name,
                playernum,
                alien->name,
                n);

        d_mod = DECLARE_KNOW_GAIN;

        break;
    case 'w':
        if (get_num_updates() < CombatUpdate) {
            sprintf(buf,
                    "You cannot declare ware until after the combat update, [%d]\n",
                    CombatUpdate);

            notify(playernum, governor, buf);

            return;
        }

        setbit(race->atwar, n);
        clrbit(race->allied, n);
        /* You allies can't be friendly with you */
        clrbit(alien->allied, playernum);

        if (success(4)) {
            sprintf(buf,
                    "You enemies flaunt their secondary male reproductive glands in your\ngeneral direction.\n");

            notify(playernum, governor, buf);
        } else {
            sprintf(buf, "Give 'em hell!\n");
            notify(playernum, governor, buf);
        }

        sprintf(buf,
                " Player #%d (%s) has declared war against you!\n",
                playernum,
                race->name);

        warn_race(n, buf);

        switch (int_rand(1,5)) {
        case 1:
            sprintf(buf,
                    "%s [%d] declares WAR on %s [%d].\n",
                    race->name,
                    playernum,
                    alien->name,
                    n);

            break;
        case 2:
            sprintf(buf,
                    "%s [%d] has had enough of %s [%d] and declares WAR!\n",
                    race->name,
                    playernum,
                    alien->name,
                    n);

            break;
        case 3:
            sprintf(buf,
                    "%s [%d] decided that it is time to declare WARE on %s [%d]!\n",
                    race->name,
                    playernum,
                    alien->name,
                    n);

            break;
        case 4:
            sprintf(buf,
                    "%s [%d] had no choice but to declare WAR against %s [%d]!\n",
                    race->name,
                    playernum,
                    alien->name,
                    n);

            break;
        case 5:
            sprintf(buf,
                    "%s [%d] says 'screw it!' and declares WAR on %s [%d]\n",
                    race->name,
                    playernum,
                    alien->name,
                    n);

            break;
        default:

            break;
        }

        d_mod = DECLARE_KNOW_GAIN;

        break;
    default:
        notify(playernum, governor, "I don't understand.\n");

        return;
    }

    post(buf, DECLARATION);
    warn_race(playernum, buf);

    /* They, of course, learn more about you */
    if (alien->translate[playernum - 1] < DECLARE_MAX_KNOW) {
        alien->translate[playernum - 1] = MIN(alien->translate[playernum - 1] + d_mod, DECLARE_MAX_KNOW);
    } else {
        alien->translate[playernum - 1] = alien->translate[playernum -1];
    }

    putrace(alien);
    putrace(race);
}

#ifdef VOTING
void vote (int playernum, int governor, int apcount)
{
    racetype *race;
    int check;
    int nvotes;
    int nays;
    int yays;

    race = races[playernum - 1];

    if (race->God) {
        sprintf(buf, "Your vote doesn't count, however, here is the count.\n");
        notify(playernum, governor, buf);
        show_votes(playernum, governor);

        return;
    }

    if (race->Guest) {
        sprintf(buf, "You are not allowed to vote, but, here is the count.\n");
        notify(playernum, governor, buf);
        show_votes(playernum, governor);

        return;
    }

    if (argn > 2) {
        check = 0;

        if (match(args[1], "update")) {
            if (match(args[2], "go")) {
                race->votes |= VOTE_UPDATE_GO;
                check = 1;
            } else if (match(args[2], "wait")) {
                race->votes &= ~VOTE_UPDATE_GO;
            } else {
                sprintf(buf, "No such update choice \'%s\'\n", args[2]);
                notify(playernum, governor, buf);

                return;
            }
        } else {
            sprintf(buf, "No such vote \'%s\'\n", args[1]);
            notify(playernum, governor, buf);

            return;
        }

        putrace(race);

        if (check) {
            /*
             * Ok...someone voted yes. Tally them all up and see if we should do
             * something.
             */
            nays = 0;
            yays = 0;
            nvotes = 0;

            for (playernum = 1; playernum <= Num_races; ++playernum) {
                race = races[playernum - 1];

                if (race->God || race->Guest || race->dissolved) {
                    continue;
                }

                ++nvotes;

                if (race->votes && VOTE_UPDATE_GO) {
                    ++yays;
                } else {
                    ++nays;
                }
            }

            /* Is Update/Movement vote unanimous now? */
            if ((nvotes > 0) && (nvotes == yays) && (nays == 0)) {
                /* Do it... */
                do_update(1);
            }
        }
    } else {
        if (race->votes && VOTE_UPDATE_GO) {
            sprintf(buf, "Your vote on updates is go\n");
        } else {
            sprintf(buf, "Your vote on updates is wait\n");
        }

        notify(playernum, governor, buf);
        show_votes(playernum, governor);
    }
}

void show_votes(int playernum, int governor)
{
    int nvotes;
    int nays;
    int yays;
    int pnum;
    racetype *race;

    nvotes = 0;
    yays = nvotes;
    nays = yays;

    for (pnum = 1; pnum < Num_races; ++pnum) {
        race = races[pnum - 1];

        if (race->God || race->Guest || race->dissolved) {
            continue;
        }

        ++nvotes;

        if (race->votes && VOTE_UPDATE_GO) {
            ++yays;
            sprintf(buf, "  %s voted go.\n", race->name);
        } else {
            ++nays;
            sprintf(buf, "  %s voted wait.\n", race->name);
        }

        if (races[playernum - 1]->God) {
            notify(playernum, governor, buf);
        }
    }

    sprintf(buf,
            "  Total votes = %d, Go = %d, Wait = %d.\n",
            nvotes,
            yays,
            nays);

    notify(playernum, governor, buf);
}

#endif
