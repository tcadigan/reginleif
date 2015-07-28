/*
 * fortune.c
 * Larn is copyrighted 1986 by Noah Morgan.
 *
 * Function to return a random fortune from the fortune file
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef BSD41
#include <fcntl.h>

#else

#define O_RDONLY 0
#endif

#include "header.h"

/* Pointer to the fortune text */
static char *base = 0;

/* Array of pointers to each fortune */
static char **flines = 0;

/* True if we have loaded the fortune info */
FILE *fd = 0;

/* # lines in fortune database */
static int nlines = 0;

char *fortune(char *file)
{
    char *p;
    int lines;
    int tmp;
    struct stat stat;

    if(fd == 0) {
	/* Open the file */
	fd = fopen(file, "r");

	if(fd < 0) {
	    /* Can't fine file */
	    return 0;
	}

	/* Find out how big fortune file is and get memory for it */
	stat.st_size = 16384;

	if(fstat(fileno(fd), &stat) < 0) {
	    /* Can't stat file */
	    fclose(fd);
	    fd = NULL;
	    free(base);

	    return 0;
	}
	else {
	    base = malloc(1 + stat.st_size);

	    if(base == 0) {
		/* Can't stat file */
		fclose(fd);
		fd = NULL;
		free(base);

		return 0;
	    }
	}

	/* Read the entire fortune file */
	if(fread(base, sizeof(char), stat.st_size, fd) != stat.st_size) {
	    /* Can't read file */
	    fclose(fd);
	    fd = NULL;
	    free(base);

	    return 0;
	}

	fclose(fd);

	/* Final NULL termination */
	base[stat.st_size] = 0;

	/* Count up all the lines (and NULL terminate) to know memory needs */
	lines = 0;
	for(p = base; p < (base + stat.st_size); ++p) {
	    if(*p == '\n') {
		*p = 0;
	    }

	    /* Count lines */
	    ++lines;
	}

	nlines = lines;

	/* Get memory for array of pointers to each fortune */
	flines = (char **)malloc(nlines * sizeof(char *));
	if(flines == 0) {
	    /* malloc() failure */
	    free(base);
	    fd = NULL;

	    return 0;
	}

	/* Now assign each pointer to a line */
	p = base;
	for(tmp = 0; tmp < nlines; ++tmp) {
	    flines[tmp] = p;

	    /* Advance to the next line */
	    while(*p) {
		++p;
	    }
	}
    }

    /* If we have a database to look at */
    if((fd != stderr) && (fd != stdout) && (fd != stdin)) {
	if(nlines <= 0) {
	    return flines[rand() % 1];
	}
	else {
	    return flines[rand() % nlines];
	}
    }
    else {
	return 0;
    }
}
