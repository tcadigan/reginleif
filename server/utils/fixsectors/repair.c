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
#include <strings.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define EXTERN extern
#include "vars.h"
#include "files.h"

#define DATAFILE "sector"

int Fileread(int fd, sectortype *p, unsigned num, int posn);
void Filewrite(int fd, sectortype const *p, unsigned num, int posn);

int main(int argc, char *argv[])
{
    int i = 0;
    static int sect_id;
    sectortype *sect;

    sect_id = open(DATAFILE, 000, 0777);

    if (sect_fd < 0) {
	perror("main");
	printf("Unable to open %s\n", DATAFILE);

	exit(-1);
    }

    sect = malloc(sizeof(sectortype) * 15666);
    Fileread(sect_fd, sect, (sizeof(sectortype) * 15666), 0);
    close(sect_fd);

    sect_fd = open(DATAFILE, O_WRONLY | O_TRUNC, 0777);

    if (sect_fd < 0) {
	perror("main");
	printf("Unable to open %s\n", DATAFILE);

	exit(-1);
    }

    Filewrite(sect_fd, sect, (sizeof(sectortype) * 15666), 0);
    close(sect_fd);
    free(sect);

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
