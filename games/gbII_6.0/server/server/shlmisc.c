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
 * Miscellaneous stuff included in the shell
 *
 * #ident  "@(#)shlmisc.c  1.12 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/shlmisc.c,v 1.5 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)  $RCSfile: shlmisc.c,v $ $Revision: 1.5 $";
 */
#include "shlmisc.h"

#include <ctype.h>
#include <curses.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "buffers.h"
#include "files_shl.h"
#include "fleet.h"
#include "GB_server.h"
#include "max.h"
#include "md5.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "tweakables.h"
#include "vars.h"

char *Ship(shiptype *s);
void grant(int, int, int);
void governors(int, int, int);
void do_revoke(racetype *, int, int);
int do_shiplist(shiptype **, int *);
int in_list(int, char *, shiptype *, int *);
void fix(int, int);
void DontOwn_Err(int, int, int);

#ifdef CHAP_AUTH
int Getracenum(char *, char *, int *, int *, char *, int);

#else
int Getracenum(char *, char *, int *, int *);

#endif

int GetPlayer(char *);
void allocateAPs(int, int, int);
void deductAps(int, int, int, int, int);
void list(int, int);
double morale_factor(double);

extern int authorized(int, shiptype *);
extern int authorized_in_star(int, int, startype *);

#define FNAMESIZE 20

struct allocated {
    char *addr;
    char fname[FNAMESIZE + 1];
    int lineno;
};

char *Ship(shiptype *s)
{
    /* Switch between 0 and 1 - adr is a global variable */
    adr = !adr;
    sprintf(junk[adr],
            "%c%d %s [%d]",
            Shipltrs[s->type],
            s->number,
            s->name,
            s->owner);

    /* junk is a global buffer */
    return junk[adr];
}

void grant(int playernum, int governor, int apcount)
{
    racetype *race;
    int gov;
    int nextshipno;
    int shipno;
    shiptype *ship;
    /* int warned = 0; */

    race = races[playernum - 1];

    if (argn < 3) {
        notify(playernum, governor, "Syntax: grant <governor> star\n");
        notify(playernum, governor, "        grant <governor> ship <shiplist>\n");
        notify(playernum, governor, "        grant <governor> money <amount>\n");

        return;
    }

    gov = atoi(args[1]);

    if ((gov < 0) || (gov > MAXGOVERNORS)) {
        notify(playernum, governor, "Bad governor number.\n");

        return;
    } else if (!race->governor[gov].active) {
        notify(playernum, governor, "That governor is not active.\n");

        return;
    } else if (match(args[2], "star")) {
        int snum;

        if (Dir[playernum - 1][governor].level != LEVEL_STAR) {
            notify(playernum, governor, "Please cs to the star system first.\n");

            return;
        }

        snum = Dir[playernum - 1][governor].snum;
        Stars[snum]->governor[playernum - 1] = gov;
        sprintf(buf,
                "\"%s\" has granted you control of the /%s star system.\n",
                race->governor[governor].name,
                Stars[snum]->name);

        warn(playernum, gov, buf);
        putstar(Stars[snum], snum);
    } else if (match(args[2], "ship")) {
        nextshipno = start_shiplist(playernum, governor, args[3]);
        shipno = do_shiplist(&ship, &nextshipno);

        while (shipno) {
            if (in_list(playernum, args[3], ship, &nextshipno)) {
                ship->governor = gov;
                sprintf(buf,
                        "\"%s\" granted you %s at %s\n",
                        race->governor[governor].name,
                        Ship(ship),
                        prin_ship_orbits(ship));

                warn(playernum, gov, buf);
                putship(ship);
                sprintf(buf,
                        "%s granted to \"%s\"\n",
                        Ship(ship),
                        race->governor[gov].name);

                notify(playernum, governor, buf);
                free(ship);
            } else {
                free(ship);
            }

            shipno = do_shiplist(&ship, &nextshipno);
        }
    } else if (match(args[2], "money")) {
        int amount;

        if (argn < 4) {
            notify(playernum, governor, "Indicate the amount of money.\n");

            return;
        }

        amount = atoi(args[3]);

        if (((long)amount > MONEY(race, governor))
            && ((long)-amount > -MONEY(race, governor))) {
            notify(playernum, governor, "Illegal amount\n");

            return;
        }

        if (amount > 0) {
            /* Add to a gov */
            sprintf(buf,
                    "\"%s\" granted you %d money.\n",
                    race->governor[governor].name,
                    amount);

            warn(playernum, gov, buf);
            MONEY(race, governor) -= abs(amount);
            MONEY(race, gov) += abs(amount);
        } else if (amount < 0) {
            /* Deduct from a gov */
            sprintf(buf,
                    "\"%s\" docked you %d money.\n",
                    race->governor[governor].name,
                    -amount);

            warn(playernum, gov, buf);
            MONEY(race, governor) += abs(amount);
            MONEY(race, gov) -= abs(amount);
        }

        putrace(race);

        return;
    } else {
        notify(playernum, governor, "You can't grant that.\n");
    }
}

