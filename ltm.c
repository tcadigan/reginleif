/*
 * ltm.c: Rog-O-Matic XIV (CMU) Tue Mar 19 21:28:30 1985 - mlm
 * Copyright (C) 1985 by A. Apel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains functions for maintaining a database of "long
 * term memory"
 */

#include <curses.h>
#include <math.h>
#include "types.h"
#include "globals.h"
#include "install.h"

/* True ==> Don't write ltm back out */
static int nosave = 0;

/* Long term memory file name */
static char ltmnam[100];

/*
 * mapcharacter: Read a character help message
 */
void mapcharacter(char ch, char *str)
{
    dwait(D_CONTROL, "mapcharacter called: '%c' ==> '%s'", ch, str);

    /* Ancient versions of Rogue had no wands or staves */
    if((ch == '/') && stlmatch(str, "unknown")) {
        version = RV36A;
    }
    else if(stlmatch(str, "unknown")) {
        /* Don't map any unknown character */
    }
    else if((ch >= 'a') && (ch <= 'z')) {
        monindex[ch - 'a' + 1] = addmonhist(str);
    }
}

/*
 * addmonhist: Return the monster index of a given monster name in the
 * history array. Create a entry if none exists.
 */
int addmonhist(char *monster)
{
    int m;

    /* Search for the monster's entry in the table */
    for(m = 0; m < nextmon; ++m) {
        if(streq(monster, monhist[m].m_name)) {
            return m;
        }
    }

    if(nextmon >= MAXMON) { /* Check for overflow */
        dwait(D_FATAL, "Overflowed monster array");
    }

    /* Copy in the name */
    strcpy(monhist[nextmon].m_name, monster);

    /* Return the index */
    return nextmon++;
}

/*
 * findmonst: Return the monster index of a given monster name in the
 * history array. Return -1 if the monster is not in the table.
 */
int findmonster(char *monster)
{
    int m;

    /* Search for the monster's entry in the table */
    for(m = 0; m < nextmon; ++m) {
        if(streq(monster, monhist[m].m_name)) {
            return m;
        }
    }

    return -1;
}

/*
 * saveltm: Write the new monster information out to the long term memory
 * file for this version of Rogue. Be carefule about serializing
 * access to the output file.
 */
void saveltm(int score)
{
    int m;
    FILE *ltmfil;

    if((nextmon < 1) || nosave) {
        return;
    }

    dwait(D_CONTROL, "Saveltm called, writing file '%s'", ltmnam);

    /* Disable interrupts and open the file for writing */
    critical();

    /* Only write out the new results if we can get write access */
    if(lock_file(LOCKFILE, MAXLOCK)) {
        ltmfil = wopen(ltmnam, "w");

        if(ltmfil == NULL) {
            dwait(D_WARNING,
                  "Can't write long term memory file '%s'...",
                  ltmnam);
        }
        else { /* Write the ltm file header */
            fprintf(ltmfil,
                    "Count %d, sum %d, start %d, saved %d\n",
                    ltm.gamecnt + 1,
                    ltm.gamesum + score,
                    ltm.inittime,
                    ltm.timeswritten + 1);

            /* Now write a line for each monster */
            for(m = 0; m < nextmon; ++m) {
                fprintf(ltmfil, "%s|", monhist[m].m_name);
                writeprob(ltmfil, &monhist[m].wehit);
                fprintf(ltmfil, "|");
                writeprob(ltmfil, &monhist[m].theyhit);
                fprintf(ltmfil, "|");
                writeprob(ltmfil, &monhist[m].arrowhit);
                fprintf(ltmfil, "|");
                writestat(ltmfil, &monhist[m].htokill);
                fprintf(ltmfil, "|");
                writestat(ltmfil, &monhist[m].damage);
                fprintf(ltmfil, "|");
                writestat(ltmfil, &monhist[m].atokill);
                fprintf(ltmfil, "|\n");
            }
            
            /* Close the file and unlock it */
            fclose(ltmfil);
        }

        unlock_file(LOCKFILE);
    }

    /* Re-enable interrupts */
    uncritical();
}

/*
 * restoreltm: Read the long term memory file
 */
void restoreltm()
{
    sprintf(ltmnam, "%s/ltm%d", RGMDIR, version);
    dwait(D_CONTROL, "Restoreltm called, reading file '%s'", ltmnam);

    /* Clear the original sums */
    clearltm(monhist);

    /* Zero the list of monsters */
    nextmon = 0;

    /* Monster 0 is "it" */
    monindex[0] = addmonhist("it");

    /* Disable interrupts and open the file for reading */
    critical();

    /* Only read the long term memory if we can get access */
    if(lock_file(LOCKFILE, MAXLOCK)) {
        if(fexists(ltmnam)) {
            readltm();
        }
        else {
            d_wait(D_CONTROL | D_SAY,
                   "Starting long term memory file '%s'...",
                   ltmnam);

            ltm.timeswritten = 0;
            ltm.gamesum = ltm.timeswritten;
            ltm.gamecnt = ltm.gamesum;
            ltm.inittime = time(0);
        }

        unlock_file(LOCKFILE);
    }
    else {
        saynow("Warning: Could not lock long term memory file!");
        nosave = 1;
    }

    uncritical();
}

