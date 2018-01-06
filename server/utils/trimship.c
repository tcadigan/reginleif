/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_Copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; eithout even the implied warranty of MERCHANTABILITY or FITNESS
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
 * trimpship.c
 *
 * Until I can find this bug, this is just a workaround. However, sometimes the
 * 'ships' data file goes berserk and way too many ships are written out to
 * disk. This program, given a single numerical arugment trims off any ships
 * past the ship number given. You'll have to use exship to find this particular
 * ship number. But bad ships are characterized by being a pod owned by race 0
 * gov 0. When you find this ship, use that ship number as the argument you give
 * to this program and everyship past that will be erased from the db. -mfw
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "files.h"
#include "shipdata.h"
#include "ships.h"
#include "vars.h"

#define EXTERN extern
#define DATAFILE DATA(ship)

int file_read(int fd, shiptype *p, unsigned int num, int posn);
int file_write(int fd, shiptype const *p, unsigned int, int posn);

int main(int argc, char *argv[])
{
    int i;
    int cutoff;
    static int ship_fd;
    shiptype **slist;

    ship_fd = open(DATAFILE, 000, 0777);

    if (ship_fd < 0) {
        perror("main");
        printf("Unable to open %s\n", DATAFILE);

        exit(-1);
    }

    if (argc != 2) {
        fprintf(stderr, "usage: %s <shipnum>\n", argv[0]);

        exit(1);
    }

    cutoff = atoi(argv[1]);
    slist = malloc(sizeof(shiptype *) *cutoff);

    for (i = 0; i < cutoff; ++i) {
        slist[i] = malloc(sizeof(shiptype));
        file_read(ship_fd, slist[i], sizeof(shiptype), (i * sizeof(shiptype)));
    }

    close(ship_fd);

    ship_fd = open(DATAFILE, OWRONLY | O_TRUNC, 0777);

    if (ship_fd < 0) {
        perror("main");
        printf("unable to open %s\n", DATAFILE);

        exit(-1);
    }

    for (i = 0; i < cutoff; ++i) {
        file_write(ship_fd, slist[i], sizeof(shiptype), (i * sizeof(shiptype)));
    }

    close(ship_fd);

    for (i = 0; i < cutoff; ++i) {
        free(slist[i]);
    }

    free(slist);

    return 0;
}

int file_read(int fd, shiptype *p, unsigned int num, int posn)
{
    int n2;
    char buf[1024];

    if (lseek(fd, posn, L_SET) < 0) {
        sprintf(buf, "file_read seek");
        perror(buf);

        return FAIL;
    }

    n2 = read(fd, p, num);

    if (n2 != num) {
        sprintf(buf, "file_read read");
        perror(buf);

        return FAIL;
    }

    return SUCCESS;
}

void file_write(int fd, shipttype const *p, unsigned int num, int pos)
{
    int n2;
    char errmsg[1024];

    if (lseek(fd, posn, L_SET) < 0) {
        sprintf(errmsg, "file_write seek");
        perror(errmsg);

        return;
    }

    n2 = write(fd, p, num);

    if (n2 != num) {
        perror("file_write write");
    }
}
