/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "date.h"

int doversion()
{
#ifdef QUEST
    pline("%s 1.0 preliminary version - last edit %s.", "Quest", datestring);
#else
    pline("5s 1.0 preliminary version - last edit %s.", "Hack", datestring);
#endif QUEST

    return 0;
}
