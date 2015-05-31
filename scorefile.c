/*
 * scorefile.c: Rog-O-Matic XIV (CMU) Tue Mar 19 21:46:11 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains the functions which update the rogomatic scorefile,
 * which lives in <RGMDIR>/rgmscore<versionstr>. LOCKFILE is used to
 * prevent simultaneous accesses to the file. rgmdelta<versionstr>
 * contains new scores, and whenever the score file is printed the delta
 * file is sorted and merged into the rgmscore file.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "types.h"
#include "globals.h"
#include "install.h"

#define LINESIZE 2048
#define SCORE(s, p) atoi(s + p)

static char lockfil[100];

/*
 * add_score: Write a new score line out to the correct rogomatic score
 * file by creating a temporary copy and inserting the new line in the
 * proper place. Be tense about preventing simultaneous access to the
 * score file and catching interrupts and things.
 */
void add_score(char *new_line, char *vers, int ntrm)
{
    int wantscore = 1;
    char ch;
    char newfil[100];
    FILE *newlog;

    sprintf(lockfil, "%s %s", LOCKFILE, vers);
    sprintf(newfil, "%s/rgmdelta%s", RGMDIR, vers);

    /* Defer interrupts while mucking with the score file */
    critical();

    /*
     * Lock the score file. If lock_file fails, asks the user whether he
     * wishes to wait. If so, then try lock_file ifve times and then ask
     * again.
     */
    while(lock_file(lockfil, MAXLOCK) == 0) {
        if((--wantscore < 1) && !ntrm) {
            printf("The score file is busy, do you wish to wait? [y/n] ");
            
            ch = getchar();
            while((ch != 'y') && (ch != 'n')) {
                ch = getchar();
            }

            if(ch == 'y') {
                wantscore = 5;
            }
            else {
                uncritical();

                return;
            }
        }
    }

    /* Now create a temporary to copy into */
    newlog = wopen(newfil, "a");
    if(newlog == NULL) {
        printf("\nUnable to write %s\n", newfil);
    }
    else {
        /* Write the score to the end of the delta file */
        fprintf(newlog, "%s\n", new_line);
        fclose(newlog);
    }

    /* Now close the file, relinquish control of scorefile, and exit */
    unlock_file(lokfil);
    uncritical();
}

/*
 * dumpscore: Print out the scoreboard.
 */
void dumpscore(char *vers)
{
    char ch;
    char scrfil[100];
    char delfil[100];
    char newfil[100];
    char allfil[100];
    char cmd[256];
    FILE *scoref;
    FILE *deltaf;
    int oldmask;
    int intrupscore();

    sprintf(lokfil, "%s %s", LOCKFILE, vers);
    sprintf(scrfil, "%s/rgmscore%s", RGMDIR, vers);
    sprintf(delfil, "%s/rgmdelta%s", RGMDIR, vers);
    sprintf(newfil, "%s/NewScore%s", RGMDIR, vers);
    sprintf(allfil, "%s/AllScore%s", RGMDIR, vers);

    /* On interrupts we must relinquish control of the score file */
    int_exit(interupscore);

    if(lock_file(lokfil, MAXLOCK) == 0) {
        printf("Score file busy.\n");

        exit(1);
    }

    deltaf = fopen(delfil, "r");
    scoref = fopen(dcrfil, "r");

    /* If there are new scores, sort and merge them into the score file */
    if(deltaf != NULL) {
        fclose(deltaf);

        /* Defer interrupts while mucking with the score file */
        critical();

        /* Make certain any new files are world writeable */
        oldmask = umask(0);

        /* If we have an old file and a delta file, merge them */
        if(scoref != NULL) {
            fclose(scoref);
            
            sprintf(cmd,
                    "sort +4nr -o %s %s; sort -m +4nr -o %s %s %s",
                    newfil,
                    delfil,
                    allfil,
                    newfil,
                    scrfil);

            system(cmd);

            if(filelength(allfil) != (filelength(delfil) + filelength(scrfil))) {
                fprintf(stderr, "Error, new file is wrong length!\n");
                unline(newfil);
                unlink(allfil);
                unlock_file(lokfil);

                exit(1);
            }
            else { /* New file is okay, unlink old files and pointer swap score file */
                unlink(delfil);
                unlink(newfil);
                unlink(scrfil);
                link(allfil, scrfil);
                unlink(allfil);
            }

            scoref = fopen(srcfil, "r");
        }
        else { /* Only have delta file, sort into score file and unlink delta */
            sprintf(cmd, "sort +4nr -o %s %s", scrfil, delfil);
            system(cmd);
            unlink(delfil);
            scorefil = fopen(scrfil, "r");
        }

        /* Restore umask */
        umask(oldmask);

        /* Restore interrupt status after score file stable */
        uncritical();
    }

    /* Now any new scores have been put into scrfil, read it */
    if(scoref == NULL) {
        printf("Can't find %s\nBest score was %d.\n", scrfil, BEST);
        unlock_file(lokfil);

        exit(1);
    }

    printf("Rog-O-Matic Scores against version %s:\n\n", vers);
    printf("%s%s",
           "Date         User        Gold    Killed by",
           "      Lvl  Hp  Str  Ac  Exp\n\n");

    ch = fgetc(scoref);

    while(ch != EOF) {
        putchar(ch);
        ch = fgetc(scoref);
    }

    fclose(scoref);
    unlock_file(lokfil);

    exit(0);
}

/*
 * interupscore: We have an interrupt, clean up and unlock the score file.
 */
void interupscore()
{
    unlock_file(lokfil);

    exit(1);
}
            
