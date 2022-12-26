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
 * Franklin Street, 5th Floot, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * survey.c -- Print out survey for planets
 *
 * Tue Apr 16:34:38 MDT 1991 (Evan Koffler)
 *   Added the client_survey and mode parts
 * Thu Apr 25 11:37:56 MDT 1991
 *   Added the ships per sector stuff. Works well.
 *
 * #ident  "@(#)survey.c        1.13 12/9/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/survey.c,v 1.4 2007/06 18:09:34 gbp Exp $
 */
#include "survey.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "csp.h"
#include "csp_types.h"
#include "files_shl.h"
#include "GB_server.h"
#include "get4args.h"
#include "getplace.h"
#include "max.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "shlmisc.h"
#include "vars.h"

#include "fire.h"
#include "map.h"
#include "mobiliz.h"

#define MAX_SHIPS_PER_SECTOR 10

extern char *Desnames[];
extern char Dessymbols[];

static char const *Tox[] = {
    "Stage 0, mild",
    "Stage 1, mild",
    "Stage 2, semi-mild",
    "Stage 3, semi-semi mild",
    "Stage 4, ecologically unsound",
    "Stage 5, ecologically unsound",
    "Stage 6, below birth threshold",
    "Stage 7, ecologically unstable--below birth threshold",
    "Stage 8, ecologically poisonous--below birth threshold",
    "Stage 9, WARNING: nearing 100% toxicity",
    "Stage 10: WARNING: COMPLETELY TOXIC!!!",
    "???"
};