void governor(int playernum, int governor, int apcount)
{
    racetype *race;
    int i;
    char rank[9];
    int gov;

    race = races[playernum - 1];

    /* The only thing governors can do with this */
    if (governor || (argn < 3)) {
        for (i = 0; i <= MAXGOVERNORS; ++i) {
            if (race->governor[i].active) {
                switch(race->governor[i].rank) {
                case LEADER:
                    strcpy(rank, "LEADER ");

                    break;
                case GENERAL:
                    strcpy(rank, "GENERAL");

                    break;
                case CAPTAIN:
                    strcpy(rank, "CAPTAIN");

                    break;
                case PRIVATE:
                    strcpy(rank, "PRIVATE");

                    break;
                case NOVICE:
                    strcpy(rank, "NOVICE ");

                    break;
                default:
                    strcpy(rank, "UNKNOWN");
                }
            } else {
                /* Governor isn't active */
                strcpy(rank, "        ");
            }

            if (governor) {
#ifndef COLLECTIVE_MONEY
                sprintf(buf,
                        "%d %-15.15s %8s %8s %10ld %s",
                        i,
                        race->governor[i].name,
                        race->governor[i].active ? "ACTIVE" : "INACTIVE",
                        rank,
                        MONEY(race, i);
                        ctime(&Race->governor[i].last_login));

#else

                sprintf(buf,
                        "%d %-15.15s %8s %8s %10ld %s",
                        i,
                        race->governor[i].name,
                        race->governor[i].active ? "ACTIVE" : "INACTIVE",
                        rank,
                        (i != 0) ? 0 : MONEY(race, 0),
                        ctime(&race->governor[i].last_login));
#endif
            } else {
                sprintf(buf,
                        "%d %-12.12s %-10.10s %8s %8s %10ld %s",
                        i,
                        race->governor[i].name,
                        race->governor[i].password,
                        rank,
                        race->governor[i].active ? "ACTIVE" : "INACTIVE",
                        MONEY(race, i),
                        ctime(&race->governor[i].last_login));
            }

            notify(playernum, governor, buf);
        }
    } else {
        gov = atoi(args[1]);

        if ((gov < 0) || (gov > MAXGOVERNORS)) {
            notify(playernum, governor, "No such governor.\n");

            return;
        } else if (match(args[0], "appoint")) {
            /* Syntax: 'appoint <govnum> <govname> <password> <level>' */
            if (race->governor[gov].active) {
                notify(playernum,
                       governor,
                       "That governor is already appointed.\n");

                return;
            }

            if (argn < 4) {
                notify(playernum, governor, "Invalid number of arguments.\n");

                return;
            }

            if (strlen(args[2]) == 0) {
                notify(playernum, governor, "Bad governor name, try again.\n");

                return;
            } else {
                strncpy(race->governor[gov].name, args[2], RNAMESIZE - 1);
            }

            if (argn < 5) {
                race->governor[gov].rank = PRIVATE;
            } else {
                if (match(args[4], "general")) {
                    race->governor[gov].rank = GENERAL;
                } else if (match(args[4], "captain")) {
                    race->governor[gov].rank = CAPTAIN;
                } else if (match(args[4], "private")) {
                    race->governor[gov].rank = PRIVATE;
                } else if (match(args[4], "novice")) {
                    race->governor[gov].rank = NOVICE;
                } else {
                    notify(playernum, governor, "Invalid level for governor\n");

                    return;
                }
            }

            if (race->Guest) {
                race->governor[gov].rank = GUEST_RANK;
            }

            race->governor[gov].active = 1;
            race->governor[gov].deflevel = race->governor[0].deflevel;
            race->governor[gov].homelevel = race->governor[gov].deflevel;
            race->governor[gov].defsystem = race->governor[0].defsystem;
            race->governor[gov].homesystem = race->governor[gov].defsystem;
            race->governor[gov].defplanetnum = race->governor[0].defplanetnum;
            race->governor[gov].homeplanetnum = race->governor[gov].defplanetnum;

#ifndef COLLECTIVE_MONEY
            MONEY(race, gov) = 0;
#endif

            race->governor[gov].toggle.highlight = playernum;
            race->governor[gov].toggle.inverse = 1;
            strncpy(race->governor[gov].password, args[3], RNAMESIZE - 1);
            putrace(race);
            notify(playernum, governor, "Governor activated.\n");

            return;
        } else if (match(args[0], "promote") || match(args[0], "demote")) {
            if (!race->governor[gov].active) {
                notify(playernum, governor, "That governor isn't active!\n");

                return;
            }

            if (race->Guest) {
                notify(playernum,
                       governor,
                       "You cannot promote or demote a guest race!\n");

                return;
            }

            if (gov == 0) {
                notify(playernum,
                       governor,
                       "You cannot promote or demote the leader!\n");

                return;
            }

            if (match(args[2], "leader")) {
                notify(playernum,
                       governor,
                       "Only governor 0 can be designated a leader, PUTZ!\n");

                return;
            } else if (match(args[2], "general")) {
                race->governor[gov].rank = GENERAL;
            } else if (match(args[2], "captain")) {
                race->governor[gov].rank = CAPTAIN;
            } else if (match(args[2], "private")) {
                race->governor[gov].rank = PRIVATE;
            } else if (match(args[2], "novice")) {
                race->governor[gov].rank = NOVICE;
            } else {
                notify(playernum, governor, "Invalid level for governor\n");

                return;
            }

            putrace(race);
        } else if (match(args[0], "revoke")) {
            int j;

            if (!gov) {
                notify(playernum,
                       governor,
                       "You can't revoke your leadership!\n");

                return;
            }

            if (!race->governor[gov].active) {
                notify(playernum, governor, "That governor is not active.\n");

                return;
            }

            if (argn < 4) {
                j = 0;
            } else {
                /* Who gets this governor's stuff */
                j = atoi(args[3]);
            }

            if ((j < 0) || (j > MAXGOVERNORS)) {
                notify(playernum,
                       governor,
                       "You can't give stuff to that governor!\n");

                return;
            }

            if (!strcmp(race->governor[gov].password, args[2])) {
                notify(playernum, governor, "Incorrect password.\n");

                return;
            }

            if (!race->governor[j].active || (j == gov)) {
                notify(playernum, governor, "Bad target governor.\n");

                return;
            }

            /* Give stuff from gov to j */
            do_revoke(race, gov, j);
            putrace(race);
            notify(playernum, governor, "Done.\n");

            return;
        } else if (match(args[2], "password")) {
            if (race->Guest) {
                notify(playernum,
                       governor,
                       "Guest races cannot change passwords.\n");

                return;
            }

            if (argn < 4) {
                notify(playernum, governor, "You must give a password.\n");

                return;
            }

            if (!race->governor[gov].active) {
                notify(playernum, governor, "That governor is inactive.\n");

                return;
            }

            strncpy(race->governor[gov].password, args[3], RNAMESIZE - 1);
            putrace(race);
            notify(playernum, governor, "Password changed.\n");

            return;
        } else {
            notify(playernum, governor, "Bad option.\n");
        }
    }
}

