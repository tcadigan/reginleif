/*
 * io.c: Rog-O-Matic XIV (CMU) Tue Mar 19 21:00:11 1985 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains all of the functions which deal with the real world.
 */
#include "io.h"

#include <ctype.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <time.h>

#include "arms.h"
#include "command.h"
#include "debug.h"
#include "globals.h"
#include "install.h"
#include "ltm.h"
#include "mess.h"
#include "monsters.h"
#include "pack.h"
#include "rooms.h"
#include "scorefile.h"
#include "search.h"
#include "stats.h"
#include "termtokens.h"
#include "things.h"
#include "types.h"
#include "utility.h"

#define READ 0

/*
 * charonscreen returns the current character on the screen (using
 * curses(3)). This macro is based on the winch(win) macro.
 */
#define charonscreen(X, Y) (mvinch(int y, int x))

char *month[] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

static char screen00 = ' ';

/* Constants */
#define SENDQ 256

/* The command queue */
/* To Rogue */
char queue[SENDQ];
int head = 0;
int tail = 0;

/*
 * getrogue: Sensory interface.
 *
 * Handle the grungy low level terminal I/O. getrogue reads tokens from the
 * Rogue process and interprets them, making the screen array an image of
 * the rogue level. getrogue returns when the string waitstr has been read
 * and either the cursor is on the Rogue '@' or some other condition
 * implies that we have synchronized with Rogue.
 */

/*
 * waitstr: String to synchronize with
 * onat: 0 ==> Wait for waitstr
 *       1 ==> Cursor on @ sufficient
 *       2 ==> [1] + send ';' whenever we eat a --More-- message
 */
