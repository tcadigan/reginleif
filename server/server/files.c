/*
 * Galactic Bloodshed, a multi-player 4X game of space conquest
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
 * Franklin street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * #ident  "@(#)files.c 1.9 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/files.c,v 1.3 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: files.c,v $ $Revision: 1.3 $";
 */

#include "GB_copyright.h"
#include "files.h"

char const *Files[] = {
    PATH(Data),
    PATH(Help),
    MISC(exam.dat),
    PATH(enroll.dat),
    DATA(star),
    DATA(sector),
    DATA(planet),
    DATA(race),
    DATA(block),
    DATA(ship),
    DATA(shipfree),
    DATA(dummy),
    DATA(players),
    PATH(Msgs),
    MSGS(tele),
    DATA(power),
    PATH(News),
    NEWS(declaration),
    NEWS(transfer),
    NEWS(combat),
    NEWS(announce),
    DATA(commod),
    DATA(commodfree),
    DATA(Update.time),
    DATA(Segment.time),
    LOG(error.log),
    LOG(user.log),
    LOG(update.log),
    LOG(error.dig),
    LOG(user.dig),
    LOG(update.dig),
    MISC(backup.sh),
    BACK(Backups),
    DATA(Backups),
    MISC(welcome.txt),
    MISC(motd.txt),
    MISC(bugrep.txt),
    MISC(schedule.cfg),
    MSGS(disp)
};