void do_revoke(racetype *race, int gov, int j)
{
    int i;
    char revoke_buf[1024];
    shiptype *ship;

    sprintf(revoke_buf,
            "*** Transferring [%d,%d];s ownings to [%d,%d] ***\n\n",
            race->Playernum,
            gov,
            race->Playernum,
            j);

    notify(race->Playernum, 0, revoke_buf);

    /* First do stars... */
    for (i = 0; i < Sdata.numstars; ++i) {
        if (Stars[i]->governor[race->Playernum - 1] == gov) {
            Stars[i]->governor[race->Playernum - 1] = j;
            sprintf(revoke_buf,
                    "Changed jurisdiction of /%s...\n",
                    Stars[i]->name);

            notify(race->Playernum, 0, revoke_buf);
            putstar(Stars[i], i);
        }
    }

    /* Now do ships... */
    Num_ships = Numships();

    for (i = 1; i < Num_ships; ++i) {
        getship(&ship, i);

        if (ship->active
            && (ship->owner == race->Playernum)
            && (ship->governor == gov)) {
            ship->governor = j;
            sprintf(revoke_buf,
                    "changed ownership of %c%d...\n",
                    Shipltrs[ship->type],
                    i);

            notify(race->Playernum, 0, revoke_buf);
            putship(ship);
        }

        free(ship);
    }

    /* And money too... */
#ifndef COLLECTIVE_MONEY
    sprintf(revoke_buf, "Transferring %ld money...\n", MONEY(race, gov));
    notify(race->Playernum, 0, revoke_buf);
    MONEY(race, j) = MONEY(race, j) + MONEY(race, gov);
    MONEY(race, gov) = 0;
#endif

    /* And last but not least, flag the governor as inactive... */
    race->governor[gov].active = 0;
    strcpy(race->governor[gov].password, "");
    strcpy(race->governor[gov].name, "");
    sprintf(revoke_buf,
            "\n*** Governor [%d,%d]'s powers have been REVOKED ***\n",
            race->Playernum,
            gov);

    notify(race->Playernum, 0, revoke_buf);
    sprintf(revoke_buf, "rm %s.%d.%d", TELEGRAMFL, race->Playernum, gov);
    system(revoke_buf); /* Remove the telegram file too... */

    return;
}

int start_shiplist(int playernum, int governor, char const *string)
{
    char const *p;
    planettype *planet;
    shiptype *ship;
    int st;
    int pl;
    int sh;
    int fl;
    char *c;

    p = string;

    if (*p == '#') {
        ++p;

        return atoi(p);
    }

    if (isdigit((unsigned char)*p)) {
        return atoi(p);
    }

    if (*p == '%') {
        /* We're talking about a fleet! -mfw */
        ++p;
        c = (char *)p;
        fl = fctofi(*c);

        if ((fl > 0)
            && (races[playernum - 1]->fleet[fl].admiral != governor)
            && (races[playernum - 1]->fleet[fl].flagship != 0)) {
            notify(playernum,
                   governor,
                   "You're not admiral of the source fleet.\n");

            return 0;
        }

        if ((fl > MAXFLEETS) || (fl < 1)) {
            return 0;
        } else {
            return races[playernum - 1]->fleet[fl].flagship;
        }
    }

    /* Ship number not given */
    st = Dir[playernum - 1][governor].snum;
    pl = Dir[playernum - 1][governor].pnum;

    switch (Dir[playernum - 1][governor].level) {
    case LEVEL_UNIV:
        getsdata(&Sdata);

        return Sdata.ships;
    case LEVEL_STAR:
        /* Stars don't need to be freed */
        getstar(&Stars[st], st);

        return Stars[st]->ships;
    case LEVEL_PLAN:
        getplanet(&planet, st, pl);
        sh = planet->ships;
        free(planet);

        return sh;
    case LEVEL_SHIP:
        getship(&ship, Dir[playernum - 1][governor].shipno);
        sh = ship->ships;
        free(ship);

        return sh;
    default:

        break;
    }

    return 0;
}

