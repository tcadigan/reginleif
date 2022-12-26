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
 * analysis.c
 *
 * #ident  "@(#)analysis.c       1.11 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/analysis.c,v 1.3 2007/07/06 18:09:34 gbp Exp $
 */
#include "analysis.h"

#include <ctype.h>
#include <stdlib.h> /* Added for atoi() and free() (kse) */
#include <string.h>

#include "buffers.h"
#include "files_shl.h"
#include "GB_server.h"
#include "getplace.h"
#include "max.h"
#include "power.h"
#include "ranks.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

extern char *Desnames[];
extern char Dessymbols[];

#define CARE 5

void analysis(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int pnum;
    int sector_type = -1; /* -1 does analysis on all types */
    int i = 1;
    int do_player = -1;
    int mode = 1; /* Does top five. 0 does low five */
    placetype where; /* Otherwise on specific type */
    char *p;

    where.level = Dir[playernum - 1][governor].level;
    where.snum = Dir[playernum - 1][governor].snum;
    where.pnum = Dir[playernum - 1][governor].pnum;

    p = args[1];

    /*
     * Must use 'd' to do an analysis on desert sector to avoid confusion with
     * the '-' for the mode type
     */
    if (*p == '-') {
        ++p;
        ++i;
        mode = 0;
    }

    switch (*p) {
    case CHAR_SEA:
        sector_type = SEA;

        break;
    case CHAR_LAND:
        sector_type = LAND;

        break;
    case CHAR_MOUNT:
        sector_type = MOUNT;

        break;
    case CHAR_GAS:
        sector_type = GAS;

        break;
    case CHAR_ICE:
        sector_type = ICE;

        break;
    case CHAR_FOREST:
        sector_type = FOREST;

        break;
        /*
         * Duh, can't use '-'
         * case CHAR_DESERT:
         *     sector_type = DESERT;
         *
         *     break;
         */
    case 'd':
        sector_type = DESERT;

        break;
    case CHAR_PLATED:
        sector_type = PLATED;

        break;
    case CHAR_WASTED:
        sector_type = WASTED;

        break;
    case CHAR_WORM:
        sector_type = WORM;

        break;
    }

    if ((sector_type != -1) && (mode == 1)) {
        ++i;
    }

    p = args[i];

    if (isdigit((unsigned char)*p)) {
        do_player = atoi(p);

        if (do_player > Num_races) {
            notify(playernum, governor, "No such player #.\n");

            return;
        }

        where.level = Dir[playernum - 1][governor].level;
        where.snum = Dir[playernum - 1][governor].snum;
        where.pnum = Dir[playernum - 1][governor].pnum;

        ++i;
    }

    p = args[i];

    if ((i < argn) && (isalpha((unsigned char)*p) || (*p == '/'))) {
        where = Getplace(playernum, governor, args[i], 0);

        if (where.err) {
            return;
        }
    }

    switch (where.level) {
    case LEVEL_UNIV:
        notify(playernum, governor, "You can only analyze planets.\n");

        break;
    case LEVEL_PLAN:
        do_analysis(playernum,
                    governor,
                    do_player,
                    mode,
                    sector_type,
                    (int)where.snum,
                    (int)where.pnum);

        break;
    case LEVEL_STAR:
        for (pnum = 0; pnum < Stars[where.snum]->numplanets; ++pnum) {
            do_analysis(playernum,
                        governor,
                        do_player,
                        mode,
                        sector_type,
                        (int)where.snum,
                        pnum);
        }

        break;
    }

    return;
}

