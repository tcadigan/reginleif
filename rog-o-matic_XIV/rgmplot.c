/*
 * rgmplot.c: Rog-O-Matic XIV (CMU) Tue Feb 5 15:00:59 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This program takes a Rog-O-Matic score file sorted by date and score,
 * and produces a scatter plot of the scores.
 */
#include "rgmplot.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utility.h"

#define WIDTH 50
#define AVLEN 7
#define SCALE(n) (((n) + 100) / 200)
#define isdigit(c) (((c) >= '0') && ((c) <= '9'))

char *month[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

int doavg = 0;
int cheat = 0;
int min = -1;

int main(int argc, char *argv[])
{
    int mm;
    int dd;
    int yy;
    int score = 0;
    int lastday = -1;
    int lastmon = -1;
    int lastyy = -1;
    int h;
    int sumscores = 0;
    int numscores = 0;
    int i;
    int sum[AVLEN];
    int num[AVLEN];
    int rsum;
    int rnum;
    int davg;
    int ravg;
    char player[100];
    char plot[128];
    char cheated;

    /* Clear out the rolling average statistics */
    for(i = 0; i < AVLEN; ++i) {
        num[i] = 0;
        sum[i] = 0;
    }

    /* Get the options */
    while((--argc > 0) && ((*++argv)[0] == '-')) {
        while(*++(*argv)) {
            switch(**argv) {
            case 'c': /* List cheat games */
                ++cheat;
                
                break;
            case 'a': /* Print average */
                ++doavg;

                break;
            default:
                printf("Usage: rgmplot [-ac] [minimum]\n");
                
                exit(1);
            }
        }
    }

    if(argc > 0) {
        min = atoi(argv[0]);
    }

    /* Print out the header */
    printf("\t\t   Scatter Plot of Rog-O-Matic scores versus time\n\n");
    if(min > 0) {
        printf("\t\t              Scores greater than %d\n\n", min);
    }

    printf("\t\t0      2000      4000      6000      8000     10000\n");
    printf("\t\t|----+----|----+----|----+----|----+----|----+----|\n");

    /* Build an empty plot line */
    strcpy(plot, "|                                                 |");

    /* While more scores do action for each score */
    while(getscore(&mm, &dd, &yy, player, &score, &cheated) != EOF) {
        /* Change days, overprint the average for day, rolling average */
        if(((dd != lastday) || (mm != lastmon) || (yy != lastyy))
           && (lastday > 0)) {
            if(doavg) {
                rsum = *sum;
                rnum = *num;

                for(i = 1; i < AVLEN; ++i) {
                    rsum += sum[i];
                    rnum += num[i];
                }

                if(*num > 0) {
                    davg = SCALE(*sum / *num);
                }
                else {
                    davg = 0;
                }
                
                if(rnum > 0) {
                    ravg = SCALE(rsum / rnum);
                }
                else {
                    ravg = 0;
                }
                
                /* Roll the daily average statistics */
                for(i = AVLEN - 1; i > 0; --i) {
                    sum[i] = sum[i - 1];
                    num[i] = num[i - 1];
                }

                *num = 0;
                *sum = 0;

                /* Print a '*' for the daily average */
                if((davg > 0) && (davg < WIDTH)) {
                    plot[davg] = '*';
                }

                /* Print a '###' for the rolling average */
                if((ravg > 0) && (ravg < (WIDTH - 1))) {
                    plot[ravg + 1] = '#';
                    plot[ravg] = plot[ravg + 1];
                    plot[ravg - 1] = plot[ravg];
                }
            }

            printf("%3s %2d %4d\n%s\n", 
                   month[lastmon - 1],
                   lastday, 
                   lastyy,
                   plot);

            strcpy(plot, "|                                                 |");
        }

        if(score > EOF) {
            h = SCALE(score);
            if(h >= WIDTH) {
                sprintf(plot, "%s %d", plot, score);
            }
            else if(plot[h] == '9') {
                /* Nothing */
            }
            else if(isdigit(plot[h])) {
                ++plot[h];
            }
            else {
                plot[h] = '1';
            }

            *sum += score;
            ++*num;

            sumscores += score;
            ++numscores;

            lastday = dd;
            lastmon = mm;
            lastyy = yy;
        }
    }

    printf("\t\t|----+----|----+----|----+----|----+----|----+----|\n");
    printf("\t\t0      2000      4000      6000      8000     10000\n");

    if(numscores > 0) {
        printf("\nAverage score %d, total games %d.\n\n",
               sumscores/numscores,
               numscores);
    }

    printf("1-9    Number of games in range.\n");
    
    if(doavg) {
        printf(" *     Average of day's scores.\n");
        printf("###    Rolling of %d day average.\n", AVLEN);
    }

    return 0;
}

int getlin(char *s)
{
    int ch;
    int i;
    static int endfile = 0;

    if(endfile) {
        return EOF;
    }

    i = 0;
    ch = getchar();
    while((ch != EOF) && (ch != '\n')) {
        s[i] = ch;
        ++i;
        ch = getchar();
    }

    s[i] = '\0';

    if(ch == EOF) {
        endfile = 1;
        strcpy(s, "-1 -1, -1 string -1 ");

        return 20;
    }

    return i;
}

int getscore(int *mm, int *dd, int *yy, char *player, int *score, char *cheated)
{
    char line[128];
    char reason[32];

    while(getlin(line) != EOF) {
        sscanf(line, 
               "%d %d, %d %10s%d%c%17s",
               mm,
               dd,
               yy, 
               player, 
               score,
               cheated,
               reason);

        if(((*score >= min) || (*score < 0))
           && ((*cheated != '*') || cheat)
           && !stlmatch(reason, "saved")
           && ((*score > 2000) || !stlmatch(reason, "user"))) {
            return 1;
        }
    }

    return EOF;
}
    
