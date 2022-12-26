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
 * files_rw.c - Disk input/ouput routines
 *
 * Fileread(p, num, file, posn, routine);  -- Generic file read
 * Filewrite(p, num, file, posn, routine); -- Generic file write
 *
 * #ident  "@(#)files_rw.c      1.6 12/1/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/files_rw.c,v 1.3 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: files_rw.c,v $ $Revision: 1.3 $";
 */
#include "files_rw.h"

#include <errno.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "buffers.h"
#include "files.h"
#include "vars.h"

static char errmsg[255];

int Fileread(int fd, char *p, unsigned num, int posn)
{
    int n2;

    /*
     * Commented out for now, used in 'utils' so this function is undefined
     * there.
     */
     // debug(7, "Fileread() size: %d, position: %d\n", num, posn);

    if (lseek(fd, posn, L_SET) < 0) {
        sprintf(buf, "Fileread seek: %s", p);
        perror(buf);

        return FAIL;
    }

    n2 = read(fd, p, num);

    if (n2 != num) {
        sprintf(buf, "Fileread read: %s, wanted: %d got %d", p, num, n2);
        perror(buf);

        return FAIL;
    }

    return SUCCESS;
}

void Filewrite(int fd, char const *p, unsigned num, int posn)
{
    int n2;

    if (lseek(fd, posn, L_SET) < 0) {
        sprintf(errmsg, "Filewrite 1: posn %d, fd %d", posn, fd);
        perror(errmsg);

        return;
    }

    n2 = write(fd, p, num);

    if (n2 != num) {
        perror("Filewrite 2");

        return;
    }
}