/* Step through linked list at current player scope */
int do_shiplist(shiptype **s, int *nextshipno)
{
    int shipno;

    /*
     * HUTm (kse) Now we should be able to go through all ships lists even if
     * there were some dead ships
     */

    /* nextshipno should be index of next ship in the list */
    shipno = *nextshipno;

    if (!shipno) {
        /* If index is 0 list is at end */
        return 0;
    }

    /* Allocate memory, free in loop */
    if (!getship(s, shipno)) {
        /* If we can't get that ship there must be some error */
        return 0;
    }

    /*
     * This prevents it from returning a dead ship. I.e. Try to scrap a dead
     * ship. -JPD
     */
    if (!(*s)->alive) {
        return 0;
    }

    /* New head of list is next of current ship */
    *nextshipno = (*s)->nextship;

    /* If current ship is alive return it's index */
    if ((*s)->alive) {
        return shipno;
    }

    /* If ships was not alive iterate next list item */
    while (!(*s)->alive) {
        /* nextshipno should be index of next ship in the list */
        shipno = *nextshipno;

        if (!shipno) {
            /* If index is 0 list is at end */
            return 0;
        }

        /* Allocate memory, free in loop */
        if (!getship(s, shipno)) {
            /* If we can't get that ship there must be some error */
            return 0;
        }

        /*
         * This prevents it from returning a dead ship. I.e. Try to scrap a dead
         * ship. -JPD
         */
        if (!(*s)->alive) {
            return 0;
        }

        /* New head of list is next of current ship */
        *nextshipno = (*s)->nextship;

        /* If current ship is alive return it's index */
        if ((*s)->alive) {
            return shipno;
        }
    }

    /* We should not get here (?) */
    return 0;
}

int in_list(int playernum, char *list, shiptype *s, int *nextshipno)
{
    char *p;
    char q;
    char *c;

    if ((!races[playernum - 1]->God && (s->owner != playernum)) || !s->alive) {
        return 0;
    }

    q = Shipltrs[s->type];
    p = list;

    if ((*p == '#') || isdigit((unsigned char)*p)) {
        if ((!races[playernum - 1]->God && (s->owner != playernum))
            || !s->alive) {
            return 0;
        }

        *nextshipno = 0;

        return s->number;
    }

    if (*p == '%') {
        ++p;
        c = (char *)p;

        /* Is the ship in this fleet? -mfw */
        if ((*c == fitofc(s->fleetmember))
            || (*c == (fitofc(s->fleetmember) + 32))) {
            *nextshipno = s->nextinfleet;

            return s->number;
        } else {
            return 0;
        }
    }

    while (*p) {
        if ((*p == q) || (*p == '*')) {
            /* '*' is a wildcard */
            return s->number;
        }

        ++p;
    }

    return 0;
}

