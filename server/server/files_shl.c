/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chanksy, et al.
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
 * Franklin Street, 5th Floor, Boston, Ma 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * Disk input/output routines and misc stuff. All read routines lock the data
 * they just access (the file is not closed). Write routines close and thus
 * unlock that area.
 *
 * #ident  "@(#)files_shl.c     1.6 12/1/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/files_shl.c,v 1.4 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)       $RCSfile: files_shl.c,v $ $Revision: 1.4 $";
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "GB_copyright.h"
#include "buffers.h"
#include "power.h"
#include "proto.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

#define SHIP_CONSISTENCY

extern int errno;
extern unsigned short free_ship_list;

/* Comment this out if sys_errlist is not in libc -mfw */
/* extern char *sys_errlist[]; */

/* int sys_nerr; */
int commoddata;
int pdata;
int racedata;
int sectdata;
int shdata;
int stdata;
unsigned short free_ship_list;

void close_file(int);
void open_data_files(void);
void close_data_files(void);
void openstardata(int *);
void openshdata(int *);
void opencommoddata(int *);
void openpdata(int *);
void opensectdata(int *);
void openracedata(int *);
void getsdata(struct stardata *);
void getrace(racetype **, int);
void getstar(startype **, int);
void getplanet(planettype **, int, int);
int getship(shiptype **, int);
int getcommod(commodtype **, int);
int getsector(sectortype **, planettype *, int, int);
int getsmap(sectortype *, planettype *);
int getdeadship(void);
int getdeadcommod(void);
void putsdata(struct stardata *);
void putrace(racetype *);
void putstar(startype *, int);
void putplanet(planettype *, int, int);
void putsector(sectortype *, planettype *, int, int);
void putsmap(sectortype *, planettype *);
void putship(shiptype *);
void putcommod(commodtype *, int);
int Numraces(void);
int Numcommods(void);
int Newslength(int);
void clr_shipfree(void);
void clr_commodfree(void);
void makeshipdead(int);
void makecommoddead(int);
void Putpower(struct power[MAXPLAYERS]);
void GetPower(struct power[MAXPLAYERS]);
void Putblock(struct block[MAXPLAYERS]);
void Getblock(struct block[MAXPLAYERS]);
int nextship(shiptype *);

void close_file(int fd)
{
    close(fd);
}

void create_pid_file(void)
{
    /* Create pid file - mfw */
    FILE *fp;

    fp = fopen(LOG(GB_server.pid), "w+");

    if (fp != 0) {
        fprintf(fp, "%d", (int)getpid());
        fclose(fp);
    } else {
        fprintf(stderr, "Unable to create %s\n", LOG(GB_server.pid));

        exit(1);
    }
}

void open_data_files(void)
{
    opencommoddata(&commoddata);
    openpdata(&pdata);
    openracedata(&racedata);
    opensectdata(&sectdata);
    openshdata(&shdata);
    openstardata(&stdata);
}

void close_data_files(void)
{
    close_file(commoddata);
    close_file(pdata);
    close_file(racedata);
    close_file(sectdata);
    close_file(shdata);
    close_file(stdata);
}

void delete_pid_file(void)
{
    /* Remove pid file -mfw */
    if (remove(LOG(gb.pid)) != 0) {
        fprintf(stderr, "Unable to remove %s\n", LOG(GB_server.pid));
    }
}

void openstardata(int *fd)
{
    /* printf(" openstardata\n"); */
    *fd = open(STARDATAFL, O_RDWR, 0777);

    if (*fd < 0) {
        perror("openstardata");
        printf("unable to open%s\n", STARDATAFL);

        exit(-1);
    }
}

void openshdata(int *fd)
{
    *fd = open(SHIPDATAFL, O_RDWR, 0777);

    if (*fd < 0) {
        perror("openshdata");
        printf("unable to open %s\n", SHIPDATAFL);

        exit(-1);
    }
}

void opencommoddata(int *fd)
{
    *fd = open(COMMODDATAFL, O_RDWR, 0777);

    if (*fd < 0) {
        perror("opencommoddata");
        printf("unable to open %s\n", COMMODDATAFL);

        exit(-1);
    }
}

