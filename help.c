/*
 * help.c
 * Larn is copyrighted 1986 by Noah Morgan.
 *
 * Help function to display the help info
 *
 * format of the .larn.help file
 *
 * 1st character of file: # of pages of help available (ascii digit)
 * page (23 lines) for the introductory message (not ocunted in above)
 * pages of help text (23 lines per page)
 */

extern char helpfile[];

void help()
{
    int i;
    int j;

#ifndef VT100
    /* Intermediate translation buffer when not a VT100 */
    char tmbuf[128];
#endif

    /* Open the help file and get # pages */
    j = openhelp();

    if(j < 0) {
	return;
    }

    /* Skip over intro message */
    for(i = 0; i < 23; ++i) {
	lgetl();
    }

    while(j > 0) {
	clear();
	
	for(i = 0; i < 23; ++i) {
#ifdef VT100
	    /* Print out each line that we read in */
	    lprcat(lgetl());
#else
	    /* Intercept \33's */
	    tmcapcnv(tmbuf, lgetl());
	    lprcat(tmbuf);
#endif
	}
	
	if(j > 1) {
	    lprcat("    ---- Press ");
	    standout("return");
	    lprcat(" to exit, ");
	    standout("space");
	    lprcat(" for more help ---- ");
	    i = 0;

	    while((i != ' ') && (i != '\n') && (i != '\33')) {
		i = getchar();
	    }

	    if((i == '\n') || (i == '\33')) {
		lrclose();
		setscroll();
		drawscreen();

		return;
	    }
	}

	--j;
    }

    lrclose();
    retcont();
    drawscreen();
}

/*
 * Function to display the welcome message and background
 */
void welcome()
{
    int i;

#ifndef VT100
    /* Intermediate translation buffer when not a VT100 */
    char tmbuf[128];
#endif

    /* Open the help file */
    if(openhelp() < 0) {
	return;
    }

    clear();

    for(i = 0; i < 23; ++i) {
#ifdef VT100
	/* Print out each line that we read in */
	lprcat(lgetl());
#else
	/* Intercept \33's */
	tmcapcnv(tmbuf, lgetl());
	lprcat(tmbuf);
#endif
    }
    
    /* Press return to continue */
    lrclose();
    retcont();
}

/*
 * Function to say press return to continue and reset scroll when done
 */
void retcont()
{
    cursor(1, 24);
    lprcat("Press ");
    standout("return");
    lprcat(" to continue: ");

    char c = getchar();
    while(c != '\n') {
	c = getchar();
    }

    setscroll();
}

/*
 * Routine to open the help file and return the first character - '0'
 */
int openhelp()
{
    if(lopen(helpfile) < 0) {
	lprintf("Can't open help file \"%s\" ", helpfile);
	lflush();
	sleep(4);
	drawscreen();
	setscroll();

	return -1;
    }

    resetscroll();

    return (lgetc() - '0');
}
