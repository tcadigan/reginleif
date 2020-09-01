/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.options.h"

#include <curses.h>

#include "config.h"
#include "hack.topl.h"
#include "hack.tty.h"

#ifdef OPTIONS
#include "hack.h"

int doset()
{
    int flg = 1;
    char buf[BUFSZ];
    char *str;

    pline("What option do you want to set? [(!)eo] ");
    getlin(buf);
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
	    echo();
        }
        else {
	    noecho();
        }

        break;
    case 'o':
        flags.oneline = flg;

        break;
    default:
        pline("Unknown option '%s'", str);
    }

    return 0;
}
#endif