void openpdata(int *fd)
{
    *fd = open(PLANETDATAFL, O_RDWR, 0777);

    if (*fd < 0) {
        perror("openpdata");
        printf("unable to open %s\n", PLANETDATAFL);

        exit(-1);
    }
}

void opensectdata(int *fd)
{
    *fd = open(SECTORDATAFL, O_RDWR, 0777);

    if (*fd < 0) {
        perror("opensectdata");
        printf("unable to open %s\n", SECTORDATAFL);

        exit(-1);
    }
}

void openracedata(int *fd)
{
    *fd = open(RACEDATAFL, O_RDWR, 0777);

    if (*fd < 0) {
        perror("openrdata");
        printf("unable to open %s\n", RACEDATAFL);

        exit(-1);
    }
}

void getsdata(struct stardata *s)
{
    Fileread(stdata, (char *)s, sizeof(struct stardata), 0);
}

void getrace(racetype **r, int rnum)
{
    *r = (racetype *)malloc(sizeof(racetype));

    if (!*r) {
#ifdef ENROL
        printf("getrace: Error in malloc\n");

#else

        loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc failure [getrace]");
#endif

        exit(1);
    }

    Fileread(racedata,
             (char *)r,
             sizeof(racetype),
             (rnum - 1) * sizeof(racetype));
}

void getstar(startype **s, int star)
{
    if ((s < &Stars[0]) || (s >= &Stars[NUMSTARS - 1])) {
        *s = (startype *)malloc(sizeof(startype));

        if (!*s) {
#ifdef ENROL
            printf("getstar: Error in malloc\n");

#else

            loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc failure [getstar]");
#endif

            exit(1);
        }
    }

    Fileread(stdata,
             (char *)s,
             sizeof(startype),
             (int)(sizeof(Sdata) + (star * sizeof(startype))));
}

void getplanet(planettype **p, int star, int pnum)
{
    int filepos;

    if ((p < &planets[0][0]) || (p >= &planets[NUMSTARS - 1][MAXPLANETS - 1])) {
        /* Allocate space for others */
        *p = (planettype *)malloc(sizeof(planettype));

        if (!*p) {
#ifdef ENROL
            printf("getplanet: Error in malloc\n");

#else

            loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc failure [getplanet]");
#endif

            exit(1);
        }
    }

    filepos = Stars[star]->planetpos[pnum];
    Fileread(pdata, (char *)p, sizeof(planettype), filepos);
}

int getsector(sectortype **s, planettype *p, int x, int y)
{
    int filepos;

    filepos = p->sectormappos + (((y * p->Maxx) + x) * sizeof(sectortype));
    *s = (sectortype *)malloc(sizeof(sectortype));

    if (!*s) {
#ifdef ENROL
        printf("getsector: Error in malloc\n");

#else

        loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc failure [getsector]");
#endif

        exit(1);
    }

    if (!Fileread(sectdata, (char *)*s, sizeof(sectortype), filepos)) {
        free(*s);
        return FAIL;
    } else {
        return SUCCESS;
    }
}

int getsmap(sectortype *map, planettype *p)
{
    if (!Fileread(sectdata, (char *)map, (p->Maxx * p->Maxy * sizeof(sectortype)), p->sectormappos)) {
        return FAIL;
    } else {
        return SUCCESS;
    }
}

int getship(shiptype **s, int shipnum)
{
    struct stat buffer;

    if (shipnum <= 0) {
        return 0;
    }

    fstat(shdata, &buffer);

    if ((buffer.st_size / sizeof(shiptype)) < shipnum) {
        return 0;
    } else {
        *s = (shiptype *)malloc(sizeof(shiptype));

        if (!*s) {
#ifdef ENROL
            printf("getship: Error in malloc\n");

#else

            loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc failure [getship]");
#endif

            exit(1);
        }

        Fileread(shdata,
                 (char *)*s,
                 sizeof(shiptype),
                 (shipnum - 1) * sizeof(shiptype));

        return 1;
    }
}