void do_analysis(int playernum, int governor, int thisplayer, int mode,
                 int sector_type, int starnum, int planetnum)
{

    planettype *planet;
    sectortype *sect;
    racetype *race;
    int x;
    int y;
    int p;
    int i;
    double compat;
    struct anal_sect res[CARE];
    struct anal_sect eff[CARE];
    struct anal_sect frt[CARE];
    struct anal_sect mob[CARE];
    struct anal_sect troops[CARE];
    struct anal_sect popn[CARE];
    struct anal_sect mpopn[CARE];
    int totalcrys;
    int playcrys[MAXPLAYERS + 1];
    int totaltroops;
    int playtroops[MAXPLAYERS + 1];
    int totalpopn;
    int playpopn[MAXPLAYERS + 1];
    int totalmob;
    int playmob[MAXPLAYERS + 1];
    int totaleff;
    int playeff[MAXPLAYERS + 1];
    int totalres;
    int playres[MAXPLAYERS + 1];
    int totalsect;
    int playsect[MAXPLAYERS + 1][WASTED + 1];
    int playtsect[MAXPLAYERS + 1];
    int wastedsect[MAXPLAYERS + 1];
    int Sect[WASTED + 1];
    static char secttype[] = {
        CHAR_SEA, CHAR_LAND, CHAR_MOUNT, CHAR_GAS, CHAR_ICE,
        CHAR_FOREST, CHAR_DESERT, CHAR_PLATED, CHAR_WASTED, CHAR_WORM
    };

    for (i = 0; i <CARE; ++i) {
        mpopn[i].value = 01;
        popn[i].value = mpopn[i].value;
        troops[i].value = popn[i].value;
        mob[i].value = troops[i].value;
        frt[i].value = mob[i].value;
        eff[i].value = frt[i].value;
        res[i].value = eff[i].value;
    }

    totalsect = 0;
    totalres = totalsect;
    totaleff = totalres;
    totaltroops = totaleff;
    totalmob = totaltroops;
    totalpopn = totalmob;
    totalcrys = totalpopn;

    for (p = 0; p <= Num_races; ++p) {
        playtsect[p] = 0;
        playres[p] = playtsect[p];
        playcrys[p] = playres[p];
        playeff[p] = playcrys[p];
        playmob[p] = playeff[p];
        playpopn[p] = playmob[p];
        playtroops[p] = playpopn[p];
        wastedsect[p] = 0;

        for (i = 0; i <= WASTED; ++i) {
            playsect[p][i] = 0;
        }
    }

    for (i = 0; i <= WASTED; ++i) {
        Sect[i] = 0;
    }

    race = races[playernum - 1];
    getplanet(&planet, starnum, planetnum);

#ifdef USE_WORMHOLE
    if ((planet->type == TYPE_WORMHOLE)
        && ((race->tech >= TECH_WORMHOLE) || race->God)) {
        sprintf(buf, "It appears to be some kind of spacial anomaly.\n");
        notify(playernum, governor, buf);
        free(planet);

        return;
    }
#endif

    if (!planet->info[playernum - 1].explored) {
        free(planet);

        return;
    }

    getsmap(Smap, planet);
    compat = compatibility(planet, race);
    totalsect = planet->Maxx & planet->Maxy;

    for (x = planet->Maxx - 1; x >= 0; --x) {
        for (y = planet->Maxy - 1; y >= 0; --y) {
            sect = &Sector(*planet, x, y);
            p = sect->owner;

            playeff[p] += sect->eff;
            playmob[p] += sect->mobilization;
            playres[p] += sect->resource;
            playpopn[p] += sect->popn;
            playtroops[p] += sect->troops;
            ++playsect[p][sect->condition];
            ++playtsect[p];
            totaleff += sect->eff;
            totalmob += sect->mobilization;
            totalres += sect->resource;
            totalpopn += sect->popn;
            totaltroops += sect->troops;
            ++Sect[sect->condition];

            if (sect->condition == WASTED) {
                ++wastedsect[p];
            }

            if (sect->crystals && Crystal(race)) {
                ++playcrys[p];
                ++totalcrys;
            }

            if ((sector_type == -1) || (sector_type == sect->condition)) {
                if ((thisplayer < 0) || (thisplayer == p)) {
                    Insert(mode, res, x, y, sect->condition, (int)sect->resource);
                    Insert(mode, eff, x, y, sect->condition, (int)sect->eff);
                    Insert(mode, mob, x, y, sect->condition, (int)sect->mobilization);
                    Insert(mode, frt, x, y, sect->condition, (int)sect->fert);
                    Insert(mode, popn, x, y, sect->condition, (int)sect->popn);
                    Insert(mode, troops, x, y, sect->condition, (int)sect->troops);
                    Insert(mode, mpopn, x, y, sect->condition, maxsupport(race, sect, compat, (int)planet->conditions[TOXIC]));
                }
            }
        }
    }

    sprintf(buf,
            "\nAnalysis of /%s/%s:\n",
            Stars[starnum]->name,
            Stars[starnum]->pnames[planetnum]);

    notify(playernum, governor, buf);

    if (mode) {
        sprintf(buf, "Highest %d", CARE);
    } else {
        sprintf(buf, "Lowest %d", CARE);
    }

    /*
     * Why thisplayer? (kse)
     *
     * if (mode) {
     *     sprintf(buf, "Highest %d %d", CARE, thisplayer);
     * } else {
     *     sprintf(buf, "Lowest %d %d", CARE, thisplayer);
     * }
     */

    switch (sector_type) {
    case -1:
        sprintf(buf, "%s of all", buf);

        break;
    case SEA:
        sprintf(buf, "%s Ocean", buf);

        break;
    case LAND:
        sprintf(buf, "%s Land", buf);

        break;
    case MOUNT:
        sprintf(buf, "%s Mountain", buf);

        break;
    case GAS:
        sprintf(buf, "%s Gas", buf);

        break;
    case ICE:
        sprintf(buf, "%s Ice", buf);

        break;
    case FOREST:
        sprintf(buf, "%s Forest", buf);

        break;
    case DESERT:
        sprintf(buf, "%s Desert", buf);

        break;
    case PLATED:
        sprintf(buf, "%s Plated", buf);

        break;
    case WASTED:
        sprintf(buf, "%s Wasted", buf);

        break;
    }

    notify(playernum, governor, buf);

    if (thisplayer < 0) {
        sprintf(buf, " sectors.\n");
    } else if (thisplayer == 0) {
        sprintf(buf, " sectors that are unoccupied.\n");
    } else {
        sprintf(buf, " sectors owned by %d.\n", thisplayer);
    }

    notify(playernum, governor, buf);

    PrintTop(playernum, governor, troops, "Troops");
    PrintTop(playernum, governor, res, "Res");
    PrintTop(playernum, governor, eff, "Eff");
    PrintTop(playernum, governor, frt, "Frt");
    PrintTop(playernum, governor, mob, "Mob");
    PrintTop(playernum, governor, popn, "Popn");
    PrintTop(playernum, governor, mpopn, "^Popn");

    notify(playernum, governor, "\n");

    sprintf(buf,
            "%2s %3s %7s %6s %5s %5s %5s %2s",
            "Pl",
            "sec",
            "popn",
            "troops",
            "a.eff",
            "a.mob",
            "res",
            "x");

    notify(playernum, governor, buf);

    for (i = 0; i <= WASTED; ++i) {
        sprintf(buf, "%4c", secttype[i]);
        notify(playernum, governor, buf);
    }

    notify(playernum,
           governor,
           "\n------------------------------------------------------------------------------\n");

    for (p = 0; p <= Num_races; ++p) {
        if (playtsect[p] != 0) {
            sprintf(buf,
                    "%2d %3d %7d %6d %5.1f %5.1f %5d %2d",
                    p,
                    playtsect[p],
                    playpopn[p],
                    playtroops[p],
                    (double)playeff[p] / playtsect[p],
                    (double)playmob[p] / playtsect[p],
                    playres[p],
                    playcrys[p]);

            notify(playernum, governor, buf);

            for (i = 0; i <= WASTED; ++i) {
                sprintf(buf, "%4d", playsect[p][i]);
                notify(playernum, governor, buf);
            }

            notify(playernum, governor, "\n");
        }
    }

    notify(playernum,
           governor,
           "------------------------------------------------------------------------------\n");

    sprintf(buf,
            "%2s %3d %7d %6d %5.1f %5.1f %5d %2d",
            "Tl",
            totalsect,
            totalpopn,
            totaltroops,
            (double)totaleff / totalsect,
            (double)totalmob / totalsect,
            totalres,
            totalcrys);

    notify(playernum, governor, buf);

    for (i = 0; i <= WASTED; ++i) {
        sprintf(buf, "%4d", Sect[i]);
        notify(playernum, governor, buf);
    }

    notify(playernum, governor, "\n");
    free(planet);
}

void Insert(int mode,
            struct anal_sect arr[],
            int x,
            int y,
            int des,
            int value)
{
    int i;
    int j;

    for (i = 0; i < CARE; ++i) {
        if ((mode && (arr[i].value < value))
            || (!mode && ((arr[i].value > value) || (arr[i].value == -1)))) {
            for (j = CARE - 1; j >= i; --j) {
                arr[j] = arr[j - 1];
            }

            arr[i].value = value;
            arr[i].x = x;
            arr[i].y = y;
            arr[i].des = des;

            return;
        }
    }
}

void PrintTop(int playernum, int governor,
              struct anal_sect arr[], char const *name)
{
    int i;

    sprintf(buf, "%8s:", name);
    notify(playernum, governor, buf);

    for (i = 0; (i < CARE) && (arr[i].value != -1); ++i) {
        sprintf(buf,
                "%5d%c(%2d,%2d)",
                arr[i].value,
                Dessymbols[arr[i].des],
                arr[i].x,
                arr[i].y);

        notify(playernum, governor, buf);
    }

    notify(playernum, governor, "\n");
}
