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
 * #ident  "@(#)lists.c  1.5 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/lists.c,v 1.4 2007/07/06 17.30.26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: lists.c,v $ $Revision: 1.4 $";
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
/* #include <strings.h> */

#include "proto.h"
#include "power.h"
#include "races.h"
#include "shipdata.h"
#include "ships.h"
#include "vars.h"

int ShipVector[NUMSTYPES];

/* Utilities for dealing with ship lists */
void insert_sh_univ(struct stardata *sdata, shiptype *s)
{
    s->nextship = sdata->ships;
    sdata->ships = s->number;
    s->whatorbits = LEVEL_UNIV;
}

void insert_sh_star(startype *star, shiptype *s)
{
    s->nextship = star->ships;
    star->ships = s->number;
    s->whatorbits = LEVEL_STAR;
}

void insert_sh_plan(planettype *pl, shiptype *s)
{
    s->nextship = pl->ships;
    pl->ships = s->number;
    s->whatorbits = LEVEL_PLAN;
}

void insert_sh_ship(shiptype *s, shiptype *s2)
{
    s->nextship = s2->ships;
    s2->ships = s->number;
    s->whatorbits = LEVEL_SHIP;
    s->whatdest = LEVEL_SHIP;
    s->destshipno = s2->number;
}

void remove_sh_star(shiptype *s)
{
    int sh;
    shiptype *s2;

    getstar(&Stars[s->storbits], (int)s->storbits);
    sh = Stars[s->storbits]->ships;

    if (sh == s->number) {
        Stars[s->storbits]->ships = nextship(s);
        putstar(Stars[s->storbits], (int)s->storbits);
    } else {
        while (sh && (sh != s->number)) {
            if (getship(&s2, sh)) {
                sh = nextship(s2);

                if (sh != s->number) {
                    free(s2);
                }
            }
        }

        if (sh) {
            s2->nextship = nextship(s);
            putship(s2);
            free(s2);
        } else {
            loginfo(ERRORLOG,
                    NOERRNO,
                    "ship %d not found in the star list in should have been in",
                    s->number);
        }
    }

    s->whatorbits = LEVEL_UNIV;
    s->nextship = 0;
}

void remove_sh_plan(shiptype *s)
{
    int sh;
    shiptype *s2;
    planettype *p;

    getplanet(&p, (int)s->storbits, (int)s->pnumorbits);
    sh = p->ships;

    if (sh == s->number) {
        p->ships = nextship(s);
        putplanet(p, (int)s->storbits, (int)s->pnumorbits);
    } else {
        while (sh && (sh != s->number)) {
            if (getship(&s2, sh)) {
                sh = nextship(s2);

                if (sh != s->number) {
                    /* Don't free it if it is the s2 we want */
                    free(s2);
                }
            }
        }

        if (sh) {
            s2->nextship = nextship(s);
            putship(s2);
            free(s2);
        } else {
            loginfo(ERRORLOG,
                    NOERRNO,
                    "ship %d not found in the planet list it should have been in",
                    s->number);
        }
    }

    free(p);
    s->nextship = 0;
    s->whatorbits = LEVEL_UNIV;
}

void remove_sh_ship(shiptype *s, shiptype *ship)
{
    int sh;
    shiptype *s2;

    sh = ship->ships;

    if (sh == s->number) {
        ship->ships = nextship(s);
    } else {
        while (sh && (sh != s->number)) {
            if (getship(&s2, sh)) {
                sh = nextship(s2);

                if (sh != s->number) {
                    free(s2);
                }
            }
        }

        if (sh) {
            s2->nextship = nextship(s);
            putship(s2);
            free(s2);
        } else {
            loginfo(ERRORLOG,
                    NOERRNO,
                    "ship %d not found in the ship list it should have been in",
                    s->number);
        }
    }

    s->nextship = 0;

    /* Put in limbo - wait for insert_sh_*...*/
    s->whatorbits = LEVEL_UNIV;
}

void insert_sh_fleet(int player, int gov, shiptype *s, int fl)
{
    racetype *r;

    r = races[player - 1];

    /* First ship in fleet, appoint the admiral */
    if (!r->flee[fl].flagship) {
        r->fleet[fl].admiral = gov;
        strcpy(r->fleet[fl].name, "\0");
    }

    s->fleetmember = fl;
    s->nextinfleet = r->fleet[fl].flagship;
    r->fleet[fl].flagship = s->number;
    putrace(r);
}

void remove_sh_fleet(int player, int gov, shiptype *s)
{
    shiptype *s2;
    racetype *r;
    int sh;
    int fl;
    int ok;
    int found;

    r = races[player - 1];
    fl = s->fleetmember;

    if (fl <= 0) {
        s->fleetmember = 0;
        s->nextinfleet = 0;

        return;
    }

    sh = r->fleet[fl].flagship;

    if (!sh) {
        s->fleetmember = 0;
        s->nextinfleet = 0;

        return;
    }

    if (sh == s->number) {
        r->fleet[fl].flagship = s->nextinfleet;
    } else {
        found = 0;

        while (sh && (sh != s->number)) {
            ok = getship(&s2, sh);
            sh = s2->nextinfleet;

            if (ok && (sh != s->number)) {
                free(s2);
            } else if(ok && (sh == s->number)) {
                found = 1;

                break;
            }
        }

        if (found) {
            s2->nextinfleet = s->nextinfleet;
            putship(s2);
            free(s2);
        }
    }

    s->fleetmember = 0;
    s->nextinfleet = 0;

    /* No more ships, remove the admiral */
    if (!r->fleet[fl].flagship) {
        r->fleet[fl].admiral = 0;
        strcpy(r->fleet[fl].name, "\0");
    }

    putship(s);
    putrace(r);
}

double GetComplexity(int ship)
{
    shiptype s;

    s.armor = Shipdata[ship][ABIL_ARMOR];

    if (Shipdata[ship][ABIL_PRIMARY]) {
        s.guns = PRIMARY;
    } else {
        s.guns = NONE;
    }

    s.primary = Shipdata[ship][ABIL_GUNS];
    s.primtype = Shipdata[ship][ABIL_PRIMARY];
    s.secondary = Shipdata[ship][ABIL_GUNS];

    if (Shipdata[ship][ABIL_SECONDARY]) {
        s.sectype = SECONDARY;
    } else {
        s.sectype = NONE;
    }

    s.max_crew = Shipdata[ship][ABIL_MAXCREW];
    s.max_resource = Shipdata[ship][ABIL_CARGO];
    s.max_hanger = Shipdata[ship][ABIL_HANGER];
    s.max_destruct = Shipdata[ship][ABIL_DESTCAP];
    s.max_fuel = Shipdata[ship][ABIL_FUELCAP];
    s.max_speed = Shipdata[ship][ABIL_SPEED];
    s.build_type = ship;
    s.mount = Shipdata[ship][ABIL_MOUNT];
    s.hyper_drive.has = Shipdata[ship][ABIL_JUMP];
    s.cloak = 0;
    s.laser = Shipdata[ship][ABIL_LASER];
    s.cew = 0;
    s.cew_range = 0;
    s.size = ship_size(&s);
    s.base_mass = getmass(&s);
    s.mass = getmass(&s);

    return complexity(&s);
}

int ShipCompare(void const *s1, void const *s2)
{
    return (int)(GetComplexity(*(int *)s1) - GetComplexity(*(int *)s2));
}

void SortShips(void)
{
    int i;

    for (i = 0; i < NUMSTYPES; ++i) {
        ShipVector[i] = i;
    }

    qsort(ShipVector, NUMSTYPES, sizeof(inf), ShipCompare);
}
