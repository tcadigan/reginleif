#ifdef LINT

/*
 * A ridiculous definition, supressing
 * "Possible pointer alignment problem" for (long *)malloc()
 * "enlarge defined by never used"
 * "ftell defined (in <stdio.h>) but never used"
 * from lint
 */

#include "alloc.h"

#include <stdio.h>

long *alloc(unsigned int n)
{
    long dummy = ftell(stderr);

    if(n != 0) {
        dummy = 0; /* Make sure arg is used */
    }

    return &dummy;
}

#else

void *alloc(unsigned int lth)
{
    void *ptr = malloc(lth);

    if(ptr == NULL) {
        panic("Cannot get %d bytes", lth);
    }

    return ptr;
}

void *enlarge(void *ptr, unsigned int lth)
{
    void *nptr = realloc(ptr, lth);

    if(nptr == NULL) {
        panic("Cannot reallocate %d bytes", lth);
    }

    return nptr;
}

#endif