/* Deity fix-it utilities */
void fix(int playernum, int governor)
{
    planettype *p;
    shiptype *s;
    int x;
    int y;

    if (match(args[1], "sector")) {
        char *what;
        long amt;
        sectortype *sect;

        if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
            notify(playernum, governor, "Change scope to planet first.\n");

            return;
        }

        /* Fix sector routines */
        if (argn < 6) {
            /* Error */
            notify(playernum, governor, "Not enough args\n");

            return;
        }

        x = atoi(args[2]);
        y = atoi(args[3]);
        what = args[4];
        amt = atoi(args[5]);

        getplanet(&p,
                  Dir[playernum - 1][governor].snum,
                  Dir[playernum - 1][governor].pnum);

        getsector(&sect, p, x, y);
        sprintf(buf, "Owner=%d\n", sect->owner);
        notify(playernum, governor, buf);

        /* Now what? */
        if (match(what, "res")) {
            sect->resource = amt;
            sprintf(buf, "Sector resource set to %d\n", sect->resource);
        } else if (match(what, "mob")) {
            sect->mobilization = amt;
            sprintf(buf, "Sector mobility set to %d\n", sect->mobilization);
        } else if (match(what, "owner")) {
            sect->owner = amt;
            sprintf(buf, "Sector owner set to %d\n", sect->owner);
        } else if (match(what, "popn")) {
            sect->popn = amt;
            sprintf(buf, "Sector population set to %d\n", sect->popn);
        } else if (match(what, "eff")) {
            sect->eff = amt;
            sprintf(buf, "Sector efficiency set to %d\n", sect->eff);
        } else if (match(what, "mil")) {
            sect->troops = amt;
            sprintf(buf, "Sector military set to %d\n", sect->troops);
        } else if (match(what, "fert")) {
            sect->fert = amt;
            sprintf(buf, "Sector fertility set to %d\n", sect->fert);
        } else if (match(what, "type")) {
            sect->type = amt;
            sprintf(buf, "Sector type set to %d\n", sect->type);
        } else if (match(what, "xtals")) {
            sect->crystals = amt;
            sprintf(buf, "Sector crystals set to %d\n", sect->crystals);
        } else {
            notify(playernum, governor, "No such option for 'fix sector'.\n");
            free(sect);
            free(p);

            return;
        }

        putsector(sect, p, x, y);
        free(sect);
        free(p);
        notify(playernum, governor, buf);

        return;
    } else if (match(args[1], "planet")) {
        if (Dir[playernum - 1][governor].level != LEVEL_PLAN) {
            notify(playernum, governor, "Change scope to the planet first.\n");

            return;
        }

        getplanet(&p,
                  Dir[playernum - 1][governor].snum,
                  Dir[playernum - 1][governor].pnum);

        if (match(args[2], "Maxx")) {
            if (argn > 3) {
                p->Maxx = atoi(args[3]);
            }

            sprintf(buf, "Maxx = %d\n", p->Maxx);
        } else if (match(args[2], "Maxy")) {
            if (argn > 3) {
                p->Maxy = atoi(args[3]);
            }

            sprintf(buf, "Maxy = %d\n", p->Maxy);
        } else if (match(args[2], "xpos")) {
            if (argn > 3) {
                p->xpos = (double)atoi(args[3]);
            }

            sprintf(buf, "xpos = %f\n", p->xpos);
        } else if (match(args[2], "ypos")) {
            if (argn > 3) {
                p->ypos = (double)atoi(args[3]);
            }

            sprintf(buf, "ypos = %f\n", p->ypos);
        } else if (match(args[2], "ships")) {
            if (argn > 3) {
                p->ships = atoi(args[3]);
            }

            sprintf(buf, "ships = %d\n", p->ships);
        } else if (match(args[2], "sectormappos")) {
            if (argn > 3) {
                p->sectormappos = atoi(args[3]);
            }

            sprintf(buf, "sectormappos = %d\n", p->sectormappos);
        } else if (match(args[2], "rtemp")) {
            if (argn > 3) {
                p->conditions[RTEMP] = atoi(args[3]);
            }

            sprintf(buf, "RTEMP = %d\n", p->conditions[RTEMP]);
        } else if (match(args[2], "temperature")) {
            if (argn > 3) {
                p->conditions[TEMP] = atoi(args[3]);
            }

            sprintf(buf, "TEMP = %d\n", p->conditions[TEMP]);
        } else if (match(args[2], "methane")) {
            if (argn > 3) {
                p->conditions[METHANE] = atoi(args[3]);
            }

            sprintf(buf, "METHANE = %d\n", p->conditions[METHANE]);
        } else if (match(args[2], "oxygen")) {
            if (argn > 3) {
                p->conditions[OXYGEN] = atoi(args[3]);
            }

            sprintf(buf, "OXYGEN = %d\n", p->conditions[OXYGEN]);
        } else if (match(args[2], "co2")) {
            if (argn > 3) {
                p->conditions[CO2] = atoi(args[3]);
            }

            sprintf(buf, "CO2 = %d\n", p->conditions[CO2]);
        } else if (match(args[2], "hydrogen")) {
            if (argn > 3) {
                p->conditions[HYDROGEN] = atoi(args[3]);
            }

            sprintf(buf, "HYDROGEN = %d\n", p->conditions[HYDROGEN]);
        } else if (match(args[2], "nitrogen")) {
            if (argn > 3) {
                p->conditions[NITROGEN] = atoi(args[3]);
            }

            sprintf(buf, "NITROGEN = %d\n", p->conditions[NITROGEN]);
        } else if (match(args[2], "sulfur")) {
            if (argn > 3) {
                p->conditions[SULFUR] = atoi(args[3]);
            }

            sprintf(buf, "SULFUR = %d\n", p->conditions[SULFUR]);
        } else if (match(args[2], "helium")) {
            if (argn > 3) {
                p->conditions[HELIUM] = atoi(args[3]);
            }

            sprintf(buf, "HELIUM = %d\n", p->conditions[HELIUM]);
        } else if (match(args[2], "other")) {
            if (argn > 3) {
                p->conditions[OTHER] = atoi(args[3]);
            }

            sprintf(buf, "OTHER = %d\n", p->conditions[OTHER]);
        } else if (match(args[2], "toxic")) {
            if (argn > 3) {
                p->conditions[TOXIC] = atoi(args[3]);
            }

            sprintf(buf, "TOXIC = %d\n", p->conditions[TOXIC]);
        } else if (match(args[2], "population")) {
            if (argn > 3) {
                p->popn = atoi(args[3]);
            }

            sprintf(buf, "population = %ld\n", p->popn);
        } else {
            notify(playernum, governor, "No such option for 'fix planet'.\n");
            free(p);

            return;
        }

        notify(playernum, governor, buf);

        if (argn > 3) {
            putplanet(p,
                      Dir[playernum - 1][governor].snum,
                      Dir[playernum - 1][governor].pnum);
        }

        free(p);

        return;
    } else if (match(args[1], "ship")) {
        if (Dir[playernum - 1][governor].level != LEVEL_SHIP) {
            notify(playernum,
                   governor,
                   "Change scope to the ship you wish to fix.\n");

            return;
        }

        getship(&s, Dir[playernum - 1][governor].shipno);

        if (match(args[2], "fuel")) {
            if (argn > 3) {
                s->fuel = (double)atoi(args[3]);
            }

            sprintf(buf, "fuel = %f\n", s->fuel);
        } else if (match(args[2], "max_fuel")) {
            if (argn > 3) {
                s->max_fuel = atoi(args[3]);
            }

            sprintf(buf, "fuel = %d\n", s->max_fuel);
        } else if (match(args[2], "destruct")) {
            if (argn > 3) {
                s->destruct = atoi(args[3]);
            }

            sprintf(buf, "destruct = %d\n", s->destruct);
        } else if (match(args[2], "max_destruct")) {
            if (argn > 3) {
                s->max_destruct = atoi(args[3]);
            }

            sprintf(buf, "max_destruct = %d\n", s->max_destruct);
        } else if (match(args[2], "resource")) {
            if (argn > 3) {
                s->resource = atoi(args[3]);
            }

            sprintf(buf, "resource = %d\n", s->resource);
        } else if (match(args[2], "max_resource")) {
            if (argn > 3) {
                s->max_resource = atoi(args[3]);
            }

            sprintf(buf, "max_resource = %d\n", s->max_resource);
        } else if (match(args[2], "damage")) {
            if (argn > 3) {
                s->damage = atoi(args[3]);
            }

            sprintf(buf, "damage = %d\n", s->damage);
        } else if (match(args[2], "alive")) {
            s->alive = 1;
            s->damage = 0;
            sprintf(buf, "%s resurrected\n", Ship(s));
        } else if (match(args[2], "dead")) {
            s->alive = 0;
            s->damage = 100;
            sprintf(buf, "%s destroyed\n", Ship(s));
        } else if (match(args[2], "owner")) {
            if (argn > 3) {
                s->owner = atoi(args[3]);
            }

            sprintf(buf, "owner = %d\n", s->owner);
        } else if (match(args[2], "ships")) {
            if (argn > 3) {
                s->ships = atoi(args[3]);
            }

            sprintf(buf, "ships = %d\n", s->ships);
        } else if (match(args[2], "nextship")) {
            if (argn > 3) {
                s->nextship = atoi(args[3]);
            }

            sprintf(buf, "nextship = %d\n", s->nextship);
        } else if (match(args[2], "popn")) {
            if (argn > 3) {
                s->popn = atoi(args[3]);
            }

            sprintf(buf, "popn = %d\n", s->popn);
        } else if (match(args[2], "troops")) {
            if (argn > 3) {
                s->troops = atoi(args[3]);
            }

            sprintf(buf, "troops = %d\n", s->troops);
        } else if(match(args[2], "hanger")) {
            if (argn > 3) {
                s->hanger = atoi(args[3]);
            }

            sprintf(buf, "hanger = %d\n", s->hanger);
        } else if (match(args[2], "primary")) {
            if (argn > 3) {
                s->primary = atoi(args[3]);
            }

            sprintf(buf, "primary = %d\n", s->primary);
        } else if (match(args[2], "secondary")) {
            if (argn > 3) {
                s->secondary = atoi(args[3]);
            }

            sprintf(buf, "secondary = %d\n", s->secondary);
        } else if (match(args[2], "armor")) {
            if (argn > 3) {
                s->armor = atoi(args[3]);
            }

            sprintf(buf, "armor = %d\n", s->armor);
        } else if (match(args[2], "tech")) {
            if (argn > 3) {
                s->tech = atof(args[3]);
            }

            sprintf(buf, "tech = %lf\n", s->tech);
        } else if (match(args[2], "whatdest")) {
            if (argn > 3) {
                s->whatdest = atoi(args[3]);
            }

            sprintf(buf, "whatdest = %d\n", s->whatdest);
        } else {
            notify(playernum, governor, "No such option for 'fix ship'.\n");
            free(s);

            return;
        }

        notify(playernum, governor, buf);
        putship(s);
        free(s);

        return;
    } else if (match(args[1], "star")) {
        if (Dir[playernum - 1][governor].level != LEVEL_STAR) {
            notify(playernum, governor, "Change scope to the star first.\n");

            return;
        }

        getstar(&Stars[Dir[playernum - 1][governor].snum],
                Dir[playernum - 1][governor].snum);

        if (match(args[2], "ships")) {
            if (argn > 3) {
                Stars[Dir[playernum - 1][governor].snum]->ships = atoi(args[3]);
            }

            sprintf(buf,
                    "ships = %d",
                    Stars[Dir[playernum - 1][governor].snum]->ships);
        } else if (match(args[2], "stab")) {
            if (argn > 3) {
                Stars[Dir[playernum - 1][governor].snum]->stability = atoi(args[3]);
            }

            sprintf(buf,
                    "stability = %d\n",
                    Stars[Dir[playernum - 1][governor].snum]->stability);
        } else if (match(args[2], "nova")) {
            if (argn > 3) {
                Stars[Dir[playernum - 1][governor].snum]->nova_stage = atoi(args[3]);
            }

            sprintf(buf,
                    "nova_stage = %d\n",
                    Stars[Dir[playernum - 1][governor].snum]->nova_stage);
        } else if (match(args[2], "temp")) {
            if (argn > 3) {
                Stars[Dir[playernum - 1][governor].snum]->temperature = atoi(args[3]);
            }

            sprintf(buf,
                    "temperature = %d\n",
                    Stars[Dir[playernum - 1][governor].snum]->temperature);
        } else if (match(args[2], "grav")) {
            if (argn > 3) {
                Stars[Dir[playernum - 1][governor].snum]->gravity = atof(args[3]);
            }

            sprintf(buf,
                    "gravity = %lf\n",
                    Stars[Dir[playernum - 1][governor].snum]->gravity);
        } else {
            notify(playernum, governor, "No such option for 'fix star'.\n");

            return;
        }

        notify(playernum, governor, buf);

        putstar(Stars[Dir[playernum - 1][governor].snum],
                Dir[playernum - 1][governor].snum);

        return;
    } else {
        notify(playernum, governor, "Fix what?\n");
    }
}