void getrogue(char *waitstr, int onat)
{
    int botprinted = 0;
    int wasmapped = didreadmap;
    int r;
    int c;
    int i;
    int j;
    char ch;
    char *s;
    char *m;
    char *q;
    char *d;
    char *call;
    int *doors;
    static int moved = 0;

    /* no new doors found yet */
    newdoors = doorlist;

    /* Save our current position */
    atrow0 = atrow;
    atcol0 = atcol;

    /* FSM to check for the wait msg */
    s = waitstr;

    /* FSM to check for the '--More--' */
    m = "More--";
    
    /* FSM to check for 'Call it:' */
    call = "Call it:";

    /* FSM to check for prompt */
    q = "(* for list): ";

    /* FSM to check for tombstone grass */
    d = ")      ";
    
    if(moved) { /* If we moved last time, put an old monsters to sleep */
        sleepmonster();
        moved = 0;
    }

    /*
     * While we have not reached the end of the Rogue input, read
     * characters from Rogue and figure out what they mean.
     */
    while(*s 
          || ((!hasted || (version != RV36A)) 
              && onat
              && screen[row][col] != '@')) {
        ch = getroguetoken();

        /* If message ends in "(* for list): ", call terpmes */
        if(ch == *q) {
            if(*++q == 0) {
                terpmes();
            }
        }
        else {
            q = "(* for list): ";
        }

        /* 
         * Rogomatic now keys off the grass under the Tombstone to
         * detect that it has been killed. This was done because the
         * "Press return" prompt only happens if there is a score file
         * avialable on that system. Hopefully the grass is the same
         * in all versions of Rogue!
         */
        if(ch == *d) {
            if(*++d == 0) {
                addch(ch);
                deadrogue();

                return;
            }
        }
        else {
            d = ")_______";
        }

        /* If the message as a more, strip it off and call terpmes */
        if(ch == *m) {
            if(*++m == 0) { 
                /* More than 50 messages since last command ==> start logging */
                if((++morecount > 50) && !logging) {
                    toggleecho();
                    dwait(D_WARNING, "Started logging --More-- loop.");
                }

                /* More than 100 messages since last command ==> infinite loop */
                if(++morecount > 100) {
                    dwait(D_FATAL, "Cuaght in --More-- loop.");
                }

                /* Send a space (and possibly a semicolon to clear the message */
                if(onat == 2) {
                    sendnow(" ;");
                }
                else {
                    sendnow(" ");
                }

                /* Clear the --More-- of the end of the message */
                i = col - 7;

                while(i < col) {
                    screen[0][i] = ' ';
                    ++i;
                }

                /* Interpret the message */
                terpmes();

                /* 
                 * This code gets rid of the "Studded leather arm" bug.
                 * But it causes other problems.    MLM
                 */
                /* sprintf(&screen[0][col - 7], "--More--"); */
            }
        }
        else {
            m = "More--";
        }

        /* If the message is 'Call it:', cancel the request */
        if(ch == *call) {
            if(*++call == 0) {
                /* Send an escape (and possibly a semicolon) to clear the message */
                if(onat == 2) {
                    sendnow("%c;", ESC);
                }
                else {
                    sendnow("%c", ESC);
                }
            }
        }
        else {
            call = "Call it:";
        }

        /* Check to see whether we have read the synchronization string */
        if(*s) {
            if(*s == ch) {
                ++s;
            }
            else {
                s = waitstr;
            }
        }

        /* Now figure out what the token means */
        switch(ch) {
        case BS_TOK:
            --col;

            break;
        case CE_TOK:
            if(row && (row < 23)) {
                for(i = col; i < 80; ++i) {
                    updatepos(' ', row, i);
                    screen[row][i] = ' ';
                }
            }
            else {
                for(i = col; i < 80; ++i) {
                    screen[row][i] = ' ';
                }
            }

            if(row) {
                at(row, col);
                clrtoeol();
            }
            else if(col == 0) {
                screen00 = ' ';
            }

            break;
        case CL_TOK:
            clearscreen();

            break;
        case CM_TOK:
            screen00 = screen[0][0];

            break;
        case CR_TOK:
            /* Handle missing '--More--' between inventories MLM 24-Jun-83 */
            if((row == 0)
               && (screen[0][1] == ')')
               && (screen[0][col - 1] != '-')) {
                terpmes();
            }

            col = 0;
            
            break;
        case DO_TOK:
            ++row;

            break;
        case ER_TOK:

            break;
        case LF_TOK:
            ++row;
            col = 0;
            
            break;
        case ND_TOK:
            ++col;

            break;
        case SE_TOK:
            revvideo = 0;
            standend();

            break;
        case SO_TOK:
            revvideo = 1;
            standout();

            break;
        case TA_TOK:
            col = 8 * (1 + (col / 8));

            break;
        case EOF:
            if(!replaying || !logdigested) {
                playing = 0;

                return;
            }

            saynow("End of game log, type 'Q' to exit.");

            return;
            break;
        case UP_TOK:
            --row;

            break;
        default:
            if(ch < ' ') {
                saynow("Unknown character '\\%o'--more--", ch);
                waitforspace();
            }
            else if(row) {
                at(row, col);
                
                if(!emacs && !terse) {
                    addch(ch);
                }

                if(row == 23) {
                    botprinted = 1;
                }
                else {
                    updatepos(ch, row, col);
                }
            }
            else if(col == 0) {
                screen00 = screen[0][0];
            }
            else if((col == 1) && (ch == 'l') && (screen[0][0] == 'I')) {
                screen[0][0] = screen00;
                
                if(screen00 != ' ') {
                    terpmes();
                }

                screen[0][0] = 'I';
            }

            screen[row][col] = ch;
            ++col;

            break;
        }
    }

    if(botprinted) {
        terpbot();
    }

    if((atrow != atrow0) || (atcol != atcol0)) {
        /* Changed position, record the move */
        updateat();
        
        /* Indicate that we moved */
        moved = 1;

        /* Wake up adjacent mean monsters */
        wakemonster(8);

        /* Keep current rectangle up to date    LGCH */
        currentrectangle();
    }

    if(!usesynch && !pending()) {
        usesynch = 1;
        lastobj = NONE;
        resetinv();
    }

    if((version < RV53A) && checkrange && !pending()) {
        command(T_OTHER, "Iz");
        checkrange = 0;
    }

    /* If mapping status has changed */
    if(wasmapped != didreadmap) {
        dwait(D_CONTROL | D_SEARCH, 
              "wasmapped: %d   didreadmap: %d",
              wasmapped,
              didreadmap);

        mapinfer();
    }

    if(didreadmap != Level) {
        doors = doorlist;

        while(doors != newdoors) {
            r = *doors++;
            c = *doors++;
            dwait(D_INFORM, "new door at %d, %d", r, c);
            inferhall(r, c);
        }
    }

    if(!blinded) {
        /* For blanks around the rogue... */
        for(i = (atrow - 1); i <= (atrow + 1); ++i) {
            for(j = (atcol - 1); j <= (atcol + 1); ++j) {
                /* CANGO + BLANK impossible */
                if(seerc(' ', i, j) && onrc(CANGO, i, j)) {
                    /* Infer can't go and... */
                    unsetrc(CANGO | SAFE, i, j);

                    /* invalidate the map. */
                    setnewgoal();
                }
            }
        }
    }

    at(row, col);

    if(!emacs && !terse) {
        refresh();
    }
}

/*
 * terpbot: Read the Rogue status line and set the various status
 * variables. This routine depends on the value of version to decide what
 * the status line looks like.
 */
