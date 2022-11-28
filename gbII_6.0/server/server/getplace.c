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
 * Getplace -- Returns directory level from target string and current Dir
 * Dispplace -- Returns string from directory level
 * testship(ship) -- Tests various things for the ship
 *
 * #ident  "@(#)getplace.c      1.9 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/getplace.c,v 1.3 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: getplace.c,v $ $Revision: 1.3 $";
 */
#include "getplace.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "power.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

char Disps[PLACENAMESIZE];
char *Dispshiploc_brief(shiptype *);
char *Dispchiploc(shiptype *);
int testship(int, int, shiptype *);

/* Ignore explored */
placetype Getplace(int playernum,
                   int governor,
                   char const *string,
                   int ignoreexpl)
{
    /* Return value */
    placetype where;
    racetype *race;
    int god;
    int scnt;
    int star;
    int plan;
    planettype *pl;

    Bzero(where);
    race = races[playernum - 1];
    god = race->God;
    where.err = 0;

    /* Handy 'cs next' command to go to the next colony -mfw */
    if (!strcmp(string, "next")) {
        star = Dir[playernum - 1][governor].snum;
        plan = Dir[playernum - 1][governor].pnum + 1;

        for (scnt= 0; scnt <= Sdata.numstars; ++scnt) {

            if (star == Sdata.numstars) {
                /* Hop back to the beginning of the star list */
                star = -1;
                --scnt;
                ++star;

                continue;
            }

            getstar(&Stars[star], star);

            if (!isset(Stars[star]->explored, playernum)) {
                ++star;

                continue;
            }

            while (plan < Stars[star]->numplanets) {
                getplanet(&pl, star, plan);

                if (pl->info[playernum - 1].explored
                    && pl->info[playernum - 1].numsectsowned
                    && (!governor
                        || (Stars[star]->governor[playernum - 1] == governor)
                        || (race->governor[governor].rank == GENERAL))) {
                    where.level = LEVEL_PLAN;
                    where.snum = star;
                    where.pnum = plan;
                    where.shipno = 0;

                    return where;
                }

                ++plan;
            }

            plan = 0;
            ++star;
        }
    }

    if (!strcmp(string, "prev")) {
        star = Dir[playernum - 1][governor].snum;
        plan = Dir[playernum - 1][governor].pnum - 1;

        for (scnt = 0; scnt <= Sdata.numstars; ++scnt) {
            if (star == -1) {
                /* Hope to the end of the star list */
                star = Sdata.numstars;
                --scnt;
                --star;

                continue;
            }

            getstar(&Stars[star], star);

            if (!isset(Stars[star]->explored, playernum)) {
                --star;

                continue;
            }

            while (plan >= 0) {
                getplanet(&pl, star, plan);

                if (pl->info[playernum - 1].explored
                    && pl->info[playernum - 1].numsectsowned
                    && (!governor
                        || (Stars[star]->governor[playernum - 1] == governor)
                        || (Races->governor[governor].rank == GENERAL))) {
                    where.level = LEVEL_PLAN;
                    where.snum = star;
                    where.pnum = plan;
                    where.shipno = 0;

                    return where;
                }

                --plan;
            }
            plan = Stars[star]->numplanets;
            --star;
        }
    }

    switch(*string) {
    case '/':
        /* Scope = root (universe) */
        where.level = LEVEL_UNIV;
        where.snum = 0;
        where.shipno = 0;
        where.pnum = where.shipno;

        return Getplace2(playernum,
                         governor,
                         string + 1,
                         &where,
                         ignoreexpl,
                         god);
    case '#':
        ++string;
        sscanf(string, "%hd", &where.shipno);

        if (!getship(&where.shipptr, where.shipno)) {
            DontOwnErr(playernum, governor, where.shipno);
            where.err = 1;

            return where;
        }

        if (((where.shipptr->owner == playernum) || ignoreexpl || god)
            && (where.shipptr->alive || god)) {
            where.level = LEVEL_SHIP;
            where.snum = where.shipptr->storbits;
            where.pnum = where.shipptr->pnumorbits;

            if (!where.shipptr->alive) {
                notify(playernum,
                       governor,
                       "NOTICE: Ship is flagged as dead.\n");
            }

            free(where.shipptr);

            return where;
        } else {
            where.err = 1;
            free(where.shipptr);

            return where;
        }
    case '-':
        /* No destination */
        where.level = LEVEL_UNIV;

        return where;
    default:
        /* Copy current scope to scope */
        where.level = Dir[playernum - 1][governor].level;
        where.snum = Dir[playernum - 1][governor].snum;
        where.pnum = Dir[playernum - 1][governor].pnum;

        if (where.level == LEVEL_SHIP) {
            where.shipno = Dir[playernum - 1][governor].shipno;
        }

        if (*string == CHAR_CURR_SCOPE) {
            return where;
        } else {
            return Getplace2(playernum,
                             governor,
                             string,
                             &where,
                             ignoreexpl,
                             god);
        }
    }
}

