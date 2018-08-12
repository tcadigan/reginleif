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
 * perm.c -- Randomly permute a sector list
 *
 * #ident  "@(#)perm.c  1.5 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/perm.c,v 1.3 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: perm.c,v $ $Revision: 1.3 $";
 */
#include "perm.h"

#include "power.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

struct map {
    char x;
    char y;
} xymap[(MAX_X + 1) * (MAX_Y + 1)];

void PermuteSects(planettype *);
int Getxysect(planettype *, int *, int *, int);

/* Make a random list of sectors. */
void PermuteSects(planettype *planet)
{
    int i;
    int j;
    int x;
    int y;
    int t;
    struct map sw;

    t = planet->Maxy * planet->Maxx;

    y = 0;
    x = 0;

    for (i = x; i < t; ++i) {
        xymap[i].x = x;
        xymap[i].y = y;
        ++x;

        if (x >= planet->Maxx) {
            x = 0;
            ++y;
        }
    }

    for (i = 0; i < t; ++i) {
        sw = xymap[i];
        j = int_rand(0, t - 1);
        xymap[i] = xymap[j];
        xymap[j] = sw;
    }
}

/*
 * Get the next x,y sector in the list. If r = 1, reset the counter. Increments
 * the counter and returns whether or not this reset it to 0.
 */
int Getxysect(planettype *p, int *x, int *y, int r)
{
    static int getxy;
    static int max;

    if (r) {
        getxy = 0;
        max = p->Maxx * p->Maxy;
    } else {
        *x = xymap[getxy].x;
        *y = xymap[getxy].y;
        ++getxy;

        if (getxy > max) {
            getxy = 0;
        }
    }

    return getxy;
}