int match(char *p, char const *q)
{
    return !strncmp(p, q, strlen(p));
}

/* Case insensitive version of match -mfw */
int matchic(char *p, char const *q)
{
    return strncasecmp(p, q, strlen(p));
}

void DontOwnErr(int playernum, int governor, int shipno)
{
    sprintf(buf, "You don't own ship #%d.\n", shipno);
    notify(playernum, governor, buf);
}

int enufAP(int playernum, int governor, unsigned int ap, int x)
{
    int blah = 0;

    if (ap < x) {
        blah = 1;
        sprintf(buf, "You don't have %d action points there.\n", x);
        notify(playernum, governor, buf);
    }

    return !blah;
}

#ifdef CHAP_AUTH
int Getracenum(char *racename,
               char *govname,
               int *racenum,
               int *govnum,
               char *client_hash,
               int which)
{
    int i;
    int j;
    char server_hash[33];
    char hashbuf[MAX_COMMAND_LEN];

    /* Initialize server_hash */
    for (i = 0; i < 33; ++i) {
        server_hash[i] = '\0';
    }

    for (i = 1; i <= Num_races; ++i) {
        if (!strcmp(racename, races[i - 1]->name)) {
            *racenum = i;

            for (j = 0; j <= MAXGOVERNORS; ++j) {
                if (*races[i - 1]->governor[j].name
                    && !strcmp(govname, races[i - 1]->governor[j].name)) {
                    *govnum = j;

                    /* md5 check here */
                    sprintf(hashbuf,
                            "%s%s%s",
                            races[i - 1]->password,
                            races[i - 1]->governor[i].password,
                            des[which].key);

                    MD5String(hashbuf, server_hash);

                    /*
                     * Helpful debug output -mfw
                     *
                     * fprintf(stderr, "race_name: %s\n", races[i - 1]->name);
                     * fprintf(stderr,
                     *         "race_pass: %s\n",
                     *         races[i - 1]->password);
                     *
                     * fprintf(stderr,
                     *         "gov_name: %s\n",
                     *         races[i - 1]->governor[j].name);
                     *
                     * fprintf(stderr,
                     *         "go_pass: %s\n",
                     *         races[i - 1]->governor[j].password);
                     *
                     * fprintf(stderr, "key: %s\n", des[which].key);
                     * fprintf(stderr, "server_hash: %s\n", server_hash);
                     * fprintf(stderr, "client_hash: %s\n", client_hash);
                     */

                    if (!strcmp(server_hash, client_hash)) {
                        return 1;
                    }
                }
            }
        }
    }

    *govnum = 0;
    *racenum = *govnum;

    return 0;
}

