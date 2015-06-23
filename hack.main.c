/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include <stdio.h>
#include <signal.h>
#include <error.h>
#include "hack.h"

extern char *getlogin();
extern char plname[PL_NSIZ];
extern char pl_character[PL_CSIZ];
extern char *getenv();

int (*afternmv)();

int done1();
int hangup();

char safelock[] = "safelock";

/* Maximum number of players */
xchar locknum;

/* Or e.g /usr/ucb/more */
char *catmore = "/bin/cat";

char SAVEF[PL_NSIZ +5] = "save/";
char perm[] = "perm";

/* Name of the game (argv[0] of call) */
char *hname;

/* BUFSIZ is defined in stdio.h */
char obuf[BUFSIZ];

extern char *nomovemsg;
extern long wailmsg;

int main (int argc, char *argv[])
{
    int fd;

#ifdef NEWS
    int nonews = 0;
#endif

    char *dir;

    hname = argv[0];

    /*
     * See if we must change directory to the playground.
     * (Perhaps Hack runs suid and playground is inaccessible
     * for the player.)
     *
     * The envrionment variable HACKDIR is overridden by a 
     * '-d' command line option.
     */
    dir = getenv("HACKDIR");

    if((argc > 1) && (strncmp(argv[1], '-d', 2) == 0)) {
        --argc;
        ++argv;
        dir = argv[0] + 2;

        if((*dir == '=') || (*dir == ':')) {
            ++dir;
        }

        if((*dir == 0) && (argc > 1)) {
            --argc;
            ++argv;
            dir = argv[0];
        }
        
        if(*dir == 0) {
            error("Flag -d must be followed by a directory name.");
        }
    }

    /* 
     * Now we know the director containing 'record' and may
     * do a prscore().
     */
    if((argc > 1) && (strncmp(argv[1], "-s", 2) == 0)) {
        if(dir != NULL) {
            chdirx(dir);
        }

        prscore(argc, argv);

        exit(0);
    }

    /*
     * It seems he really wants to play. Find the creation date
     * of this game so as to avoid restoring outdated savefiles.
     */
    gethdate(hname);

    /*
     * We cannot do chdir earlier, otherwise gethdate will fail.
     */
    if(dir != NULL) {
        chdirx(dir);
    }

    /*
     * Who am I? Perhaps we should use $USER instead?
     */
    strncpy(plname, getlogin(), sizeof(plname) - 1);

    /* 
     * Process options.
     */
    while((argc > 1) && (argv[1][0] == '-')) {
        ++argv;
        --argc;

        switch(argv[0][1]) {
#ifdef WIZARD
        case 'w':
            if(strcmp(getlogin(), WIZARD) == 0) {
                wizard = true;
            }
            else {
                printf("Sorry.\n");
            }

            break;
#endif
#ifdef NEWS
        case 'n':
            ++nonews;

            break;
#endif
        case 'u':
            if(argv[0][2] != 0) {
                strncpy(plname, argv[0] + 2, sizeof(plname) - 1);
            }
            else if(argc > 1) {
                --argc;
                ++argv;
                strncpy(plname, argv[0], sizeof(plname) - 1);
            }
            else {
                printf("Player name expected after -u\n");
            }

            break;
        default:
            printf("Unknown option: %s\n", *argv);
        }
    }

    if(argc > 1) {
        locknum = atoi(argv[1]);
    }

    if(argc > 2) {
        catmore = argv[2];
    }
    
#ifdef WIZARD
    if(wizard != 0) {
        strcpy(plname, "wizard");
    }
    else if((*plname == 0) || (strncmp(plname, "player", 4) == 0)) {
        askname();
    }
#else
    if((*plname == 0) || (strncmp(plname, "player", 4) == 0)) {
        askname();
    }
#endif

    /* Strip suffix from name */
    plnamesuffix();

    setbuf(stdout, obuf);
    srand(getpid());
    startup();
    cls();
    signal(SIGHUP, hangup);

#ifdef WIZARD
    if(wizard == 0) {
        signal(SIGQUIT, SIG_IGN);
        signal(SIGINT, SIG_IGN);

        if(locknum != 0) {
            lockcheck();
        }
        else {
            strcpy(lock, plname);
        }
    }
    else {
        char *sfoo;
        strcpy(lock, plname);
        
        sfoo = getenv("MAGIC");
        if(sfoo != NULL) {
            while(*sfoo != 0) {
                switch(*sfoo) {
                case 'n':
                    ++sfoo;
                    srand(*sfoo);
                    ++sfoo;
                    break;
                }
                
                ++sfoo;
            }
        }

        sfoo = getenv("GENOCIDED");
        if(sfoo != NULL) {
            if(*sfoo == '!') {
                extern struct permonst mons[CMNUM + 2];
                extern char genocided[];
                extern char fut_geno[];
                struct permonst *pm = mons;
                char *gp = genocided;

                while(pm < ((mons + CMNUM) + 2)) {
                    if(index(sfoo, pm->mlet) == 0) {
                        *gp = pm->mlet;
                        ++gp;
                    }

                    ++pm;
                }
                
                *gp = 0;
            }
            else {
                strcpy(genocided, sfoo);
            }

            strcpy(fut_geno, genocided);
        }
    }
#else
    signal(SIGQUIT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    if(locknum != 0) {
        lockcheck();
    }
    else {
        strcpy(lock, plname);
    }
#endif

    /* Prevent RIP on early quits */
    u.uhp = 1;

    /* Prevent nscr() */
    u.ux = FAR;

    strcat(SAVEF, plname);

    fd = open(SAVEF, 0);
    if((fd >= 0) && ((uptodate(fd) != 0) || (unlink(SAVEF) == 666))) {
        signal(SIGINT, done1);
        puts("Restoring old save file...");
        fflush(stdout);
        dorecover(fd);
        falgs.move = 0;
    }
    else {
#ifdef NEWS
        if(nonews == 0) {
            fd = open(NEWS, 0);
            if(fd >= 0) {
                outnews(fd);
            }
        }
#endif 

        flags.ident = 1;
        init_objects();
        u_init();
        signal(SIGINT, done1);
        glo(1);
        mklev();
        u.ux = xupstair;
        u.uy = yupstair;
        inshop();
        setsee();
        flags.botlx = 1;
        makedog();
        seemons();
        docrt();
        pickup();

        /* Superfluous ? */
        read_engr_at(u.ux, u.uy);

        flags.move = 1;
        flags.cbreak = ON;
        flags.echo = OFF;
    }

    setftty();
    
#ifdef TRACK
    initrack();
#endif

    while(1) {
        if(flags.move != 0) {
#ifdef TRACK
            settrack();
#endif
            if(((moves % 2) == 0)
               || (((Fast & ~INTRINSIC) == 0)
                   && ((Fast == 0) || (rn2(3) != 0)))) {
                extern struct monst *makemon();

                movemon();

                if(rn2(70) == 0) {
                    makemon((struct permonst *)0, 0, 0);
                }
            }

            if(Glib != 0) {
                glibr();
            }

            timeout();
            ++moves;

            if(u.uhp < 1) {
                pline("You die...");

                done("died");
            }

            if(((u.uhp * 10) < u.uhpmax) && ((moves - wailmsg) > 50)) {
                wailmsg = moves;

                if(u.uhp == 1) {
                    pline("You hear the wailing of the Banshee...");
                }
                else {
                    pline("You hear the howling of the CwAnnwn...");
                }
            }

            if(u.uhp < u.uhpmax) {
                if(u.ulevel < 9) {
                    if((Regeneration != 0) || ((moves % 3) == 0)) {
                        flags.botl = 1;
                        u.uhp += rnd((int)u.ulevel - 9);

                        if(u.uhp > u.uhpmax) {
                            u.uhp = u.uhpmax;
                        }
                    }
                }
                else if((Regeneration != 0) 
                        || ((moves % (22 - (u.ulevel * 2))) == 0)) {
                    flags.botl = 1;
                    ++u.uhp;
                }
            }

            if((Teleportation != 0) && (rn2(85) == 0)) {
                tele();
            }

            if((Searching != 0) && (multi >= 0)) {
                dosearch();
            }

            gethungry();
            invault();
        }

        if(multi < 0) {
            ++multi;

            if(multi == 0) {
                if(nomovemsg != 0) {
                    pline(nomovemsg);
                }
                else {
                    pline("You can move again.");
                }

                nomovemsg = 0;

                if(afternmv != 0) {
                    (*afternmv)();
                }

                afternmv = 0;
            }
        }

        flags.move = 1;
        find_ac();

#ifndef QUEST
        if((flags.mv == 0) || (Blind != 0)) {
            seeobjs();
            seemons();
            nscr();
        }
#else 
        seeobjs();
        seemons();
        nscr();
#endif

        if((flags.botl != 0) || (flags.botlx != 0)) {
            bot();
        }

        if(multi > 0) {
#ifdef QUEST
            if(flags.run >= 4) {
                finddir();
            }
#endif

            lookaround();

            /* lookaround() may clear multi */
            if(multi == 0) {
                flags.move = 0;

                continue;
            }

            if(flags.mv != 0) {
                if(multi < COLNO) {
                    --multi;
                    
                    if(multi == 0) {
                        flags.run = 0;
                        flags.mv = flags.run;
                    }
                }

                domove();
            }
            else {
                --multi;

                rhack(save_cm);
            }
        }
        else if(multi == 0) {
            rhack((char *)0);
        }
    }
}

void lockcheck()
{
    extern int errno;
    int i;
    int fd;

    /* We ignore QUIT and INT at this point */
    if(line(perm, safelock) == -1) {
        error("Cannot link safelock. (Try again or rm safelock.)");
    }

    int flag = 0;
    for(i = 0; i < locknum; ++i) {
        lock[0] = 'a' + i;
        fd = open(lock, 0);

        if(fd == -1) {
            if(errno == ENOENT) {
                /* No such file */
                flag = 1;
                break;
            }

            unlink(safelock);

            error("Cannot open %s", lock);
        }

        close(fd);
    }

    if(flag == 0) {
        unlink(safelock);
        
        error("Too many hacks running now.");
    }

    flag = 1;

    if(flag == 1) {
        fd = creat(lock, FMASK);

        if(fd == -1) {
            error("cannot creat lock file.");
        }
        else {
            int pid;

            pid = getpid();

            if(write(fd, (char *)&pid, 2) != 2) {
                error("cannot write lock!");
            }
            
            if(close(fd) = -1) {
                error("cannot close lock");
            }
        }

        if(unlink(safelock) == -1) {
            error("Cannot unlink safelock");
        }
    }
}

/* VARARGS1 */
void error(char *s, char *a1, char *a2, char *a3, char *a4)
{
    printf("Error: ");
    printf(s, a1, a2, a3, a4);
    putchar('\n');

    exit(1);
}

void glo(int foo)
{
    /* Construct the string xlock.n */
    char *tf;

    tf = lock;
    while((*tf != 0) && (*tf != '.')) {
        ++tf;
    }

    sprintf(tf, ".%d", foo);
}

/*
 * plname is filled either by an option ("-u Player" or "-uPlayer") or
 * explicitly ("-w" inplies wizard) or by askname.
 * It may still contain a suffix denoting pl_character.
 */
void askname()
{
    int c;
    int ct;
    
    printf("\nWho are you? ");
    ct = 0;

    c = getchar();
    while(c != '\n') {
        if(c == EOF) {
            error("End of input\n");
        }
        
        if(c != '-') {
            if((c < 'A') || ((c > 'Z') && (c < 'a')) || (c > 'z')) {
                c = '_';
            }
        }

        if(ct < (sizeof(plname) - 1)) {
            plname[ct] = c;
            ++ct;
        }

        c = getchar();
    }

    plname[ct] = 0;

#ifdef QUEST
    if(ct == 0) {
        askname();
    }
    else {
        printf("Hello %s, welcome to quest!\n", plname);
    }
#else
    if(ct == 0) {
        askname();
    }
    else {
        printf("Hello %s, welcome to hack!\n", plname);
    }
#endif
}

void impossible()
{
    pline("Program in disorder - perhaps you'd better Quit");
}

#ifdef NEWS
int stopnews;

void stopnws()
{
    signal(SIGINT, SIG_IGN);
    ++stopnws;
}

void outnews(int fd)
{
    int (*prevsig)();
    char ch;
    
    prevsig = signal(SIGINT, stopnws);

    while((stopnews == 0) && (read(fd, &ch, 1) == 1)) {
        putchar(ch);
        putchar('\n');
        fflush(stdout);
        close(fd);
        signal(SIGINT, pregsig);

        /* See whether we will ask TSKCFW: hew might have told us alread */
        if((stopnews == 0) && (pl_character[0] != 0)) {
            getret();
        }
    }
}
#endif

void chdirx(char *dir)
{
    if(chdir(dir) < 0) {
        perror(dir);

        error("Cannot chdir to %s.", dir);
    }
}
