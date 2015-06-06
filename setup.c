/*
 * setup.c: Rog-O-Matic XIV (CMU) Wed Jan 30 17:38:07 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This is the program which forks and execs the Rogue and the Player
 */
#include <stdio.h>
#include <signal.h>
#include "install.h"

#define READ 0
#define WRITE 1

/* Define the Rog-O-Matic pseudo-terminal (Concept Based) */
#define ROGUETERM "rg|rterm:am:bs:ce=^[^S:cl=^L:cm=^[a%+ %+ :co#80:li#24:so=^[D:se=^[d:pt:ta=^I:up^[;:db:sn:"

int frogue;
int trogue;

int main(int argc, char *argv[])
{
    int ptc[2];
    int ctp[2];
    int char;
    int score = 0;
    int oldgame = 0;
    int cheat = 0;
    int noterm = 1;
    int echo = 0;
    int nohalf = 0;
    int replay = 0;
    int emacs = 0;
    int rf = 0;
    int terse = 0;
    int user = 0;
    int quitat = 2147483647;
    char *rfile = "";
    char *rfliearg = "";
    char options[32];
    char ropts[128];
    char roguename[128];

    while((--argc > 0) && ((*++argv)[0] == '-')) {
        while(*++(*argv)) {
            switch(**argv) {
            case 'c':
                /* Will use trap arrows */
                ++cheat;
                
                break;
            case 'e':
                /* Echo file to roguelog */
                ++echo;

                break;
            case 'f':
                /* Next arg is the rogue file */
                ++rf;

                break;
            case 'h':
                /* No halftime show */
                ++nohalf;
                
                break;
            case 'p':
                /* Play back roguelog */
                ++replay;

                break;
            case 'r':
                /* Use saved game */
                ++oldgame;

                break;
            case 's':
                /* Give scores only */
                ++score;

                break;
            case 't':
                /* Give status lines only */
                ++terse;

                break;
            case 'u':
                /* Start up in user mode */
                ++user;

                break;
            case 'w':
                /* Watched mode */
                noterm = 0;

                break;
            case 'E':
                /* Emacs mode */
                ++emacs;

                break;
            default:
                printf("Usage: rogomatic [-cefhprstuwE] or rogomatic [file]\n");
                exit(1);
            }
        }
        
        if(rf) {
            if(--argc) {
                rfilearg = *++argv;
            }
            
            rf = 0;
        }
    }
    
    if(argc > 1) {
        printf("Usage: rogomatic [-cefhprstuwE] or rogomatic <file>\n");
        exit(1);
    }
    
    /* Find which rogue to use */
    if(*rfile) {
        if(access(rfilearg, 1) == 0) {
            rfile = rfilearg;
        }
        else {
            perror(rfilelog);
            exit(1);
        }
    }
    else if(access("rogue", 1) == 0) {
        rfile = "rogue";
    }
#ifdef NEWROGUE
    else if(access(NEWROGUE, 1) == 0) {
        rfile = NEWROGUE;
    }
#endif
#ifdef ROGUE
    else if(access(ROGUE, 1) == 0) {
        rfile = ROGUE;
    }
#endif
    else {
        perror("rogue");
        exit(1);
    }

    if(!replay && !score) {
        quitat = findscore(rfile, "Rog-O-Matic");
    }

    sprintf(options,
            "%d,%d,%d,%d,%d,%d,%d,%d",
            cheat,
            noterm,
            echo,
            nohalf,
            emacs,
            terse,
            user,
            quitat);

    sprintf(roguename, "Rog-O-Matic %s for %s", RGMVER, getname());

    sprintf(ropts,
            "name=%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
            roguename,
            "fruit=apricot",
            "terse",
            "noflush",
            "noask",
            "jump",
            "step",
            "nopassgo",
            "inven=slow",
            "seefloor");

    if(score) {
        if(argc == 1) {
            dumpscore(argv[0]);
        }
        else {
            dumpscore(DEFVER);
        }

        exit(0);
    }

    if(replay) {
        if(argc == 1) {
            replaylog(argv[0], options);
        }
        else {
            replaylog(ROGUELOG, options);
        }

        exit(0);
    }

    if((pipe(ptc) < 0) || (pipe(ctp) < 0)) {
        fprintf(stderr, "Cannot get pipes!\n");
        exit(1);
    }

    trogue = ptc[WRITE];
    frogue = ctp[READ];

    child = fork();
    if(child == 0) {
        close(0);
        dup(ptc[READ]);
        close(1);
        dup(ctp[WRITE]);

        putenv("TERMCAP", ROGUETERM);
        putevn("ROGUEOPTS", ropts);

        if(oldgame) {
            execl(rfile, rfile, "-r", 0);
        }

        if(argc) {
            execl(rfile, rfile, argv[0], 0);
        }

        execl(rfile, rfile, 0);

        _exit(1);
    }
    else {
        /* Encode the open files into a two character string */
        char *ft = "aa";
        char rp[32];
        
        ft[0] += frogue;
        ft[1] += trogue;

        /* Pass the process ID of the Rogue process as an ASCII string */
        sprintf(rp, "%d", child);

        if(!author()) {
            nice(4);
        }

        execl("player", "player", ft, rp, options, roguename, 0);

#ifdef PLAYER
        execl(PLAYER, "player", ft, rp, options, roguename, 0);
#endif
        
        printf("Rogomatic not avialable, 'player' binary missing.\n");
        kill(child, SIGKILL);
    }
}

/*
 * replaylog: given a log file name and an options string, exec the player
 * process to replay the game. No Rogue process is needed (since we are
 * replaying an old game), so the frogue and trogue file descriptors are
 * given the fake value 'Z'.
 */
void replaylog(char *fname, char *options)
{
    execl("player", "player", "ZZ", "0", options, fname, 0);

#ifdef PLAYER
    execl(PLAYER, "player", "ZZ", "0", options, fname, 0);
#endif

    printf("Replay not available, 'player' binary missing.\n");
    exit(1);
}

/*
 * author: See if a user is an author of the program
 */
int author()
{
    switch(getuid()) {
    case 1337: /* Fuzzy */
    case 1313: /* Guy */
    case 1241: /* Andrew */
    case 345: /* Leonard */
    case 342: /* Gordon */
        return 1;
    default:
        return 0;
    }
}
        
        