#else

int Getracenum(char *racepass, char *govpass, int *racenum, int *govnum)
{
    int i;
    int j;

#if DEBUG
#endif

    for (i = 1; i <= Num_races; ++i) {
        if (!strcmp(racepass, races[i - 1]->password)) {
            *racenum = i;

            for (j = 0; j <= MAXGOVERNORS; ++j) {
                if (*races[i - 1]->governor[j].password
                    && !strcmp(govpass, races[i - 1]->governor[j].password)) {
                    *govnum = j;

                    return 1;
                }
            }
        }
    }

    *govnum = 0;
    *racenum = 0;

    return 0;
}
#endif

/* Returns player # from string contained that player's name or #. */
int GetPlayer(char *name)
{
    int rnum;
    int i;

    rnum = 0;

    if (isdigit((unsigned char)*name)) {
        rnum = atoi(name);

        if ((rnum < 1) || (rnum > Num_races)) {
            return 0;
        }

        return rnum;
    } else {
        for (i = 1; i <= Num_races; ++i) {
            if (match(name, races[i - 1]->name)) {
                return i;
            }
        }

        return 0;
    }
}

void allocateAPs(int playernum, int governor, int apcount)
{
    int maxalloc;
    int alloc;

    if (Dir[playernum - 1][governor].level == LEVEL_UNIV) {
        sprintf(buf,
                "Change scope to the system you which to transfer global APs to.\n");

        notify(playernum, governor, buf);

        return;
    }

    alloc = atoi(args[1]);

    if (alloc <= 0) {
        notify(playernum,
               governor,
               "You must specify a positive amount of APs to allocate.\n");

        return;
    }

    getsdata(&Sdata);

    maxalloc = MIN(Sdata.AP[playernum - 1],
                   LIMIT_APS - Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1]);

    if (alloc > maxalloc) {
        sprintf(buf, "Illegal value (%d) - maximum = %d\n", alloc, maxalloc);
        notify(playernum, governor, buf);

        return;
    }

    Sdata.AP[playernum - 1] -= alloc;
    putsdata(&Sdata);

    getstar(&Stars[Dir[playernum - 1][governor].snum],
            Dir[playernum - 1][governor].snum);

    Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1] =
        MIN(LIMIT_APS,
            Stars[Dir[playernum - 1][governor].snum]->AP[playernum - 1] + alloc);

    putstar(Stars[Dir[playernum - 1][governor].snum],
            Dir[playernum - 1][governor].snum);

    sprintf(buf, "Allocated\n");
    notify(playernum, governor, buf);
}

