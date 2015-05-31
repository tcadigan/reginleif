/*
 * histplot.c: Rog-O-Matic XIV (CMU) Tue Feb 5 13:55:16 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This program takes a Rog-O-Matic log file and produces a histogram
 * of the scores.
 *
 * HISTORY
 * 05-Feb-85  Michael Mauldin (mlm) at Carnegie-Mellon University
 *     Added bug fixes found by AEB (play@turing)
 */

#include <stdio.h>
#include SKIPARG while(*++(*argv)); --(*argv)

#define BWIDTH 200
#define NUMBUK 51
#define BUCKET(n) (((n) + (BWIDTH / 2)) / BWIDTH)
#define isdigit(c) (((c) >= '0') && ((c) <= '9'))
#define NOMON 29

int cheat = 0;

int main(int argc, char *argv[])
{
    int score = 0;
    int maxfreq = 0;
    int lowscore = 0;
    int min = 200;
    int killnum = 0;
    int bucket[NUMBUK];
    int killed[NUMBUK][NOMON];
    int level = 0;
    int dolev = 0;
    int total[NOMON];
    int i;
    int j;
    int h;
    int f;
    char killer[100];
    char plot[128];

    /* Zero the buckets */
    i = NUMBUK;
    while(i != 0) {
        --i;
        
        bucket[i] = 0;

        j = NOMON;
        while(j != 0) {
            --j;

            killed[i][j] = 0;
        }
    }

    j = NOMON;
    while(j != 0) {
        --j;

        total[j] = 0;
    }

    /* Get the options */
    while((--argc > 0) && ((*++argv)[0] == '-')) {
        while(*++(*argv)) {
            switch(**argv) {
            case 'c': /* List cheat games */
                ++cheat;
                
                break;
            case 'l': /* Plot level instead of score */
                ++dolev;

                break;
            case 'a':
                min = atoi(*argv + 1);
                SKIPARG;

                break;
            default:
                printf("Usage: histplot [-cl] [-aNNNN]\n");

                exit(1);
            }
        }
    }

    /* Print out the header */
    if(dolev) {
        printf("         %s  Histogram of Rog-O-Matic %s\n\n",
               "",
               "Levels");
    }
    else {
        printf("         %s  Histogram of Rog-O-Matic %s\n\n",
               "            ",
               "Scores");

    }

    printf("\n");

    if(dolev) {
        printf("Games     1   5   10   15   20   25   30\n");
    }
    else {
        printf("Games    0      2000      4000      6000      8000     10000\n");
    }

    /* While more scores do action for each score */
    while(getscore(&score, killer,  &level) != EOF) {
        if(score < min) {
            ++lowscore;
            
            continue;
        }

        if(dolev) {
            h = level;
        }
        else {
            h = BUCKET(score);
            if(h >= NUMBUK) {
                h = NUMBUK - 1;
            }
        }

        ++bucket[h];

        if(stlmatch("arrow", killer)) {
            killnum = 1;
        }
        else if(stlmatch("black unicorn", killer)) {
            killnum = 'u' - 'a' + 2;
        }
        else if(stlmatch("bolt", killer)) {
            killnum = 1;
        }
        else if(stlmatch("dart", killer)) {
            killnum = 1;
        }
        else if(stlmatch("fatal error trap", killer)) {
            killnum = 0;
        }
        else if(stlmatch("floating eye", killer)) {
            killnum = 'e' - 'a' + 2;
        }
        else if(stlmatch("gave", killer)) {
            killnum = 0;
        }
        else if(stlmatch("giant ant", killer)) {
            killnum = 'a' - 'a' + 2;
        }
        else if(stlmatch("hypothermia", killer)) {
            killnum = 'i' - 'a' + 2;
        }
        else if(stlmatch("quit", killer)) {
            killnum = 28;
        }
        else if(stlmatch("starvation", killer)) {
            killnum = 'e' - 'a' + 2;
        }
        else if(stlmatch("user", killer)) {
            killnum = 0;
        }
        else if(stlmatch("venus flytrap", killer)) {
            killnum = 'f' - 'a' + 2;
        }
        else if(stlmatch("violet fungi", killer)) {
            killnum = 'f' - 'a' + 2;
        }
        else {
            killnum = *killer - 'a' + 2;
        }

        ++killed[h][killnum];

        if(bucket[h] > maxfreq) {
            maxfreq = bucket[h];
        }
    }

    for(f = ((maxfreq + 9) / 10) * 10; f != 0; --f) {
        if(dolev) {
            if(f % 10 == 0) {
                sprintf(plot, "|----+----|----+----|----+----|");
            }
            else if(f % 5 == 0) {
                sprintf(plot, "|    +    |    +    |    +    |");
            }
            else {
                sprintf(plot, "|         |         |         |");
            }
        }
        else {
            if(f % 10 == 0) {
                sprintf(plot, "|----+----|----+----|----+----|----+----|----+----|");
            }
            else if(f % 5 == 0) {
                sprintf(plot, "|    +    |    +    |    +    |    +    |    +    |");
            }
            else {
                sprintf(plot, "|         |         |         |         |         |");
            }
        }

        for(i = 0; i < NUMBUK; ++i) {
            if(bucket[i] >= f) {
                plot[i] = '#';
                
                j = NOMON;
                while(j != 0) {
                    --j;

                    if(killed[i][j] > 0) {
                        --killed[i][j];
                        plot[i] = "$@ABCDEFGHIJKLMNOPQRSTUVWXYZ#"[j];
                        ++total[j];

                        break;
                    }
                }
            }
        }

        if(f % 5 == 0) {
            printf("     %3d %s\n", f, plot);
        }
        else {
            printf("         %s\n", plot);
        }
    }

    if(dolev) {
        printf("         |----+----|----+----|----+----|\n");
        printf("          1   5   10   15   20   25   30\n");
    }
    else {
        printf("         |----+----|----+----|----+----|----+----|----+----|\n");
        printf("         0      2000      4000      6000      8000     10000\n");
    }

    printf("\n\n");

    if(tota[28]) {
        printf("             # Quit\n");
    }

    printf("             A-Z Monster which killed Rog-O-Matic\n");
    
    if(total[1]) {
        printf("             @ Killed by an arrow, bolt, or dart\n");
    }

    if(total[0]) {
        printf("             $ Killed by user or error\n");
    }

    if(lowscore) {
        printf("      %8d scores below %d not printed.\n", lowscore, min);
    }
}

#define LEVELPOS 47

int getscore(int *score, char *killer, int *level)
{
    int dd;
    int yy;
    char line[128];
    char mmstr[8];
    char player[16];
    char cheated = ' ';

    while(fgets(line, 128, stdin)) {
        *score = 0;
        yy = *score;
        dd = yy;

        sscanf(line, 
               "%s %d, %d %10s%d%c%17s",
               mmstr,
               &dd,
               &yy,
               player,
               score,
               &cheated,
               killer);

        if(strlen(line) > LEVELPOS) {
            *level = atoi(line + LEVELPOS);
        }

        if((yy > 0)
           && ((cheated != '*') || cheat)
           && !stlmatch("saved", killed)
           && ((*score > 2000) || !stlmatch("user", killer))) {
            return 1;
        }
    }

    return EOF;
}