void terpbot()
{
    char sstr[30];
    char modeline[256];
    int oldlev = Level;
    int oldgold = Gold;
    int oldhp = Hp;
    int Str18 = 0;
    extern int geneid;
    int i;
    int oldstr = Str;
    int oldAc = Ac;
    int oldExp = Explev;

    /* Since we use scanf to read this field, it must not be left blank */
    if(screen[23][78] == ' ') {
        screen[23][78] = 'X';
    }

    /* Read the bottom line, there are three versions of the status line */
    if(version < RV52A) { /* Rogue 3.6, Rogue 4.7? */
        sscanf(screen[23],
               " Level: %d Gold: %d Hp: %d(%d) Str: %s Ac: %d Exp: %d/%d %s",
               &Level,
               &Gold,
               &Hp,
               &Hpmax,
               sstr,
               &Ac,
               &Explev,
               &Exp,
               Ms);

        sscanf(sstr, "%d/%d", &Str, &Str18);
        Str = (Str * 100) + Str18;

        if(Str > Strmax) {
            Strmax = Str;
        }
    }
    else if(version < RV53A) { /* Rogue 5.2 (versions A and B) */
        sscanf(screen[23],
               " Level %d Gold %d Hp: %d(%d) Str: %d(%d) Ac: %d Exp %d/%d %s",
               &Level,
               &Gold,
               &Hp,
               &Hpmax,
               &Str,
               &Strmax,
               &Ac,
               &Explev,
               &Exp,
               Ms);

        Str = Str + 100;
        Strmax = Strmax * 100;
    }
    else { /* Rogue 5.3 (and beyond???) */
        sscanf(screen[23],
               " Level: %d Gold %d Hp: %d(%d) Str: %d(%d) Ac: %d Exp %d/%d %s",
               &Level,
               &Gold,
               &Hp,
               &Hpmax,
               &Str,
               &Strmax,
               &Ac,
               &Explev,
               &Exp,
               Ms);

        Str = Str * 100;
        Strmax = Strmax * 100;
        Ac = 10 - Ac;
    }

    /* Monitor changes in some variables */
    if(screen[23][78] == 'X') {
        /* Restore blank */
        screen[23][78] = ' ';
    }
    
    if(oldlev != Level) {
        newlevel();
    }

    if(Level > MaxLevel) {
        MaxLevel = Level;
    }

    if(oldgold < Gold) {
        deletestuff(atrow, atcol);
    }

    if(oldhp < Hp) {
        newring = 1;
    }

    lastdamage = max(0, oldhp - Hp);

    /*
     * Insert code here to monitor changes in attributes due to special
     * attacks.    MLM October 26, 1983.
     */
    setbonuses();

    /* If in special output modes, generate output line */
    if((oldlev != Level)
       || (oldgold != Gold)
       || (oldstr != Str)
       || (oldAc != Ac)
       || (oldExp != Explev)) {
        /* Stuff the new values into the argument space (for ps command) */
        sprintf(modeline,
                "Rgm %d: Id%d L%d %d %d(%d) s%d a%d e%d    ",
                rogpid,
                geneid,
                Level,
                Gold,
                Hp,
                Hpmax,
                Str / 100,
                10 - Ac,
                Explev);

        modeline[arglen - 1] = '\0';
        strcpy(parmstr, modeline);

        /* Handle Emacs and Terse mode */
        if(emacs || terse) {
            /* Skip backward over blanks and nulls */
            i = 79;

            while((screen[23][i] == ' ') || (screen[23][i] == '\0')) {
                --i;
            }

            if(emacs) {
                sprintf(modeline, " %s (%%b)", screen[23]);
                
                if(strlen(modeline) > 72) {
                    sprintf(modeline, " %s", screen[23]);
                }

                fprintf(realstdout, "%s", modeline);
                fflush(realstdout);
            }
            else if(terse && (oldlev != Level)) {
                fprintf(realstdout, "%s\n", screen[23]);
                fflush(realstdout);
            }
        }
    }
}

/*
 * dumpwalls: Dump the current screen map
 */
void dumpwalls()
{
    int r;
    int c;
    int S;
    char ch;

    printexplored();

    for(r = 1; r < 23; ++r) {
        for(c = 0; c < 80; ++c) {
            S = scrmap[r][c];

            if(ARROW & S) {
                ch = 'a';
            }
            else if(TELTRAP & S) {
                ch = 't';
            }
            else if(TRAPDOR & S) {
                ch = 'v';
            }
            else if(GASTRAP & S) {
                ch = 'g';
            }
            else if(BEARTRP & S) {
                ch = 'b';
            }
            else if(DARTRAP & S) {
                ch = 's';
            }
            else if(WATERAP & S) {
                ch = 'w';
            }
            else if(TRAP & S) {
                ch = '^';
            }
            else if(STAIRS & S) {
                ch = '>';
            }
            else if(RUNOK & S) {
                ch = '%';
            }
            else if((DOOR + (BEEN & S)) == (DOOR + BEEN)) {
                ch = 'D';
            }
            else if(DOOR & S) {
                ch = 'd';
            }
            else if((BOUNDARY + (BEEN & S)) == (BOUNDARY + BEEN)) {
                ch = 'B';
            }
            else if((ROOM + (BEEN  & S)) == (ROOM + BEEN)) {
                ch = 'R';
            }
            else if(BEEN & S) {
                ch = ':';
            }
            else if(HALL & S) {
                ch = '#';
            }
            else if((BOUNDARY + (WALL & S)) == (BOUNDARY + WALL)) {
                ch = 'W';
            }
            else if(BOUNDARY & S) {
                ch = 'b';
            }
            else if(ROOM & S) {
                ch = 'r';
            }
            else if(CANGO & S) {
                ch = '.';
            }
            else if(WALL & S) {
                ch = 'W';
            }
            else if(S) {
                ch = 'X';
            }
            else {
                ch = '\0';
            }

            if(ch) {
                mvaddch(r, c, ch);
            }
        }
    }

    at(row, col);
}

