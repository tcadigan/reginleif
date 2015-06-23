/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "config.h"

#ifdef OPTIONS
#include "hack.h"

void doset()
{
    int flg = 1;
    char buf[BUFSZ];
    char *str;

    plint("What option do you want to set? [(!)eo] ");
    gelin(buf);
    str = buf;

    while(*str == ' ') {
        ++str;
    }

    if(*str == '!') {
        flg = 0;
        ++str;
    }

    switch(*str) {
    case 'e':
        flags.echo = flg;
        
        if(flg) {
            echo(ON);
        }
        else {
            echo(OFF);
        }

        break;
    case 'o':
        flags.online = flg;

        break;
    default:
        pline("Unknown option '%s'", str);
    }

    return 0;
}
#endif