void survey(int playernum, int governor, int apcount, int mode, orbitinfo *unused5)
{
    int lowx;
    int hix;
    int lowy;
    int hiy;
    int x2;
    int tindex;
    int avg_fert;
    int avg_resource;
    int crystal_count;
    int all = 0; /* Full survey 1, specific 0 */
    int inhere = -1;
    int shiplist;
    int i;
    char d;
    char sect_char;
    double compat;
    sectortype *s;
    planettype *p;
    placetype where;
    racetype *race;
    shiptype *shipa;

    struct numshipstuff {
        int pos;

        struct shipstuff {
            int shipno;
            char ltr;
            unsigned char owner;
        } shipstuffs[MAX_SHIPS_PER_SECTOR];
    };

    struct numshipstuff shiplocs[MAX_X][MAX_Y];

    /* General code -- jpd -- */
    if (!(governor == 0 ? 1 : races[playernum - 1]->governor[governor].rank <= RANK_SURVEY ? 1 : 0)) {
        no_permission(playernum, governor, "survey", RANK_SURVEY);

        return;
    }
    /* General code -- jpd --- */

    if (argn == 1) {
        /* No args */
        where.level = Dir[playernum - 1][governor].level;
        where.snum = Dir[playernum - 1][governor].snum;
        where.pnum = Dir[playernum - 1][governor].pnum;
    } else {
        /* They are surveying a sector */
        if (((isdigit((unsigned char)args[1][0])
              && (index(args[1], ',') != NULL)))
            || (*args[1] == '-')) {
            all = 1;

            if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
                sprintf(buf, "There are no sectors here.\n");
                notify(playernum, governor, buf);

                return;
            } else {
                where.level = LEVEL_PLAN;
                where.snum = Dir[playernum - 1][governor].snum;
                where.pnum = Dir[playernum - 1][governor].pnum;
            }
        } else {
            where = Getplace(playernum, governor, args[1], 0);

            if (where.err || (where.level == LEVEL_SHIP)) {
                return;
            }
        }
    }

    race = races[playernum - 1];

    if (where.level == LEVEL_PLAN) {
        getplanet(&p, (int)where.snum, (int)where.pnum);
        compat = compatibility(p, race);

        if ((isdigit((unsigned char)args[1][0])
             && (index(args[1], ',') != NULL))
            || all) {
            getsmap(Smap, p);

            if (!all) {
                get4args(args[1], &x2, &hix, &lowy, &hiy);
                /* ^^^ Translate from lowx:hix, lowy:hiy */
                x2 = MAX(0, x2);
                hix = MIN(hix, p->Maxx - 1);
                lowy = MAX(0, lowy);
                hiy = MIN(hiy, p->Maxy - 1);
            } else {
                x2 = 0;
                hix = p->Maxx - 1;
                lowy = 0;
                hiy = p->Maxy - 1;
            }

            if (!mode) {
                sprintf(buf,
                        " x,y cond/type owner race eff mob frt  res  mil popn ^popn xtals\n");

                notify(playernum, governor, buf);
            }

            if (mode) {
                if (all) {
                    sprintf(buf,
                            "%c %d %d %d %s %s %d %d %d %ld %ld %d %.2f %d\n",
                            CSP_CLIENT,
                            CSP_SURVEY_INTRO,
                            p->Maxx,
                            p->Maxy,
                            Stars[where.snum]->name,
                            Stars[where.snum]->pnames[where.pnum],
                            p->info[playernum - 1].resource,
                            p->info[playernum - 1].fuel,
                            p->info[playernum - 1].destruct,
                            p->popn,
                            p->maxpopn,
                            p->conditions[TOXIC],
                            compatibility(p, race),
                            p->slaved_to);

                    notify(playernum, governor, buf);
                }

                memset((struct shipstuff *)shiplocs, 0, sizeof(shiplocs));
                inhere = p->info[playernum - 1].numsectsowned;
                shiplist = p->ships;

                while (shiplist) {
                    getship(&shipa, shiplist);

                    if ((shipa->owner == playernum)
                        && (shipa->popn || (shipa->type == OTYPE_PROBE))) {
                        inhere = 1;
                    }

                    if (shipa->alive
                        && landed(shipa)
                        && (shiplocs[shipa->land_x][shipa->land_y].pos < MAX_SHIPS_PER_SECTOR)) {
                        shiplocs[shipa->land_x][shipa->land_y].shipstuffs[shiplocs[shipa->land_x][shipa->land_y].pos].shipno = shiplist;

                        shiplocs[shipa->land_x][shipa->land_y].shipstuffs[shiplocs[shipa->land_x][shipa->land_y].pos].owner = shipa->owner;

                        shiplocs[shipa->land_x][shipa->land_y].shipstuffs[shiplocs[shipa->land_x][shipa->land_y].pos].ltr = Shipltrs[shipa->type];

                        ++shiplocs[shipa->land_x][shipa->land_y].pos;
                    }

                    shiplist = nextship(shipa);
                    free(shipa);
                }
            }

            while (lowy <= hiy) {
                for (lowx = x2; lowx <= hix; ++lowx) {
                    s = &Sector(*p, lowx, lowy);

                    /* if (s->owner == playernum) { */
                    if (!mode) {
                        sprintf(buf, "%2d,%-2d ", lowx, lowy);
                        notify(playernum, governor, buf);
                        d = desshow(playernum, governor, p, lowx, lowy, race);

                        if (d == CHAR_CLOAKED) {
                            sprintf(buf, "?  (    ?    )\n");
                            notify(playernum, governor, buf);
                        } else {
                            sprintf(buf,
                                    " %c   %c  %6u%5u%4u%4u%4u%5u%5u%5d%6d%s\n",
                                    Dessymbols[s->condition],
                                    Dessymbols[s->type],
                                    s->owner,
                                    s->race,
                                    s->eff,
                                    s->mobilization,
                                    s->fert,
                                    s->resource,
                                    s->troops,
                                    s->popn,
                                    maxsupport(race, s, compat, p->conditions[TOXIC]),
                                    s->crystals && (race->discoveries[D_CRYSTAL] || race->God) ? " yes": " ");

                            notify(playernum, governor, buf);
                        }
                    } else {
                        /* Mode */
                        switch (s->condition) {
                        case SEA:
                            sect_char = CHAR_SEA;

                            break;
                        case LAND:
                            sect_char = CHAR_LAND;

                            break;
                        case MOUNT:
                            sect_char = CHAR_MOUNT;

                            break;
                        case GAS:
                            sect_char = CHAR_GAS;

                            break;
                        case PLATED:
                            sect_char = CHAR_PLATED;

                            break;
                        case ICE:
                            sect_char = CHAR_ICE;

                            break;
                        case DESERT:
                            sect_char = CHAR_DESERT;

                            break;
                        case FOREST:
                            sect_char = CHAR_FOREST;

                            break;
                        case WORM:
                            sect_char = CHAR_WORM;

                            break;
                        default:
                            sect_char = '?';

                            break;
                        }

                        sprintf(buf,
                                "%c %d %d %d %c %c %d %u %u %u %u %d %u %u %u %d",
                                CSP_CLIENT,
                                CSP_SURVEY_SECTOR,
                                lowx,
                                lowy,
                                sect_char,
                                desshow(playernum, governor, p, lowx, lowy, race),
                                s->condition == WASTED ? 1 : 0,
                                s->owner,
                                s->eff,
                                s->fert,
                                s->mobilization,
                                s->crystals && (race->discoveries[D_CRYSTAL] || race->God) ? 1 : 0,
                                s->resource,
                                s->popn,
                                s->troops,
                                maxsupport(race, s, compat, p->conditions[TOXIC]));

                        notify(playernum, governor, buf);

                        if (shiplocs[lowx][lowy].pos && inhere) {
                            notify(playernum, governor, ";");

                            for (i = 0; i < shiplocs[lowx][lowy].pos; ++i) {
                                sprintf(buf,
                                        " %d %c %u;",
                                        shiplocs[lowx][lowy].shipstuffs[i].shipno,
                                        shiplocs[lowx][lowy].shipstuffs[i].ltr,
                                        shiplocs[lowx][lowy].shipstuffs[i].owner);

                                notify(playernum, governor, buf);
                            }
                        }

                        notify(playernum, governor, "\n");
                    }
                }

                ++lowy;
            }

            if (mode) {
                sprintf(buf, "%c %d\n", CSP_CLIENT, CSP_SURVEY_END);
                notify(playernum, governor, buf);
            }
        } else {
            /* Survey of planet */
            sprintf(buf, "%s:\n", Stars[where.snum]->pnames[where.pnum]);
            notify(playernum, governor, buf);
            sprintf(buf,
                    "gravity   x,y absolute     x,y relative to %s\n",
                    Stars[where.snum]->name);

            notify(playernum, governor, buf);

            sprintf(buf,
                    "%7.2f   %7.1f,%7.1f   %8.1f,%81.f\n",
                    gravity(p),
                    p->xpos + Stars[where.snum]->xpos,
                    p->ypos + Stars[where.snum]->ypos,
                    p->xpos,
                    p->ypos);

            notify(playernum, governor, buf);

            if ((p->type != TYPE_WORMHOLE)
                || ((p->type == TYPE_WORMHOLE) && (race->tech < TECH_WORMHOLE))) {
                sprintf(buf, "======== Planetary conditions: ========\n");
                notify(playernum, governor, buf);
                sprintf(buf, "atmosphere concentrations:\n");
                notify(playernum, governor, buf);

                sprintf(buf,
                        "     methane %02d%%(%02d%%)     oxygen %02d%%(%02d%%)\n",
                        p->conditions[METHANE],
                        race->conditions[METHANE],
                        p->conditions[OXYGEN],
                        race->conditions[OXYGEN]);

                notify(playernum, governor, buf);

                sprintf(buf,
                        "         CO2 %02d%%(%02d%%)   hydrogen %02d%%(%02d%%)      temperature: %3d (%3d)\n",
                        p->conditions[CO2],
                        race->conditions[CO2],
                        p->conditions[HYDROGEN],
                        race->conditions[HYDROGEN],
                        p->conditions[TEMP],
                        race->conditions[TEMP]);

                notify(playernum, governor, buf);

                sprintf(buf,
                        "    nitrogen %02d%%(%02d%%)     sulfur %02d%%(%02d%%)           normal: %3d\n",
                        p->conditions[NITROGEN],
                        race->conditions[NITROGEN],
                        p->conditions[SULFUR],
                        race->conditions[SULFUR],
                        p->conditions[RTEMP]);

                notify(playernum, governor, buf);

                sprintf(buf,
                        "      helium %02d%%(%02d%%)      other %02d%%(%02d%%)\n",
                        p->conditions[HELIUM],
                        race->conditions[HELIUM],
                        p->conditions[OTHER],
                        race->conditions[OTHER]);

                notify(playernum, governor, buf);
                tindex = p->conditions[TOXIC] / 10;

                if (tindex < 0) {
                    tindex = 0;
                } else if (tindex > 10) {
                    tindex = 11;
                }

                sprintf(buf,
                        "                     Toxicity: %d %% (%s)\n",
                        p->conditions[TOXIC],
                        Tox[tindex]);

                notify(playernum, governor, buf);

                sprintf(buf,
                        "Total planetary compatibility: %.2f%%\n",
                        compatibility(p, race));

                notify(playernum, governor, buf);
                getsmap(Smap, p);
                avg_resource = 0;
                avg_fert = avg_resource;
                crystal_count = avg_fert;

                for (lowx = 0; lowx < p->Maxx; ++lowx) {
                    for (lowy = 0; lowy < p->Maxy; ++lowy) {
                        s = &Sector(*p, lowx, lowy);
                        avg_fert += s->fert;
                        avg_resource += s->resource;

                        if (race->discoveries[D_CRYSTAL] || race->God) {
                            if (s->crystals != 0) {
                                ++crystal_count;
                            }
                        }
                    }
                }

                sprintf(buf,
                        "%29s: %d\n%29s: %d\n%29s: %d\n",
                        "Average fertility",
                        avg_fert / (p->Maxx * p->Maxy),
                        "Average resource",
                        avg_resource / (p->Maxx * p->Maxy),
                        "Crystal sectors",
                        crystal_count);

                notify(playernum, governor, buf);

                if (LIMITED_RESOURCES) {
                    sprintf(buf,
                            "%29s: %ld\n",
                            "Total resource deposits",
                            p->total_resources);

                    notify(playernum, governor, buf);
                }

                sprintf(buf,
                        "fuel_stock  resource_stock dest_pot.   %s    ^%s\n",
                        race->Metamorph ? "biomass" : "popltn",
                        race->Metamorph ? "biomass" : "popltn");

                notify(playernum, governor, buf);

                sprintf(buf,
                        "%10u  %14u %9u  %7lu%11lu\n",
                        p->info[playernum - 1].fuel,
                        p->info[playernum - 1].resource,
                        p->info[playernum - 1].destruct,
                        p->popn,
                        p->maxpopn);

                notify(playernum, governor, buf);

                if (p->slaved_to) {
                    sprintf(buf,
                            "This planet ENSLAVED to player %d!\n",
                            p->slaved_to);

                    notify(playernum, governor, buf);
                }
            } else {
                /* Is worm hole */
                sprintf(buf,
                        "[Wormhole] Stability: %d%%\n",
                        Stars[where.snum]->wh_stability);

                notify(playernum, governor, buf);

                sprintf(buf,
                        "Destination: %s\n",
                        Stars[Stars[where.snum]->wh_dest_starnum]->name);

                notify(playernum, governor, buf);
            }
        }

        free(p);
    } else if (where.level == LEVEL_STAR) {
        sprintf(buf, "Star %s\n", Stars[where.snum]->name);
        notify(playernum, governor, buf);

        sprintf(buf,
                "locn: %f,%f\n",
                Stars[where.snum]->xpos,
                Stars[where.snum]->ypos);

        notify(playernum, governor, buf);

        if (race->God) {
            for (i = 0; i < Stars[where.snum]->numplanets; ++i) {
                getplanet(&p, (int)where.snum, i);

                sprintf(buf,
                        "%8d \"%s\"\n",
                        p->sectormappos,
                        Stars[where.snum]->pnames[i]);

                notify(playernum, governor, buf);
                free(p);
            }
        }

        sprintf(buf,
                "Gravity: %.2f\tInstability: ",
                Stars[where.snum]->gravity);

        notify(playernum, governor, buf);

        if ((race->tech >= TECH_SEE_STABILITY) || race->God) {
            sprintf(buf,
                    "%d%% (%s)\n",
                    Stars[where.snum]->stability,
                    Stars[where.snum]->stability < 20 ? "stable"
                    : Stars[where.snum]->stability < 40 ? "unstable"
                    : Stars[where.snum]->stability < 60 ? "dangerous"
                    : Stars[where.snum]->stability < 100 ? "WARNING! Nova iminent!"
                    : "undergoing nova");

            notify(playernum, governor, buf);
        } else {
            sprintf(buf, "(cannot determine)\n");
            notify(playernum, governor, buf);
        }

        sprintf(buf,
                "temperature class (1->10) %d\n",
                Stars[where.snum]->temperature);

        notify(playernum, governor, buf);
        sprintf(buf, "%d planets are ", Stars[where.snum]->numplanets);
        notify(playernum, governor, buf);

        for (x2 = 0; x2 < Stars[where.snum]->numplanets; ++x2) {
            sprintf(buf, "%s ", Stars[where.snum]->pnames[x2]);
            notify(playernum, governor, buf);
        }

        sprintf(buf, "\n");
        notify(playernum, governor, buf);

        if (((race->tech >= TECH_WORMHOLE) || race->God)
            && Stars[where.snum]->wh_has_wormhole) {
            sprintf(buf,
                    "System has a wormhole.  Orbit position: %d\n",
                    Stars[where.snum]->numplanets);

            notify(playernum, governor, buf);
        }
    } else if (where.level == LEVEL_UNIV) {
        /* sprintf(buf, "It's just _there_, you know?\n"; dumb -mfw */
        sprintf(buf, "Galaxy: %s\n", GAL_NAME);
        notify(playernum, governor, buf);
        sprintf(buf, "Type: %s\n", GAL_TYPE);
        notify(playernum, governor, buf);
        sprintf(buf, "Approx. size: %d\n", UNIVSIZE);
        notify(playernum, governor, buf);
        sprintf(buf, "No. of stars: %d\n", Sdata.numstars);
        notify(playernum, governor, buf);
    } else {
        sprintf(buf, "Illegal scope.\n");
        notify(playernum, governor, buf);
    }
} /* End survey */