/*
 * sendnow: Send a string to the Rogue process.
 */
void sendnow(char *f, ...)
{
    char cmd[128];
    char *s = cmd;

    va_list args;
    va_start(args, f);
    vsprintf(cmd, f, args);
    va_end(args);

    while(*s) {
        sendcnow(*s);
        ++s;
    }
}

/*
 * sendcnow: Send a character to the rogue process. This routine also does
 * the logging of characters in echo mode.
 */
void sendcnow(char c)
{
    if(replaying) {
        return;
    }

    if(logging) {
        if(cecho) {
            fprintf(fecho, "\nC: \"%c", c);
            cecho = !cecho;
        }
        else {
            fprintf(fecho, "%c", c);
        }
    }

    fprintf(trogue, "%c", c);
}

/*
 * send: Add a string to the queue of commands to be sent to Rogue. The
 * commands are sent one at a time by the resend routine.
 */
#define bump(p, sizeq) (p) = ((p) + 1) % sizeq

void send(char *f, ...)
{
    char cmd[128];
    char *s = cmd;

    va_list args;
    va_start(args, f);
    vsprintf(s, f, args);
    va_end(args);

    while(*s) {
        queue[tail] = *(s++);
        bump(tail, SENDQ);
    }

    /* Append null, so resend will treat as a unit */
    queue[tail] = '\0';
    bump(tail, SENDQ);
}

/*
 * resend: Send next block of characters from the queue
 */
int resend()
{
    /* Ptr into last command */
    char *l = lastcmd;

    /* Clear message count */
    morecount = 0;

    /* Fail if no commands */
    if(head == tail) {
        return 0;
    }

    /* Send all queued characters until the next queued NULL */
    while(queue[head]) {
        *l++ = queue[head];
        sendcnow(*l);
        bump(head, SENDQ);
    }

    bump(head, SENDQ);
    *l = '\0';

    /* Return success */
    return 1;
}

/*
 * pending: Return true if there is a command in the queue to be sent to
 * Rogue.
 */
int pending()
{
    if(head != tail) {
        return 1;
    }
    else {
        return 0;
    }
}

/*
 * getroguetoken: Get a command from Rogue (either a character or a
 * cursor motion sequence).
 */
char getroguetoken()
{
    char ch;

    if(replaying) {
        return getlogtoken();
    }

    ch = GETROGUECHAR;

    /* convert escape sequences into tokens (negative numbers) */
    if(ch == ESC) {
        ch = GETROGUECHAR;

        switch(ch) {
        case CE_CHR:
            ch = CE_TOK;
            
            break;
        case CL_CHR:
            ch = CL_TOK;

            break;
        case CM_CHR:
            ch = CM_TOK;

            break;
        case DO_CHR:
            ch = DO_TOK;

            break;
        case ND_CHR:
            ch = ND_TOK;

            break;
        case SE_CHR:
            ch = SE_TOK;

            break;
        case SO_CHR:
            ch = SO_TOK;

            break;
        case UP_CHR:
            ch = UP_TOK;

            break;
        default:
            saynow("Unknown sequence ESC-%s --More--", unctrl(ch));
            waitforspace();
            ch = ER_TOK;
        }
    }

    /* Get arguments for cursor addressing */
    if((int)ch == CM_TOK) {
        row = (int)GETROGUECHAR - 32;
        col = (int)GETROGUECHAR - 32;
    }

    /* Log the tokens */
    if(logging) {
        if(!cecho) {
            fprintf(fecho, "\"\nR: ");
            cecho = !cecho;
        }

        if(ISPRT(ch)) {
            fprintf(fecho, "%c", ch);
        }
        else {
            switch(ch) {
            case BS_TOK:
                fprintf(fecho, "{bs}");
                
                break;
            case CE_TOK:
                fprintf(fecho, "{ce}");
                
                break;
            case CL_TOK:
                fprintf(fecho, "{ff}");
                
                break;
            case CM_TOK:
                fprintf(fecho, "{cm(%d,%d)}", row, col);

                break;
            case CR_TOK:
                fprintf(fecho, "{cr}");

                break;
            case DO_TOK:
                fprintf(fecho, "{do}");

                break;
            case LF_TOK:
                fprintf(fecho, "{nl}");

                break;
            case ND_TOK:
                fprintf(fecho, "{nd}");

                break;
            case SE_TOK:
                fprintf(fecho, "{se}");
                
                break;
            case SO_TOK:
                fprintf(fecho, "{so}");

                break;
            case TA_TOK:
                fprintf(fecho, "{ta}");

                break;
            case UP_TOK:
                fprintf(fecho, "{up}");

                break;
            case ER_TOK:
                fprintf(fecho, "{ERRESC}");

                break;
            default:
                fprintf(fecho, "{ERR%o}", ch);
                ch = ER_TOK;
            }
        }

        fflush(fecho);
    }

    return ch;
}

