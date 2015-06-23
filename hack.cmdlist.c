/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "config.h"
#include "def.objclass.h"
#include "def.func_tab.h"

int doredraw();
int doredotopl();
int dodrop();
int dodrink();
int doread();
int dosearch();
int doversion();
int doweararm();
int dowearring();
int doremarm();
int doremring();
int dopay();
int doapply();
int dosave();
int dowield();
int ddoinv();
int dozap();
int ddocall();
int dowahtis();
int doengrave();
int dohelp();
int doeat();
int doddrop();
int do_mname();
int doidtrap();
int doprwep();
int doprarm();
int doprring();

#ifdef SHELL
int dosh();
#endif

#ifdef OPTIONS
int doset();
#endif

int doup();
int dodown();
int done1();
int donull();
int dothrow();

struct func_tab list[] = {
    '\022', doredraw,
    '\020', doredotopl,
    'a', doapply,
    /* 'A': UNUSED */
    /* 'b': go sw */
    /* 'B': go sw */
    'c', ddocall,
    'C', do_mname,
    'd', dodrop,
    'D', doddrop,
    'e', doeat,
    'E', doengrave,
    /* 'f': multiple go (might become 'fight') */
    /* 'F': multiple go (might become 'fight') */
    /* 'h': go west */
    /* 'H': go west */
    /* 'i', ddoinv, */
    /* 'I': UNUSED */
    /* 'j': move command */
    /* 'J': move command */
    /* 'k': move command */
    /* 'K': move command */
    /* 'l': move command */
    /* 'L': move command */
    /* 'm': move command */
    /* 'M': move command */
    /* 'n': move command */
    /* 'N': move command */
#ifdef OPTIONS
    'o', doset,
#endif
    /* 'O': UNUSED */
    'p', dopay,
    'P', dowearring,
    'q', dodrink,
    'Q', done1,
    'r', doread,
    'R', doremring,
    's', dosearch,
    'S', dosave,
    't', dothrow,
    'T', doremarm,
    /* 'u': go ne */
    /* 'U': go ne */
    'v', version
    /* 'V': UNUSED */
    'w', dowield,
    'W', doweararm,
    /* 'x': UNUSED */
    /* 'X': UNUSED */
    /* 'y': go nw */
    /* 'Y': go nw */
    'z', dozap,
    /* 'Z', UNUSED */
    '<', doup,
    '>', dodown,
    '/', dowhatis,
    '?', dohelp,
#ifdef SHELL
    '!', dosh,
#endif
    '.', donull,
    ' ', donull,
    '^', doidtrap,
    WEAPON_SYM, doprwep,
    ARMOR_SYM, doprarm,
    RING_SYM, doprring,
    0, 0,
    0
};