void repair(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int lowx;
    int hix;
    int lowy;
    int hiy;
    int x2;
    int sectors;
    int cost;
    int allowed;
    sectortype *s;
    planettype *p;
    placetype where;
    racetype *race;

    /* General code -- jpd -- */
    if (governor == 0) {
        allowed = 1;
    } else if (races[playernum - 1]->governor[governor].rank <= RANK_REPAIR) {
        allowed = 1;
    } else {
        allowed = 0;
    }

    if (!control(playernum, governor, Stars[Dir[playernum][governor].snum])
        && !allowed) {
        no_permission_thing(playernum, governor, "repair", RANK_REPAIR);

        return;
    }
    /* General code -- jpd -- */

    if (argn == 1) {
        /* No args */
        where.level = Dir[playernum -1][governor].level;
        where.snum = Dir[playernum - 1][governor].snum;
        where.pnum = Dir[playernum - 1][governor].pnum;
    } else {
        /* Repairing a sector */
        if (isdigit((unsigned char)args[1][0]) && (index(args[1], ',') != NULL)) {
            if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
                sprintf(buf, "There are no sectors here.\n");
                notify(playernum, governor, buf);

                return;
            } else {
                where.level = LEVEL_PLAN;
                where.snum = Dir[playernum - 1][governor].snum;
                where.pnum = Dir[playernum - 1][governor].pnum;
            }
        } else {
            where = Getplace(playernum, governor, args[1], 0);

            if (where.err || (where.level == LEVEL_SHIP)) {
                return;
            }
        }
    }

    race = races[playernum - 1];

    if (where.level == LEVEL_PLAN) {
        getplanet(&p, (int)where.snum, (int)where.pnum);

        if (p->type == TYPE_WORMHOLE) {
            if ((race->tech >= TECH_WORMHOLE) || race->God) {
                sprintf(buf, "Wormholes cannot be repaired\n");
            } else {
                sprintf(buf, "No sectors to repair\n");
            }

            notify(playernum, governor, buf);
            free(p);

            return;
        }

        if (!p->info[playernum - 1].numsectsowned) {
            notify(playernum,
                   governor,
                   "You don't own any sectors on this planet.\n");

            free(p);

            return;
        }

        getsmap(Smap, p);

        if (isdigit((unsigned char)args[1][0]) && (index(args[1], ',') != NULL)) {
            get4args(args[1], &x2, &hix, &lowy, &hiy);
            /* ^^^ Translate from lowx:hix,lowy:hiy */
            x2 = MAX(0, x2);
            hix = MIN(hix, p->Maxx - 1);
            lowy = MAX(0, lowy);
            hiy = MIN(hiy, p->Maxy - 1);
        } else {
            /* Repair entire planet */
            x2 = 0;
            hix = p->Maxx - 1;
            lowy = 0;
            hiy = p->Maxy - 1;
        }

        sectors = 0;
        cost = 0;

        while (lowy <= hiy) {
            for (lowx = x2; lowx <= hix; ++lowx) {
                if (p->info[playernum - 1].resource >= SECTOR_REPAIR_COST) {
                    s = &Sector(*p, lowx, lowy);

                    if ((s->condition == WASTED)
                        && ((s->owner == playernum) || !s->owner)) {
                        s->condition = s->type;
                        s->fert = MIN(100, s->fert + 20);
                        p->info[playernum - 1].resource -= SECTOR_REPAIR_COST;
                        cost += SECTOR_REPAIR_COST;
                        sectors += 1;
                        putsector(s, p, lowx, lowy);
                    }
                }
            }

            ++lowy;
        }

        putplanet(p, (int)where.snum, (int)where.pnum);
        free(p);

        sprintf(buf,
                "%d sectors are repaired at a cost of %d resource.\n",
                sectors,
                cost);

        notify(playernum, governor, buf);
    } else {
        sprintf(buf, "scope must be a planet.\n");
        notify(playernum, governor, buf);
    }
}
