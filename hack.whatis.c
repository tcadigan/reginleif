/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.whatis.h"

#include <stdio.h>

#include "hack.h"
#include "hack.main.h"
#include "hack.pri.h"
#include "hack.topl.h"
#include "hack.tty.h"

int dowhatis(char *str)
{
    FILE *fp;
    char bufr[BUFSZ];
    char *ep;
    char q;

    pline("Specify what? ");
    getlin(bufr);
    str = bufr;

    while(*str == ' ') {
        ++str;
    }

    q = *str;

    if(*(str + 1)) {
        pline("One character please.");
    }
    else {
        fp = fopen("data", "r");
        if(fp == 0) {
            pline("Cannot open data file!");
        }
        else {
            while(fgets(bufr, BUFSZ, fp) != 0) {
                if(*bufr == q) {
                    ep = index(bufr, '\n');
                    
                    if(ep != NULL) {
                        *ep = 0;
                    }
                    else {
                        impossible();
                    }

                    pline(bufr);

                    if(ep[-1] == ';') {
                        morewhat(fp);
                    }

                    fclose(fp);

                    return 0;
                }
            }
            
            pline("Unknown symbol.");
        }

        fclose(fp);
    }

    return 0;
}
 
void morewhat(FILE *fp)
{
    char bufr[BUFSZ];
    char *ep;

    pline("More info? ");
    
    if(readchar() != 'y') {
        return;
    }

    cls();

    while((fgets(bufr, BUFSZ, fp) != 0) && (*bufr == '\t')) {
        ep = index(bufr, '\n');
    
        if(ep == NULL) {
            break;
        }

        *ep = 0;

        puts(bufr + 1);
    }

    more();
    docrt();
}

    