int getcommod(commodtype **c, int commodnum)
{
    struct stat buffer;

    if (commodnum <= 0) {
        return 0;
    }

    fstat(commoddata, &buffer);

    if ((buffer.st_size / sizeof(commodtype)) < commodnum) {
        return 0;
    } else {
        *c = (commodtype *)malloc(sizeof(commodtype));

        if (!*c) {
#ifdef ENROL
            printf("getcommod: Error in malloc\n");

#else

            loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc failure [getcommod]");
#endif

            exit(1);
        }

        Fileread(commoddata,
                 (char *)*c,
                 sizeof(commodtype),
                 (commodnum - 1) * sizeof(commodtype));

        return 1;
    }
}

/*
 * Gets the ship # listed in the top of the file SHIPFREEDATAFL. This might have
 * no other uses besides build().
 */
int getdeadship(void)
{
    struct stat buffer;
    short shnum;
    int fd;
    int abort;

    fd = open(SHIPFREEDATAFL, O_RDWR, 0777);

    if (fd < 0) {
        perror("getdeadship");
        printf("unable to open %s\n", SHIPFREEDATAFL);

        exit(-1);
    }

    abort = 1;
    fstat(fd, &buffer);

    if (buffer.st_size && (abort == 1)) {
        /* Put topmost entry in fpos */
        Fileread(fd,
                 (char *)&shnum,
                 sizeof(short),
                 buffer.st_size - sizeof(short));

        /* Erase that entry, since it will now be filled */
        ftruncate(fd, (long)(buffer.st_size - sizeof(short)));
        close_file(fd);

        return (int)shnum;
    } else {
        close_file(fd);
    }

    return -1;
}

int getdeadcommod(void)
{
    struct stat buffer;
    short commodnum;
    int fd;
    int abort;

    fd = open(COMMODFREEDATAFL, O_RDWR, 0777);

    if (fd < 0) {
        perror("getdeadcommod");
        printf("unable to open %s\n", COMMODFREEDATAFL);

        exit(-1);
    }

    abort = 1;
    fstat(fd, &buffer);

    if (buffer.st_size && (abort == 1)) {
        /* Put topmost entry in fpos */
        Fileread(fd,
                (char *)&cmmodnum,
                sizeof(short),
                buffer.st_size - sizeof(short));

        /* Erase that entry, since it will now be filled */
        ftruncate(fd, (long)(buffer.st_size - sizeof(short)));
        close_file(fd);

        return (int)commodnum;
    } else {
        close_file(fd);
    }

    return -1;
}

void putsdata(struct stardata *s)
{
    Filewrite(stdata, (char *)s, sizeof(struct stardata), 0);
}

void putrace(racetype *r)
{
    Filewrite(racedata,
              (char *)r,
              sizeof(racetype),
              (r->Playernum - 1) * sizeof(racetype));
}

void putstar(startype *s, int snum)
{
    Filewrite(stdata,
              (char *s)s,
              sizeof(startype),
              (int)(sizeof(Sdata + (snum * sizeof(startype)))));
}

void putplanet(planettype *p, int star, int pnum)
{
    int filepos;

    filepos = Stars[star]->planetpos[pnum];
    Filewrite(pdata, (char *)p, sizeof(planettype), filepos);
}

void putsector(sectortype *s, planettype *p, int x, int y)
{
    int filepos;

    filepos = p->sectormappos + (((y * p->Maxx) + x) * sizeof(sectortype));
    Filewrite(sectdata, (char *)s, sizeof(sectortype), filepos);
}

void putsmap(sectortype *map, planettype *p)
{
    Filewrite(sectdata,
              (char *)map,
              p->Maxx * p->Maxy * sizeof(sectortype),
              p->sectormappos);
}

void putship(shiptype *s)
{
    if (s->number > 0) {
        Filewrite(shdata,
                  (char *)s,
                  sizeof(shiptype),
                  (s->number - 1) * sizeof(shiptype));
    }
}

void putcommod(commodtype *c, int commodnum)
{
    Filewrite(commoddata,
              (char *)c,
              sizeof(commodtype),
              (commodnum - 1) * sizeof(commodtype));
}

int Numraces(void)
{
    struct stat buffer;

    fstat(racedata, &buffer);

    return (int)(buffer.st_size / sizeof(racetype));
}