void deductAPs(int playernum, int governor, int n, int snum, int sdata)
{
    if (n) {
        if (!sdata) {
            getstar(&Stars[snum], snum);

            if (Stars[snum]->AP[playernum - 1] >= n) {
                Stars[snum]->AP[playernum - 1] -= n;
            } else {
                Stars[snum]->AP[playernum - 1] = 0;
                sprintf(buf,
                        "WHOA! You cheater! Oh! OH!\n I'm telling!!!\n");

                notify(playernum, governor, buf);
            }

            putstar(Stars[snum], snum);

            if ((Dir[playernum - 1][governor].level != LEVEL_UNIV)
                && (Dir[playernum - 1][governor].snum = snum)) {
                /* Fix the prompt */
                sprintf(Dir[playernum - 1][governor].prompt + 5,
                        "%02d",
                        Stars[snum]->AP[playernum - 1]);

                /* Fix bracket made '\0' by sprintf */
                Dir[playernum - 1][governor].prompt[7] = ']';
            }
        } else {
            getsdata(&Sdata);
            Sdata.AP[playernum - 1] = MAX(0, Sdata.AP[playernum - 1] - n);
            putsdata(&Sdata);

            if (Dir[playernum - 1][governor].level == LEVEL_UNIV) {
                sprintf(Dir[playernum - 1][governor].prompt + 2,
                        "%02d",
                        Sdata.AP[playernum - 1]);

                Dir[playernum - 1][governor].prompt[3] = ']';
            }
        }
    }
}

/* Lists all ships in current scope for debugging purposes */
void list(int playernum, int governor)
{
    shiptype *ship;
    planettype *p;
    int sh = -1;

    switch(Dir[playernum - 1][governor].level) {
    case LEVEL_UNIV:
        sh = Sdata.ships;

        break;
    case LEVEL_STAR:
        getstar(&Stars[Dir[playernum - 1][governor].snum],
                Dir[playernum - 1][governor].snum);

        sh = Stars[Dir[playernum - 1][governor].snum]->ships;

        break;
    case LEVEL_PLAN:
        getplanet(&p,
                  Dir[playernum - 1][governor].snum,
                  Dir[playernum - 1][governor].pnum);

        sh = p->ships;
        free(p);

        break;
    case LEVEL_SHIP:
        sh = Dir[playernum - 1][governor].shipno;

        break;
    }

    while (sh) {
        if (getship(&ship, sh)) {
            if (ship->alive) {
                sprintf(buf,
                        "%15s #%d '%s' (pl %d) -> #%d\n",
                        Shipnames[ship->type],
                        sh,
                        ship->name,
                        ship->owner,
                        ship->nextship);
            } else {
                sprintf(buf,
                        "%15s #%d '%s' (pl %d) -> #%d dead\n",
                        Shipnames[ship->type],
                        sh,
                        ship->name,
                        ship->owner,
                        ship->nextship);
            }

            notify(playernum, governor, buf);
            sh = nextship(ship);
            free(ship);
        } else {
            sh = 0;
        }
    }
}

double morale_factor(double x)
{
    return ((atan((double)x / 10000.0) / 3.14159565) + 0.5);
}

void no_permission(int playernum, int governor, char const *order, int level)
{
    char rank[9];

    switch (level) {
    case LEADER:
        strcpy(rank, "leader");

        break;
    case GENERAL:
        strcpy(rank, "general");

        break;
    case CAPTAIN:
        strcpy(rank, "captain");

        break;
    case PRIVATE:
        strcpy(rank, "private");

        break;
    case NOVICE:
        strcpy(rank, "novice");

        break;
    }

    sprintf(buf,
            "The \"%s\" command requires a minimum level of %s,\n",
            order,
            rank);

    notify(playernum, governor, buf);
    sprintf(buf, "or control of that ship/star to run.\n");
    notify(playernum, governor, buf);
}

int authorized(int governor, shiptype *ship)
{
    /* return (!governor || (ship->governor == governor)); */
    /* return (ship->governor == governor); */
    return ((ship->governor == governor)
            || (races[ship->owner - 1]->governor[governor].rank <= GENERAL));
}

int authorized_in_star(int playernum, int governor, startype *star)
{
    if (governor && (star->governor[playernum - 1] != governor)) {
        return 0;
    } else {
        return 1;
    }
}

int match2(char *a, char const *b, int n)
{
    return (!strncasecmp(a, b, strlen(a)) && (strlen(a) >= n));
}