/*
 * at: Move the cursor. Now just a call to move().
 */
void at(int r, int c)
{
    move(r, c);
}

/*
 * deadrogue: Called when we have been killed, it reads the tombstone
 * to see how much we had when we died and who killed us. It then
 * calls quitrogue to handle the termination handshaking and log the
 * game.
 */
#define GOLDROW 15
#define KILLROW 17
#define TOMBCOL 19

void deadrogue()
{
    int mh;
    char *killer;
    char *killend;

    printf("\n\nOops...");
    refresh();

    sscanf(&screen[GOLDROW][TOMBCOL], "%18d", &Gold);

    killer = &screen[KILLROW][TOMBCOL];
    killend = killer + 17;

    while(*killer == ' ') {
        ++killer;
    }

    while(*killend == ' ') {
        *(killend--) = '\0';
    }

    /* Record the death blow if killed by a monster */
    mh = findmonster(killer);

    if(mh != NONE) {
        addprob(&monhist[mh].theyhit, SUCCESS);
        addstat(&monhist[mh].damage, Hp);
    }

    quitrogue(killer, Gold, DIED);
}

/*
 * quitrogue: We are going to quit. Log the game and send a \n to
 * the Rogue process, then wait for it to die before returning.
 */

/*
 * reason: A reason string for the summary line 
 * gld: What is the final score
 * terminationtype: SAVED, FINISHED, or DIED */
void quitrogue(char *reason, int gld, int terminationtype)
{
    struct tm *ts;
    long clock;
    char *k;
    char *r;

    /* Save the killer and score */
    k = ourkiller;
    for(r = reason; *r && (*r != ' '); ++r) {
        *k = *r;
        ++k;
    }

    *k = '\0';
    ourscore = gld;

    /* Don't need to make up any more commands */
    if(!replaying || !logdigested) {
        playing = 0;
    }

    /* Not get the current time, so we can data the score */
    clock = time(&clock);
    ts = localtime(&clock);

    /* Now get the current time, so we can date the score */
    if(cheat) {
        sprintf(sumline,
                "%3s %2d, %4d %-8.8s %7d%s%-17.17s %3d %3d ",
                month[ts->tm_mon],
                ts->tm_mday,
                1900 + ts->tm_year,
                getname(),
                gld,
                "*",
                reason,
                MaxLevel,
                Hpmax);
    }
    else {
        sprintf(sumline,
                "%3s %2d, %4d %-8.8s %7d%s%-17.17s %3d %3d ",
                month[ts->tm_mon],
                ts->tm_mday,
                1900 + ts->tm_year,
                getname(),
                gld,
                "",
                reason,
                MaxLevel,
                Hpmax);
    }

    if(Str % 100) {
        sprintf(sumline, "%s%2d.%2d", sumline, Str / 100, Str % 100);
    }
    else {
        sprintf(sumline, "%s  %2d", sumline, Str / 100);
    }

    sprintf(sumline,
            "%s %2d %2d/%-6d  %d",
            sumline, 
            Ac,
            Explev, 
            Exp,
            ltm.gamecnt);

    /* Now write the sumamry line to toe log file */
    at(23, 0);
    clrtoeol();
    refresh();

    /* 22 is index of score in sumline */
    if(!replaying) {
        add_score(sumline, versionstr, (terse || emacs || noterm));
    }

    /* Restore interrupt status */
    reset_int();

    /* Set the termination message based on the termination method */
    if(stlmatch(reason, "total winner")) {
        termination = "victorius";
    }
    else if(stlmatch(reason, "user typing quit")) {
        termination = "abortivus";
    }
    else if(stlmatch(reason, "gave up")) {
        termination = "inops consilii";
    }
    else if(stlmatch(reason, "quit (scoreboard)")) {
        termination = "callidus";
    }
    else if(stlmatch(reason, "saved")) {
        termination = "suspendus";
    }

    /* Send the requisite handshaking to Rogue */
    if(terminationtype == DIED) {
        sendnow("\n");
    }
    else if(terminationtype == FINISHED) {
        sendnow("Qy\n");
    }
    else {
        /* Must send two yesses. R5.2 MLM */
        sendnow("Syy");
    }

    /* Wait for Rogue to die */
    wait((int *)NULL);
}