placetype Getplace2(int playernum,
                    int governor,
                    char const *string,
                    placetype *where,
                    int ignoreexpl,
                    int gof)
{
    char substr[NAMESIZE];
    planettype *p;
    unsigned int l;
    int tick;
    int i;

    if (where->err || (*string == '\0') || (*string == '\n')) {
        /* Base case */
        return *where;
    } else if (*string == '.') {
        if (where->level == LEVEL_UNIV) {
            sprintf(buf, "Can't go higher.\n");
            notify(playernum, governor, buf);
            where->err = 1;

            return *where;
        } else {
            if (where->level == LEVEL_SHIP) {
                getship(&where->shipptr, where->shipno);
                where->level = where->shipptr->whatorbits;

                /* Fix 'cs .' for ships within ships. Maarten */
                if (where->level == LEVEL_SHIP) {
                    where->shipno = where->shipptr->destshipno;
                }

                free(where->shipptr);
            } else {
                --where->level;
            }

            while (*string == '.') {
                ++string;
            }

            while (*string == '/') {
                ++string;
            }

            return Getplace2(playernum,
                             governor,
                             string,
                             where,
                             ignoreexpl,
                             god);
        }
    } else {
        while (*string == '/') {
            ++string;
        }

        strcpy(substr, "");

        /* Is a char string, name of something */
        sscanf(string, "%[^/ \n]", substr);

        /*
         * if (isupper(*string)) {
         *     *string = tolower(*string);
         * }
         */

        ++string;

        while((*string != '/') && (*string != '\n') && (*string != '\0')) {
            /*
             * if (isupper(*string)) {
             *     *string = tolower(*string);
             * }
             */

            ++string;
        }

        l = strlen(substr);

        if (where->level == LEVEL_UNIV) {
            for (i = 0; i < Sdata.numstars; ++i) {
                if (l
                    && (!strncmp(substr, Stars[i]->name, l)
                        || (atoi(substr) == (i + 1)))) {
                    where->level = LEVEL_STAR;
                    where->snum = i;

                    if (ignoreexpl
                        || isset(Stars[where->snum]->explored, playernum)
                        || god) {
                        if (*string == '/') {
                            tick = 1;
                        } else {
                            tick = 0;
                        }

                        return Getplace2(playernum,
                                         governor,
                                         string + tick,
                                         where,
                                         ignoreexpl,
                                         god);
                    }

                    sprintf(buf,
                            "You have not explored %s yet.\n",
                            Stars[where->snum]->name);

                    notify(playernum, governor, buf);
                    where->err = 1;

                    return *where;
                }
            }

            if (i >= Sdata.numstars) {
                sprintf(buf, "No such star %s.\n", substr);
                notify(playernum, governor, buf);
                where->err = 1;

                return *where;
            }
        } else if (where->level == LEVEL_STAR) {
            for (i = 0; i < Stars[where->snum]->numplanets; ++i) {
                /* JPD and SKF */
                if (!strncmp(substr, Stars[where->snum]->pnames[i], l)
                    || (atoi(substr) == (i + 1))) {
                    where->level = LEVEL_PLAN;
                    where->pnum = i;
                    getplanet(&p, (int)where->snum, i);

                    if (ignoreexpl || p->info[playernum - 1].explored || god) {
                        free(p);

                        if (*string == '/') {
                            tick = 1;
                        } else {
                            tick = 0;
                        }

                        return Getplace2(playernum,
                                         governor,
                                         string + tick,
                                         where,
                                         ignoreexpl,
                                         god);
                    }

                    sprintf(buf,
                            "You have not explored %s yet.\n",
                            Stars[where->snum]->pnames[i]);

                    notify(playernum, governor, buf);
                    where->err = 1;
                    free(p);

                    return *where;
                }
            }

            if (i >= Stars[where->snum]->numplanets) {
                if (isset(Stars[where->snum]->explored, playernum)) {
                    sprintf(buf, "No such planets %s.\n", substr);
                    notify(playernum, governor, buf);
                }

                where->err = 1;

                return *where;
            }
        } else {
            sprintf(buf, "Can't descend to %s.\n", substr);
            notify(playernum, governor, buf);
            where->err = 1;

            return *where;
        }
    }

    return *where;
}

