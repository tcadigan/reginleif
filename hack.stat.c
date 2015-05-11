/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include <sys/types.h>
#include <sys/stat.h>

extern char *index();

struct stat buf;
struct stat hbuf;

void gethdate(char *name)
{
    char *np;
    if(stat(name, &hbuf) != 0) {
        np = index(name, '/');

        if(np != 0) {
            error("Cannot get status of %s.", np + 1);
        }
        else {
            error("Cannot get status of %s.", name);
        }
    }
}

int uptodate(int fd)
{
    if(fstat(fd, &buf) != 0) {
        pline("Cannot get status?");
        
        return 0;
    }

    if(buf.st_ctime < hbuf.st_ctime) {
        pline("Saved level is out of date.");

        return 0;
    }

    return 1;
}
              