/*
 * waitfor: Snarf characters from Rogue until a string is found.
 *          The characters are echoed to the user's screen.
 *
 *          The string must not contain a valid prefix of itself
 *          internally.
 *
 * MLM 8/27/82
 */
void waitfor(char *mess)
{
    char *m = mess;
    
    while(*m) {
        if(getroguetoken() == *m) {
            ++m;
        }
        else {
            m = mess;
        }
    }
}

/*
 * say: Display a message on the top line. Restore cursor to Rogue.
 */
void say(char *f, va_list args)
{
    char buf[BUFSIZ];
    char *b;

    if(!emacs && !terse) {
        vsprintf(buf, f, args);
        at(0, 0);
        
        for(b = buf; *b; ++b) {
            printw("%s", unctrl(*b));
        }

        clrtoeol();
        at(row, col);
    }
}

/*
 * saynow: Display a message on the top line. Restore cursor to Rogue,
 *         and refresh the screen.
 */
void saynow(char *f, ...)
{
    if(!emacs && !terse) {
        va_list args;
        va_start(args, f);
        say(f, args);
        va_end(args);
        refresh();
    }   
}

/*
 * waitforspace: Wait for the user to type a space.
 * Be sure to interpret a snapshot command, if given.
 */
void waitforspace()
{
    char ch;

    refresh();

    if(!noterm) {
        ch = fgetc(stdin);

        while(ch != ' ') {
            if(ch == '/') {
                dosnapshot();
            }

            ch = fgetc(stdin);
        }
    }

    at(row, col);
}

/*
 * givehelp: Each time a ? is pressed, this routine prints the next
 * help message in a sequence of help messages. Nexthelp is an
 */
char *nexthelp[] = {
    "Rgm commands: t = toggle run mode, e = logging, i = inventory, - = status     [?]",
    "Rgm commands: <ret> = singlestep, ` = summary, / = snapshot, R = replay       [?]",
    "Rgm commands: m = long term memory display, G = display gene settings         [?]",
    "Rogue commands: S = Save, Q = Quit, h j k l H J K L b n u y N B U Y f s < >   [?]",
    "Wizard: d = debug, ! = show items, @ = show monsters, # = show level flags    [?]",
    "Wizard: ( = database, ) = cycles, + = possible secret doors, : = chicken      [?]",
    "Wizard: [ = weapstat, ] = rustproof armor, r = resetinv, & = object count     [?]",
    "Wizard: * = toggle blind, C = toggle cosmic, M = mazedoor, A = attempt, { = flags",
    NULL
};

char **helpline = nexthelp;

void givehelp()
{
    if(*helpline == NULL) {
        helpline = nexthelp;
    }

    saynow(*helpline++);
}

/*
 * pauserogue: Wait for the user to type a space and then redraw the
 *             screen. Now uses the stored image and passes it to
 *             curses rather than sending a form feed to Rogue.  MLM
 */
void pauserogue()
{
    at(23, 0);
    addstr("--press space to continue--");
    clrtoeol();
    refresh();

    waitforspace();

    redrawscreen();
}

/*
 * getrogver: Read the output of the Rogue version command
 *            and set version. RV36B = 362 (3.6 with wands)
 *            and RV52A = 521 (5.2). Note that RV36A is
 *            inferred when we send a "//" command to identify
 *            wands.
 *
 * Get version from first 2000 chars of a log file  Feb 9, 1985 - mlm
 */

#define VERMSG "ersion "

void getrogver()
{
    char *vstr = versionstr;
    char *m = VERMSG;
    int cnt = 2000;
    int ch;

    if(replaying) { /* Look for the version string in log */
        while((cnt-- > 0) && *m) {
            if(fgetc(logfile) == *m) {
                ++m;
            }
            else {
                m = VERMSG;
            }
        }

        if(*m == '\0') { /* Found VERMSG, get version string */
            ch = fgetc(logfile);

            while(ch != ' ') {
                *(vstr++) = ch;

                ch = fgetc(logfile);
            }

            *--vstr = '\0';
        }
        else { /* Use default version */
            sprintf(versionstr, DEFVER);
        }

        /* Put log file back to start */
        rewind(logfile);
    }
    else { /* Execute the version command */
        sendnow("v");
        waitfor("ersion ");

        ch = getroguetoken();
        
        while(ch != ' ') {
            *(vstr++) = ch;
            
            ch = getroguetoken();
        }

        *--vstr = '\0';
    }

    if(stlmatch(versionstr, "3.6")) {
        version = RV36B;
    }
    else if(stlmatch(versionstr, "5.2")) {
        version = RV52A;
    }
    else if(stlmatch(versionstr, "5.3")) {
        version = RV53A;
    }
    else {
        saynow("What a strange version of Rogue! ");
    }
}

