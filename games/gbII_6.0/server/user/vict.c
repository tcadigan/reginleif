/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (C) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it nuder
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
 * Franklin street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 * *****************************************************************************
 *
 * vict.c
 *
 * Created:
 * Author: ??
 *
 * Version: 1.2 16:59:42
 *
 * Contains: victory()
 *           create_victory_list()
 *
 * #ident "@(#)vict.c 1.10 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/vict.c,v 1.3 2007/07/06 18:09:34 gbp Exp $
 * *****************************************************************************
 */
#include "vict.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../server/buffers.h"
#include "../server/GB_server.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/vars.h"

/* Prototypes */
void create_victory_list(struct vic[MAXPLAYERS]);

static int victory_sort(struct vic *, struct vic *);

/*
 * victory:
 *
 * arguments:
 *   playernum  Playernum who called this function
 *   governor   Governor who called this function
 *
 * called by:
 *   main
 *   check_logsize
 *
 * description: Called from process_commands
 */
void victory(int playernum, int governor)
{
    struct vic vic[MAXPLAYERS];
    racetype *race;
    int i;
    int count;
    int god = 0;

    if (argn > 1) {
        count = atoi(args[1]);
    } else {
        count = Num_races;
    }

    if (count > Num_races) {
        count = Num_races;
    }

    create_victory_list(vic);

    race = races[playernum - 1];

    if (race->God) {
        god = 1;
    }

    strcpy(buf, "----==== CURRENT PLAYER RANKINGS ====----\n");
    notify(playernum, governor, buf);

    sprintf(buf,
            "%-4.4s %-15.15s %8s\n",
            "No.",
            "Name",
            god ? "Score": "");

    notify(playernum, governor, buf);

    for (i = 0; i < count; ++i) {
        if (vic[i].no_count == 0) {
            if (god) {
                sprintf(buf,
                        "%2d %c [%2d] %-15.15s %5ld  %6.2f %3d %s %s\n",
                        i + 1,
                        vic[i].Thing ? 'M' : ' ',
                        vic[i].racenum,
                        vic[i].name,
                        vic[i].rawscore,
                        vic[i].tech,
                        vic[i].IQ,
                        races[vic[i].racenum - 1]->password,
                        races[vic[i].racenum - 1]->governor[0].password);
            } else {
                sprintf(buf,
                        "%2d   [%2d] %-15.15s\n",
                        i + 1,
                        vic[i].racenum,
                        vic[i].name);
            }

            notify(playernum, governor, buf);
        }
    }

#ifdef VICTORY
    sprintf(buf,
            "\nVICTORY CONDITIONS: Ownership of %d%% of galaxy (%d planets) for %d updates.\n",
            VICTORY_PERCENT,
            (int)(VICTORY_PERCENT * Planet_count) / 100,
            VICTORY_UPDATES);

#else

    sprintf(buf, "VICTORY CONDITIONS: (not set)\n\n");
#endif

    notify(playernum, governor, buf);
}

/*
 * create_victory_list:
 *
 * arguments:
 *   vic  victory structure
 *
 * called by: victory (local to this file)
 *
 * description: Puts together and sorts the races
 */
void create_victory_list(struct vic vic[MAXPLAYERS])
{
    racetype *vic_races[MAXPLAYERS];
    int i;
    int (*func)(struct vic *, struct vic *);

    for (i = 1; i <= Num_races; ++i) {
        vic_races[i - 1] = races[i - 1];
        vic[i - 1].no_count = 0;
    }

    for (i = 1; i <= Num_races; ++i) {
        vic[i - 1].racenum = i;
        strcpy(vic[i - 1].name, vic_races[i - 1]->name);
        vic[i - 1].rawscore = vic_races[i - 1]->victory_score;
        /* vic[i - 1].rawscore = (long)(vic_races[i - 1]->morale + vic_races[i - 1]->victory_score); */
        vic[i - 1].tech = vic_races[i - 1]->tech;
        vic[i - 1].Thing = vic_races[i - 1]->Metamorph;
        vic[i - 1].IQ = vic_races[i - 1]->IQ;

        if (vic_races[i - 1]->God
            || vic_races[i - 1]->Guest
            || vic_races[i - 1]->dissolved) {
            vic[i - 1].no_count = 1;
        }
    }

    func = victory_sort;
    qsort(vic, Num_races, sizeof(struct vic), func);
}

/*
 * victory_sort:
 *
 * arguments:
 *   a  Victory structure
 *   b  Victory structure
 *
 * called by: create_victory_list (local to this file)
 *
 * description:
 */
static int victory_sort(struct vic *a, struct vic *b)
{
    if (a->no_count) {
        return 1;
    } else if (b->no_count) {
        return -1;
    } else {
        return (b->rawscore - a->rawscore);
    }
}