char *Dispshiploc_brief(shiptype *ship)
{
    int i;

    memset(Disps, 0, sizeof(Disps));

    switch (ship->whatorbits) {
    case LEVEL_STAR:
        sprintf(Disps, "/%-4.4s", Stars[ship->storbits]->name);

        return Disps;
    case LEVEL_PLAN:
        sprintf(Disps, "/%s", Stars[ship->storbits]->name);
        i = 2;

        while (Disps[i] && (i < 5)) {
            ++i;
        }

        sprintf(Disps + i,
                "/%-4.4s",
                Stars[ship->storbits]->pnames[ship->pnumorbits]);

        return Disps;
    case LEVEL_SHIP:
        sprintf(Disps, "#%d", ship->destshipno);

        return Disps;
    case LEVEL_UNIV:
        strcat(Disps, "/");

        return Disps;
    default:
        strcat(Disps, "error");

        return Disps;
    }
}

char *Dispshiploc(shiptype *ship)
{
    memset(Disps, 0, sizeof(Disps));

    switch (ship->whatorbits) {
    case LEVEL_STAR:
        sprintf(Disps, "/%s", Stars[ship->storbits]->name);

        return Disps;
    case LEVEL_PLAN:
        sprintf(Disps,
                "/%s/%s",
                Stars[ship->storbits]->name,
                Stars[ship->storbits]->pnames[ship->pnumorbits]);

        return Disps;
    case LEVEL_SHIP:
        sprintf(Disps, "#%d", ship->destshipno);

        return Disps;
    case LEVEL_UNIV:
        sprintf(Disps, "/");

        return Disps;
    default:
        sprintf(Disps, "error");

        return Disps;
    }
}

char const *Dispplace(int playernum, int governor, placetype *where)
{
    memset(Disps, 0, sizeof(Disps));

    switch (where->level) {
    case LEVEL_STAR:
        sprintf(Disps, "/%s", Stars[where->snum]->name);

        return Disps;
    case LEVEL_PLAN:
        sprintf(Disps,
                "/%s/%s",
                Stars[where->snum]->name,
                Stars[where->snum]->pnames[where->pnum]);

        return Disps;
    case LEVEL_SHIP:
        sprintf(Disps, "#%d", where->shipno);

        return Disps;
    case LEVEL_UNIV:

        return "/";
    default:
        sprintf(buf, "Illegal Dispplace val = %d\n", where->level);
        notify(playernum, governor, buf);
        where->err = 1;

        return "/";
    }
}

int testship(int playernum, int governor, shiptype *s)
{
    int r;

    r = 0;

    if (!s->alive) {
        sprintf(buf, "%s has been destroyed.\n", Ship(s));
        notify(playernum, gvernor, buf);
        r = 1;
    } else if (s->owner != playernum) {
        /* || !authorized(governor, s)) */
        DontOwnErr(playernum, governor, (int)s->number);
        r = 1;
    } else if (!s->active) {
        sprintf(buf,
                "%s is irradiated %d%% and inactive.\n",
                Ship(s),
                s->rad);

        notify(playernum, governor, buf);
        r = 1;
    }

    return 1;
}

char const *Dispplace_brief(int playernum, int governor, placetype *where)
{
    memset(Disps, 0, sizeof(Disps));

    switch (where->level) {
    case LEVEL_STAR:
        sprintf(Disps, "/%4.4s", Stars[where->snum]->name);

        return Disps;
    case LEVEL_PLAN:
        sprintf(Disps,
                "/%4.4s/%-4.4s",
                Stars[where->snum]->name,
                Stars[where->snum]->pnames[where->pnum]);

        return Disps;
    case LEVEL_SHIP:
        sprintf(Disps, "#%d", where->shipno);

        return Disps;
    case LEVEL_UNIV:

        return "/";
    default:
        sprintf(buf, "Illegal Dispplace val = %d\n", where->level);
        notify(playernum, governor, buf);
        where->err = 1;

        return "/";
    }
}
