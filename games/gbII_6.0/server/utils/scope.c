/*
 * Until I can find this bug, this is just a workaround. However, sometimes the
 * 'ships' data file goes berserk and way too many ships are written out to
 * disk. This program, given a single numerical argument trims of any ships past
 * the ship number given. You'll have to use exship to find this particular ship
 * number. But bad ships are characterized by being a pod owned by race 0 gov 0.
 * When you find this ship, use that ship number as the argument you give to
 * this program and every ship past that will be erased from the db. -mfw
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../server/vars.h"
#include "../server/files.h"

#define DATAFILE DATA(sector)

int Fileread(int fd, sectortype *p, unsigned num, int posn);
void Filewrite(int fd, sectortype const *p, unsigned num, int posn);

int main(int argc, char *argv[])
{
    int i = 0;
    static int sect_fd;
    sectortype *sect;

    sect_fd = open(DATAFILE, 000, 0777);

    if (sect_fd < 0) {
        perror("main");
        printf("Unable to open %s\n", DATAFILE);

        exit(-1);
    }

    sect = malloc(sizeof(sectortype));

    while (Fileread(sect_fd, sect, sizeof(sectortype), i * sizeof(sectortype))) {
        if (!sect->eff
            && !sect->fert
            && !sect->mobilization
            && !sect->crystals
            && !sect->owner
            && !sect->race
            && !sect->type
            && !sect->condition
            && !sect->resource
            && !sect->popn
            && !sect->troops) {
            printf("Bad record found at position %d\n", i);

            break;
        }

        ++i;
    }

    close(sect_fd);
    free(sect);
    printf("Size of sectortype: %lu bytes.\n", sizeof(sectortype));

    exit(0);
}

int Fileread(int fd, sectortype *p, unsigned num, int posn)
{
    int n2;
    char buf[1024];

    if (lseek(fd, posn, L_SET) < 0) {
        sprintf(buf, "Fileread seek");
        perror(buf);

        return FAIL;
    }

    n2 = read(fd, p, num);

    if (n2 != num) {
        sprintf(buf, "Fileread read");
        perror(buf);

        return FAIL;
    }

    return SUCCESS;
}

void Filewrite(int fd, sectortype const *p, unsigned num, int posn)
{
    int n2;
    char errmsg[1024];

    if (lseek(fd, posn, L_SET) < 0) {
        sprintf(errmsg, "Filewrite 1");
        perror(errmsg);

        return;
    }

    n2 = write(fd, p, num);

    if (n2 != num) {
        perror("Filewrite 2");

        return;
    }
}
