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
 * #ident  "@(#)get4args.c      1.5 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/get4args.c,v 1.3 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: get4args.c,v $ $Revision: 1.3 $";
 */
#include "get4args.h"

#include <stdio.h>

void get4args(char *s, int *xl, int *xh, int *yl, int *yh)
{
    char *p;
    char s1[17];
    char s2[17];

    p = s;

    sscanf(p, "%[^,]", s1);

    while ((*p != ':') && (*p != ',')) {
        ++p;
    }

    if (*p == ':') {
        sscanf(s1, "%d:%d", xl, xh);

        while (*p != ',') {
            ++p;
        }
    } else if (*p == ',') {
        sscanf(s1, "%d", xl);
        *xh = *xl;
    }

    sscanf(p, "%s", s2);

    while ((*p != ':') && (*p != '\0')) {
        ++p;
    }

    if (*p == ':') {
        sscanf(s2, ",%d:%d", yl, yh);
    } else {
        sscanf(s2, ",%d", yl);
        *yh = *yl;
    }
}
