/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "config.h"
#include "def.func_tab.h"
#include "hack.apply.h"
#include "hack.do.h"
#include "hack.do_name.h"
#include "hack.do_wear.h"
#include "hack.eat.h"
#include "hack.end.h"
#include "hack.engrave.h"
#include "hack.options.h"
#include "hack.invent.h"
#include "hack.read.h"
#include "hack.save.h"
#include "hack.search.h"
#include "hack.shk.h"
#include "hack.topl.h"
#include "hack.version.h"
#include "hack.whatis.h"
#include "hack.wield.h"
#include "hack.zap.h"

struct func_tab list[] = {
    { '\022', doredraw },
    { '\020', doredotopl },
    { 'a', doapply } , 
    /* 'A': UNUSED */
    /* 'b': go sw */
    /* 'B': go sw */
    { 'c', ddocall }, 
    { 'C', do_mname },
    { 'd', dodrop },
    { 'D', doddrop },
    { 'e', doeat },
    { 'E', doengrave },
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
    { 'o', doset },
#endif
    /* 'O': UNUSED */
    { 'p', dopay },
    { 'P', dowearring },
    { 'q', dodrink },
    { 'Q', done2 },
    { 'r', doread },
    { 'R', doremring },
    { 's', dosearch },
    { 'S', dosave },
    { 't', dothrow },
    { 'T', doremarm },
    /* 'u': go ne */
    /* 'U': go ne */
    { 'v', doversion },
    /* 'V': UNUSED */
    { 'w', dowield },
    { 'W', doweararm },
    /* 'x': UNUSED */
    /* 'X': UNUSED */
    /* 'y': go nw */
    /* 'Y': go nw */
    { 'z', dozap },
    /* 'Z', UNUSED */
    { '<', doup },
    { '>', dodown },
    { '/', dowhatis },
    { '?', dohelp },
#ifdef SHELL
    { '!', dosh },
#endif
    { '.', donull },
    { ' ', donull },
    { '^', doidtrap },
    { WEAPON_SYM, doprwep },
    { ARMOR_SYM, doprarm },
    { RING_SYM, doprring },
    { 0, 0 }
};
