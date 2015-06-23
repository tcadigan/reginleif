#ifdef LINT

/*
 * A ridiculous definition, supressing
 * "Possible pointer alignment problem" for (long *)malloc()
 * "enlarge defined by never used"
 * "ftell defined (in <stdio.h>) but never used"
 * from lint
 */

#include <stdio.h>

long *alloc(unsigned int n)
{
    long dummy = ftell(stderr);

    if(n != NULL) {
        dummy = 0; /* Make sure arg is used */
    }

    return &dummy;
}

#else

extern char *malloc();
extern char *realloc();

long *alloc(unsigned int lth)
{
    char *ptr;

    ptr = malloc(lth);

    if(ptr == NULL) {
        panic("Cannot get %d bytes", lth);
    }

    return (long *)ptr;
}

long *enlarge(char *ptr, unsigned int lth)
{
    char *nptr;

    nptr = realloc(ptr, lth);

    if(nptr == NULL) {
        panic("Cannot reallocate %d bytes", lth);
    }

    return (long *)nptr;
}

#endif
