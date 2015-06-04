/*
 * gene.c: Rog-O-Matic XIV (CMU) Sat Jul 5 23:47:33 1986 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * Initialize and summarize the gene pool
 *
 * EDITLOG
 *  LastEditDate = Sat Jul 5 23:47:33 1986 - Michael Mauldin
 *  LastFileName = /usre2/mlm/src/rog/ver14/gene.c
 *
 * HISTORY
 *  5-Jul-86  Michael Mauldin (mlm) at Carnegie-Mellon University
 *     Created.
 */

#include <stdio.h>
#include "types.h"
#include "install.h"

int knob[MAXKNOB];
char *knob_name[MAXKNOB] = {
    "trap searching:   ",
    "door searching:   ",
    "resting:          ",
    "using arrows:     ",
    "experimenting:    ",
    "retreating:       ",
    "waking monsters:  ",
    "hoarding food:    "
};

char genelock[100];
char genelog[100];
char genepool[100];

int main(int argc, char *argv[])
{
    int m = 10;
    int init = 0;
    int seed = 0;
    int version = RV53A;
    int full = 0;

    /* Get the options */
    while((--argvc > 0) && (*++argv[0] == '-')) {
        while(*++(*argv)) {
            switch(**argv) {
            case 'a':
                full = 2;

                break;
            case 'i':
                ++init;
                
                break;
            case 'f':
                full = 1;

                break;
            case 'm':
                m = atoi(*argv + 1);
                SKIPARG;
                printf("Gene pool size %d.\n", m);

                break;
            case 's':
                seed = atoi(*argv + 1);
                SKIPARG;
                printf("Random seed %d.\n", m);

                break;
            case 'v':
                version = atoi(*argv + 1);
                SKIPARG;
                printf("Rogue version %d.\n", version);

                break;
            default:
                quit(1, "Usage: gene [-if] [-msv<value>] [genepool]\n");
            }
        }
    }

    if(argc > 0) {
        if(readgenes(argv[0])) { /* Read the gene pool */
            /* Print a summary */
            analyzepool(full);
        }
        else {
            fprintf(stderr, "Cannot read file '%s'\n", argv[0]);
        }

        exit(0);
    }

    /* No file argument, assign the gene log and pool file names */
    sprintf(genelock, "%s/GeneLock%d", RGMDIR, version);
    sprintf(genelog, "%s/GeneLog%d", RGMDIR, version);
    sprintf(genepool, "%s/GenePool%d", RGMDIR, version);

    /* Disable interrupts */
    critical();
    
    if(lock_file(genelock, MAXLOCK)) {
        if(init) {
            /* Set the random number generator */
            srand(seed);

            /* Open the gene log file */
            openlog(genelog);

            /* Random starting point */
            initpool(MAXKNOB, m);

            /* Write out the gene pool */
            writegenes(genepool);

            /* Close the log file */
            closelog();
        }
        else if(!readgenes(genepool)) { /* Read the gene pool */
            quit(1, "Cannot read file '%s'\n", genepool);
        }

        unlock_file(genelock);
    }
    else {
        quit(1, "Cannot access file '%s'\n", genepool);
    }

    /* Re-enable interrupts */
    uncritical();

    /* Print a summary */
    analyzepool(full);

    return 0;
}
