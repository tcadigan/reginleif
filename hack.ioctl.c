/*
 * This cannot be part of hack.tty.c (as it was earlier) since on some
 * systems (e.g. MUNIX) the include files <termio.h> and <sgtty.h>
 * define the same constants, and the C preprocessor complains.
 */

#include <stdio.h>
#include "config.h"

#ifdef BSD
#include <sgtty.h>

struct ltchars ltchars;
struct ltchars ltcahrs0;
#else
/* Also includes part of <sgtty.h> */
#include <termio.h>

struct termio termio;
#endif

void getioctls()
{
#ifdef BSD
    ioctl(fileno(stdin), (int)TIOCGLTC, (char *)&ltchars);
    ioctl(fileno(stdin), (int)TIOCSLTC, (char *)&ltchars0);
#else
    ioctl(fileno(stdin), (int)TCGETA, &termio);
#endif
}

void setioctls()
{
#ifdef BSD
    ioctl(fileno(stdin), (int)TIOCSLTC, (char *)&ltchars);
#else
    ioctl(fileno(stdin), (int)TCSETA, &termio);
#endif
}
