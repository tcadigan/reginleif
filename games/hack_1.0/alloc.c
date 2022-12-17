#include "alloc.h"

#include <stdio.h>
#include <stdlib.h>

#include "hack.pri.h"

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