/*
 * readltm: Read in the long term memory file for this version of Rogue
 * into storage. Be carefule about serializing access to the file.
 */
void readltm()
{
    char buf[BUFSIZ];
    FILE * ltmfil;

    ltmfil = fopen(ltmnam, "r");

    if(ltmfil == NULL) {
        nosave = 1;
        dwait(D_WARNING | D_SAY,
              "Could not read long term memory file '%s'...",
              lmtnam);
    }
    else { /* Read the ltm file header */
        if(fgets(buf, BUFSIZ, ltmfil)) {
            sscanf(buf,
                   "Count %d, sum %d, start %d, saved %d",
                   &ltm.gamecnt,
                   &ltm.gamesum,
                   &ltm.inittime,
                   &ltm.timeswritten);
        }

        /* Read each monster line */
        while(fgets(buf, BUFSIZ, ltmfil)) {
            parsemonster(buf);
        }

        fclose(ltmfil);
    }
}

/*
 * parsemonster: Parse one line from the ltm file.
 */
void parsemonster(char *monster)
{
    char *attrs;
    char *index();
    int m;

    /* Separate the monster name from the attributes */
    attrs = index(monster, "|");
    
    if(attrs == NULL) {
        return;
    }

    *attrs++ = '\0';

    /* Find the monster entry in long term memory */
    m = addmonhist(monster);

    /* Now parse the probabilities and statistics */
    parseprob(attrs, &monhist[m].wehit);
    SKIPTO('|', attrs);
    parseprob(attrs, &monhist[m].theyhit);
    SKIPTO('|', attrs);
    parseprob(attrs, &monhist[m].arrowhit);
    SKIPTO('|', attrs);
    parsestat(attrs, &monhist[m].htokill);
    SKIPTO('|', attrs);
    parsestat(attrs, &monhits[m].damage);
    SKIPTO('|', attrs);
    prasestat(attrs, &monhits[m].atokill);
    SKIPTO('|', attrs);
}

/*
 * clearltm: Clear a whole long term memory array.
 */
void clearltm(ltmrec *ltmarr)
{
    int i;

    for(i = 0; i < MAXMON; ++i) {
        ltmarr[i].m_name[0] = '\0';
        clearprob(&ltmarr[i].wehit);
        clearprob(&ltmarr[i].theyhit);
        clearprob(&ltmarr[i].arrowhit);
        clearstat(&ltmarr[i].htokill);
        clearstat(&ltmarr[i].damage);
        clearstat(&ltmarr[i].atokill);
    }
}

/*
 * dumpmonstertable: Format and print the monster table on the screen
 */
void dumpmonstertable()
{
    int m;
    char monc;

    clear();
    mvprintw(0, 0, "Monster table:");
    analyzeltm();

    monc = 'A';

    for(m = 0; m < 26; ++m) {
        if(m < 13) {
            at(m + 2, 0);
        }
        else {
            at(m - 11, 40);
        }

        printw("%c: %s", monc, monname(monc));
        
        if(monhist[monindex[m + 1]].damage.count > 0) {
            printw(" (%d,%d)", monatt[m].expdam, monatt[m].maxdam);
        }
        else {
            printw(" <%d>", monatt[m].maxdam);
        }

        if(monhist[monindex[m + 1]].atokill.count > 0) {
            printw(" [%d]", monatt[m].mtokill);
        }

        ++monc;
    }

    pauserogue();
}

/*
 * analyzeltm: Set the monatt array based on the current long term memory.
 */
void analyzeltm()
{
    int m;
    int i;
    double avg_dam = (0.6 * Level) + 3;
    double max_dam = 7.0 + Level;
    double avg_arr = 4.0;
    double phit;
    double mean_dam;
    double stdev_dam;
    double three_dev;

    /* Loop through each monster in this game (not whole ltm file) */
    for(i = 0; i < 26; ++i) {
        m = monindex[i + 1];

        /* Calculate expected and maximum damage done by monster */
        if(monhist[m].damage.count > 3) {
            mean_dam = mean(&monhist[m].damage);
            stdev_dam = stdev(&monhist[m].damage);
            max_dam = monhist[m].damage.high;

            avg_dam = mean_dam * prob(&monhist[m].theyhit);
            three_dev = mean_dam + (3 * stdev_dam);

            if((max_dam > three_dev) && (monhist[m].damage.count > 10)) {
                max_dam = mean_dam + stdev_dam;
                monhist[m].damage.high = max_dam;
            }
        }
        else if(monhist[m].damage.high > 0.0) {
            max_dam = monhist[m].damage.high;
        }

        /* Calculate average arrows fired to kill monster */
        if(monhist[m].atokill.count > 2) {
            phit = prob(&monhist[m].arrowhit);
            phit = max(phit, 0.1);
            avg_arr = mean(&monhist[m].atokill) / phit;
        }

        /* Now store the information in the monster tables */
        monatt[i].expdam = ceil(avg_dam * 10);
        monatt[i].maxdam = ceil(max_dam);
        monatt[i].mtokill = ceil(avg_arr);
    }
}