/* Return number of ships */
int Numships(void)
{
    struct stat buffer;
    long int num;

    fstat(shdata, &buffer);
    num = (long int)(buffer.st_size / sizeof(shiptype));

    /* Temporary test for now, to help find bloat bug XXX */
    if (num > 10000) {
        loginfo(ERRORLOG, NOERRNO, "Numships(): Ship soft limit exceeded.");
    }

    return num;
}

int Numcommods(void)
{
    struct stat buffer;

    fstat(commoddata, &buffer);

    return (int)(buffer.st_size / sizeof(commodtype));
}

int Newslength(int type)
{
    struct stat buffer;
    FILE *fp;

    switch (type) {
    case DECLARATION:
        fp = fopen(DECLARATIONFL, "r");

        if (fp == NULL) {
            fp = fopen(DECLARATIONFL, "w+");
        }

        break;
    case TRANSFER:
        fp = fopen(TRANSFERFL, "r");

        if (fp == NULL) {
            fp = fopen(TRANSFERFL, "w+");
        }

        break;
    case COMBAT:
        fp = fopen(COMBATFL, "r");

        if (fp == NULL) {
            fp = fopen(COMBATFL, "w+");
        }

        break;
    case ANNOUNCE:
        fp = fopen(ANNOUNCEFL, "r");

        if (fp == NULL) {
            fp = fopen(ANNOUNCEFL, "w+");
        }

        break;
    default:

        break;
    }

    fstat(filno(fp), &buffer);
    fclose(fp);

    return (int)buffer.st_size;
}

/* Delete contents of dead ship file */
void clr_shipfree(void)
{
    fclose(fopen(SHIPFREEDATAFL, "w+"));
}

void clr_commodfree(void)
{
    fclose(fopen(COMMODFREEDATAFL, "w+"));
}

/* Writes the ship to the dead ship file at its end. */
void makeshipdead(int shipnum)
{
    int fd;
    unsigned short shipno;
    struct stat buffer;

    /* Convert to u_short */
    shipno = shipnum;

    if (shipno == 0) {
        return;
    }

    fd = open(SHIPFREEDATAFL, O_WRONLY, 0777);

    if (fd < 0) {
        printf("fd = %d\n", fd);
        printf("errno = %d\n", errno);
        perror("openshfdata");
        printf("unable to open %s\n", SHIPFREEDATAFL);

        exit(-1);
    }

    /* Write the ship # at the very end of SHIPFREEDATAFL */
    fstat(fd, &buffer);
    FileWrite(fd, (char *)&shipno, sizeof(shipno), buffer.st_size);
    close_file(fd);
}

void makecommoddead(int commodnum)
{
    int fd;
    unsigned short cmoodno;
    struct stat buffer;

    /* Convert to u_short */
    commodno = commodnum;

    if (commodno == 0) {
        return;
    }

    fd = open(COMMODFREEDATAFL, O_WRONLY, 0777);

    if (fd < 0) {
        printf("fd = %d\n", fd);
        printf("errno = %d\n", errno);
        perror("opencommodfdata");
        printf("unable to open %s\n", COMMODFREEDATAFL);

        exit(-1);
    }

    /* Write the commod # at the very end of the COMMODFREEDATAFL */
    fstat(fd, &buffer);
    Filewrite(fd, (char *)&commodno, sizeof(commodno), buffer.st_size);
    close_file(fd);
}

void Putpower(struct power p[MAXPLAYERS])
{
    int power_fd;

    power_fd = open(POWFL, O_RDWR, 0777);

    if (power_fd < 0) {
        perror("open power data");
        printf("unable to open %s\n", POWFL);

        return;
    }

    write(power_fd, (char *)p, sizeof(*p) * MAXPLAYERS);
    close_file(power_fd);
}

void GetPower(struct power p[MAXPLAYERS])
{
    int power_fd;

    power_fd = open(POWFL, O_RDONLY, 0777);

    if (power_fd < 0) {
        perror("open power data");
        printf("unable to open %s\n", POWFL);

        return;
    } else {
        read(power_fd, (char *)p, sizeof(*p) * MAXPLAYERS);
        close_file(power_fd);
    }
}