/*
 * charsavail: How many characters are there at the terminal? If any
 * characters are found, 'noterm' is reset, since there is obviously
 * a terminal around if the user is typing at us.
 */
int charsavail()
{
    long n;
    int retc;

    retc = ioctl(READ, FIONREAD, &n);

    if(retc) {
        saynow("Ioctl returns %d, n = %ld\n", retc, n);

        n = 0;
    }

    if(n > 0) {
        noterm = 0;
    }

    return (int)n;
}

/*
 * redrawscreen: Make the user's screen lock like the Rogue screen (screen).
 */
void redrawscreen()
{
    int i;
    int j;
    char ch;

    clear();

    for(i = 1; i < 24; ++i) {
        for(j = 0; j < 80; ++j) {
            ch = screen[i][j];
            
            if(ch > ' ') {
                mvaddch(i, j, ch);
            }
        }
    }

    at(row, col);

    refresh();
}

/*
 * toggleecho: Toggle the I/O echo feature. If first time, open the
 * roguelog file.
 */
void toggleecho()
{
    if(replaying) {
        return;
    }

    logging = !logging;

    if(logging) {
        fecho = wopen(ROGUELOG, "w");

        if(fecho == NULL) {
            logging = !logging;
            saynow("can't open %s", ROGUELOG);
        }
        else {
            fprintf(fecho, "Rogomatic Game Log\n\n");
            saynow("Logging to file %s", ROGUELOG);
            cecho = 1;

            if(*versionstr) {
                command(T_OTHER, "v");
            }
        }
    }
    else {
        if(cecho) {
            fprintf(fecho, "\n");
        }
        else {
            fprintf(fecho, "\"\n");
        }

        fclose(fecho);

        if(playing) {
            saynow("File %s closed", ROGUELOG);
        }
    }

    if(playing) {
        at(row, col);
        refresh();
    }
}

/*
 * clearsendqueue: Throw away queued Rogue commands.
 */
void clearsendqueue()
{
    head = tail;
}

/*
 * startreplay: Open the log file to replay.
 */
void startreplay(FILE **logf, char *logfname)
{
    *logf = fopen(logfname, "r");
    
    if(*logf == NULL) {
        fprintf(stderr, "Can't open \'%s\'.\n", logfname);
        
        exit(1);
    }
}

/*
 * putn: Put 'n' copies of character 'c' on file 'f'.
 */
void putn(char c, FILE *f, int n)
{
    while(n) {
        --n;
        putc(c, f);
    }
}

/*
 * printsnap: Printf a snapshot to file f.
 */
void printsnap(FILE *f)
{
    int i;
    int j;
    int length;
    struct tm *ts;
    long clock;

    /* Now get the current time, so we can date the snapshot */
    clock = time(&clock);
    ts = localtime(&clock);

    /* Print snapshot timestamp */
    fprintf(f,
            "\nSnapshot taken on %s %d, %d at %02d:%02d:%02d:\n\n",
            month[ts->tm_mon],
            ts->tm_mday,
            1900 + ts->tm_year,
            ts->tm_hour,
            ts->tm_min,
            ts->tm_sec);

    /* Print the current map */
    putn('-', f, 79);
    fprintf(f, "\n");
    
    for(i = 0; i < 24; ++i) {
        for(length = 79; length >= 0; --length) {
            if((mvinch(i, length) & A_CHARTEXT) == ' ') {
                break;
            }

            for(j = 0; j <= length; ++j) {
                fprintf(f, "%c", (char)(mvinch(i, j) & A_CHARTEXT));
            }
        }

        fprintf(f, "\n");
    }

    putn('-', f, 79);

    /* Print status variables */
    fprintf(f, "\n\n%s\n\n", statusline());

    /* Print the inventory */
    dumpinv(f);
    fprintf(f, "\n");
    putn('-', f, 79);
    fprintf(f, "\n");
}

/*
 * getlogtoken: Routine to retrieve a rogue token form the log file.
 * This allows us to replay a game with all the diagnostic commands of
 * Rog-O-Matic at our disposal.    LGCH
 */
char getlogtoken()
{
    int acceptline;
    char ch = GETLOGCHAR;
    char ch1;
    char ch2;
    char dig;

    while(ch == NEWLINE) {
        acceptline = 0;
        ch = GETLOGCHAR;

        if(ch == 'R') {
            ch = GETLOGCHAR;

            if(ch == ':') {
                ch = GETLOGCHAR;

                if(ch == ' ') {
                    ch = GETLOGCHAR;
                    acceptline = 1;
                }
            }
        }

        if(!acceptline) {
            while(((int)ch != NEWLINE) && ((int)ch != EOF)) {
                ch = GETLOGCHAR;
            }
        }
    }

    if(ch == '{') {
        ch1 = GETLOGCHAR;
        ch2 = GETLOGCHAR;
        
        /* Ignore the close '}' */
        ch = GETLOGCHAR;

        switch(ch1) {
        case 'b':
            ch = BS_TOK;

            break;
        case 'c':
            switch(ch2) {
            case 'e':
                ch = CE_TOK;

                break;
            case 'm':
                ch = CM_TOK;
                row = 0;

                dig = GETLOGCHAR;

                while(dig != ')') {
                    col = (col * 10) + dig - '0';

                    dig = GETLOGCHAR;
                }

                /* Ignore '}' */
                GETLOGCHAR;

                break;
            case 'r':
                ch = CR_TOK;
            }

            break;
        case 'd':
            ch = DO_TOK;
            
            break;
        case 'f':
            ch = CL_TOK;
            
            break;
        case 'n':
            if(ch2 == 'l') {
                ch = LF_TOK;
            }
            else {
                ch = ND_TOK;
            }

            break;
        case 's':
            if(ch2 == 'e') {
                ch = SE_TOK;
            }
            else {
                ch = SO_TOK;
            }

            break;
        case 't':
            ch = TA_TOK;

            break;
        case 'u':
            ch = UP_TOK;

            break;
        case 'E':
            while(GETLOGCHAR != '}') {
            }

            ch = ER_TOK;

            break;
        }
    }

    return ch;
}

/*
 * getoldcommand: Retrieve the old command from a logfile we are replaying.
 */
void getoldcommand(char *s)
{
    int charcount = 0;
    char ch = ' ';
    char term = '"';
    char *startpat = "\nC: ";

    while(*startpat && ((int)ch != EOF)) {
        ch = GETLOGCHAR;

        if(ch != *(startpat++)) {
            startpat = "\nC: ";
        }
    }

    if((int) ch != EOF) {
        ch = GETLOGCHAR;
        term = ch;

        ch = GETLOGCHAR;

        while((ch != term) && ((int)ch != EOF) && (charcount++ < 128)) {
            *(s++) = ch;

            ch = GETLOGCHAR;
        }
    }

    *s = '\0';
}

/*
 * dosnapshot: Add a snapshot to the SNAPSHOT file.
 */
void dosnapshot()
{
    snapshot = wopen(SNAPSHOT, "a");

    if(snapshot == NULL) {
        saynow("Cannot write file %s.", SNAPSHOT);
    }
    else {
        printsnap(snapshot);
        saynow("Snapshot added to %s.", SNAPSHOT);
    }
}

/*
 * clearscreen: done whenever a {ff} is sent by Rogue. This code is
 * separate so ti can be called from replay(), since there is an imiplicit
 * formfeed not recorded in the log file.    MLM
 */
void clearscreen()
{
    int i;
    int j;

    col = 0;
    row = col;
    screen00 = ' ';

    for(i = 0; i < 24; ++i) {
        for(j = 0; j < 80; ++j) {
            screen[i][j] = ' ';
            unsetrc(STUFF, i, j);
        }
    }

    initstufflist();
    mlistlen = 0;

    /* temp hack MLM */
    /* initmonsterlist(); */
}

/*
 * statusline: Write all about our current status into a string.
 * Returns a pointer to a static area.    MLM
 */
char *statusline()
{
    static char staticarea[256];
    char *s = staticarea;

    sprintf(s, "Status: ");

    if(aggravated) {
        strcat(s, "aggravated, ");
    }
    
    if(beingheld) {
        strcat(s, "being held, ");
    }
    
    if(blinded) {
        strcat(s, "blind, ");
    }
    
    if(confused) {
        strcat(s, "confused, ");
    }

    if(cosmic) {
        strcat(s, "cosmic, ");
    }
    
    if(cursedarmor) {
        strcat(s, "cursed armor, ");
    }

    if(cursedweapon) {
        strcat(s, "cursed weapon, ");
    }

    if(doublehasted) {
        strcat(s, "perm hasted, ");
    }

    if(droppedscare) {
        strcat(s, "dropped scare, ");
    }

    if(floating) {
        strcat(s, "floating, ");
    }

    if(hasted) {
        strcat(s, "hasted, ");
    }

    if(protected) {
        strcat(s, "protected, ");
    }
    
    if(redhands) {
        strcat(s, "red hands, ");
    }
    
    if(Level == didreadmap) {
        strcat(s, "mapped, ");
    }

    if(*genocided) {
        sprintf(s, "%sgenocided '%s', ", s, genocided);
    }

    sprintf(s,
            "%s%d food%s, %d missile%s, %d turn%s, (%d,%d %d,%d) bonus",
            s,
            larder,
            plural(larder),
            ammo,
            plural(ammo),
            turns,
            plural(turns),
            gplushit,
            gplusdam,
            wplushit,
            wplusdam);

    return s;
}