void Putblock(struct block b[MAXPLAYERS])
{
    int block_fd;

    block_fd = open(BLOCKDATAFL, O_RDWR, 0777);

    if (block_fd < 0) {
        perror("open block data");
        printf("unable to open %s\n", BLOCKDATAFL);

        return;
    }

    write(block_fd, (char *)b, sizeof(*b) * MAXPLAYERS);
    close_file(block_fd);
}

void Getblock(struct block b[MAXPLAYERS])
{
    int block_fd;

    block_fd = open(BLOCKDATAFL, O_RDONLY, 0777);

    if (block_fd < 0) {
        perror("open block data");
        printf("unable to open %s\n", BLOCKDATAFL);

        return;
    } else {
        read(block_fd, (char *)b, sizeof(*b) * MAXPLAYERS);
        close_file(block_fd);
    }
}

int getFreeShip(int type, int player)
{
    shiptype *s;
    int rval = 0;

    if ((player == 0) && (type == 0)) {
        return rval;
    }

    /* Check for a free governor ship number */
    if (type == OTYPE_GOV) {
        if (getship(&s, player)) {
            if (!s->owner && !s->alive) {
                rval = player;
            }

            free(s);

            if (rval > 0) {
                return rval;
            }
        }
    }

    if (free_ship_list) {
        /* Try to read free ship */
        if (getship(&s, free_ship_list)) {
            /* Check if it's really free */
            if (!s->alive && s->reuse) {
                /* OK, update lists */
                rval = free_ship_list;
                free_ship_list = nextship(s);
                s->nextship = 0;
            } else {
                /* Darn! List is fucked up */
                loginfo(ERRORLOG,
                        NOERRNO,
                        "Non-free ship %d in free ship list",
                        free_ship_list);

                free_ship_list = 0;
            }

            free(s);
        } else {
            /* Darn! List is fucked up */
            loginfo(ERRORLOG,
                    NOERRNO,
                    "Cannot getship() ship %d in free ship list",
                    free_ship_list);

            free_ship_list = 0;
        }
    } else {
        /* Ship slots still available? */
        rval = Numships();

        if (rval < SHIP_NUM_LIMIT) {
            if (rval < FIRST_USER_SHIP) {
                rval = FIRST_USER_SHIP;
            } else {
                rval += 1;
            }
        } else {
            rval = 0;
        }
    }

    return rval;
}

int initFreeShipList(void)
{
    int i;
    int n = Numships();
    shiptype **s;

    Num_ships = n;

    s = (shiptype **)malloc(sizeof(shiptype *) * (Num_ships + 1));

    for (i = 1; i <= n; ++i) {
        getship(&s[i], i);
    }

    doFreeShipList(s, FIRST_USER_SHIP, n);

    for (i = 1; i <= n; ++i) {
        putship(s[i]);
        free(s[i]);
    }

    free(s);

    return 1;
}

int doFreeShipList(shiptype **s, int low, int high)
{
    int i;
    int tail = 0;

    free_ship_list = 0;

    for (i = low; i <= high; ++i) {
        if (s[i]->reuse && !s[i]->alive) {
            /* If ship is dead and reusable add it to free ship list */
            s[i]->nextship = tail;
            tail = i;
            free_ship_list = tail;
        }
    }

    return 1;
}

int shipSlotsAvail(void)
{
    return (free_ship_list || (Numships() <= SHIP_NUM_LIMIT));
}

/*
 * Occasionally we'd get some weirdness in ship numbers, not sure what the cause
 * is so far, however, I've added this function to do a little sanity checking
 * on the ship number returned. -mfw
 */
int nextship(shiptype *s)
{
    int n;

    n = s->nextship;

    if (n > Numships()) {
        loginfo(ERRORLOG,
                NOERRNO,
                "nextship value higher than total ships in ship #%d",
                s->number);

        return 0;
    } else if (n == s->number) {
        loginfo(ERRORLOG,
                NOERRNO,
                "nextship points to itself in ship #%d",
                s->number);

        return 0;
    } else {
        return n;
    }
}
