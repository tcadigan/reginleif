/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#include "hack.end.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "alloc.h"
#include "hack.h"
#include "hack.bones.h"
#include "hack.do_name.h"
#include "hack.invent.h"
#include "hack.main.h"
#include "hack.objnam.h"
#include "hack.pri.h"
#include "hack.rip.h"
#include "hack.shk.h"
#include "hack.termcap.h"
#include "hack.topl.h"
#include "hack.tty.h"

extern char plname[];
extern char pl_character[];

xchar maxdlevel = 1;

void done1(int sig)
{
    signal(SIGINT, SIG_IGN);
    pline("Really quit?");
    
    if(readchar() != 'y') {
        signal(SIGINT, done1);
        clrlin();
        fflush(stdout);

        if(multi > 0) {
            nomul(0);
        }

        return;
    }

    done("quit");
}

int done2()
{
    signal(SIGINT, SIG_IGN);
    pline("Really quit?");
    
    if(readchar() != 'y') {
        signal(SIGINT, done1);
        clrlin();
        fflush(stdout);

        if(multi > 0) {
            nomul(0);
        }

        return 0;
    }

    done("quit");

    /* NOTREACHED */
    return 0;
}

int done_stopprint;

void done_itr()
{
    ++done_stopprint;
    signal(SIGINT, SIG_IGN);
}

void done_in_by(struct monst *mtmp)
{
    static char buf[BUFSZ];
    pline("You die ...");

    if(mtmp->data->mlet == ' ') {
        sprintf(buf, "the ghost of %s", (char *)mtmp->mextra);
        killer = buf;
    }
    else if(mtmp->mnamelth != 0) {
        sprintf(buf, "%s called %s", mtmp->data->mname, NAME(mtmp));
        killer = buf;
    }
    else if(mtmp->minvis != 0) {
        sprintf(buf, "invisible %s", mtmp->data->mname);
        killer = buf;
    }
    else {
        killer = mtmp->data->mname;
    }

    done("died");
}

/* Called with arg "died", "escaped", "quit", "choked", "panic" or "starved" */
/* Be careful not to call panic from here! */
void done(char *st1)
{
#ifdef WIZARD
    if((wizard != 0) && (*st1 == 'd')) {
        u.ustrmax += 2;
        u.ustr = u.ustrmax;
        u.uhpmax += 10;
        u.uhp = u.uhpmax;

        if(uarm != 0) {
            ++uarm->spe;
        }

        if(uwep != 0) {
            /* NB: uwep need not be a weapon! */
            ++uwep->spe;
        }

        u.uswldtim = 0;
        pline("For some reason you are still alive.");
        flags.move = 0;

        if(multi > 0) {
            multi = 0;
        }
        else {
            multi = 1;
        }

        flags.botl = 1;

        return;
    }
#endif

    signal(SIGINT, done_itr);

    if((*st1 == 'q') && (u.uhp < 1)) {
        st1 = "died";
        killer = "quit while already on Charon's boat";
    }

    if(*st1 == 's') {
        killer = "starvation";
    }

    paybill();
    clearlocks();

    if(index("cds", *st1) != 0) {
        savebones();
        outrip();
    }

    /* Does a cls() */
    settty((char *)0);

    if(done_stopprint == 0) {
        printf("Goodbye %s %s...\n\n", pl_character, plname);
    }

    long int tmp;
    tmp = u.ugold - u.ugold0;
    
    if(tmp < 0) {
        tmp = 0;
    }

    if(*st1 == 'd') {
        tmp -= (tmp / 10);
    }
    else {
        killer = st1;
    }

    u.urexp += tmp;

    if(*st1 == 'e') {
        extern struct monst *mydogs;
        struct monst *mtmp = mydogs;
        struct obj *otmp;
        int i;
        unsigned int worthlessct = 0;

        u.urexp += (50 * maxdlevel);

        if(mtmp != 0) {
            if(done_stopprint == 0) {
                printf("You");
            }

            while(mtmp != 0) {
                if(done_stopprint != 0) {
                    printf(" and %s", monnam(mtmp));
                }
                
                u.urexp += mtmp->mhp;
                mtmp = mtmp->nmon;
            }

            if(done_stopprint == 0) {
                printf("\nescaped from the dungeon with %lu points,\n",
                       u.urexp);
            }
        }
        else {
            if(done_stopprint == 0) {
                printf("You escaped from the dungeon with %lu points,\n",
                       u.urexp);
            }
        }

        for(otmp = invent; otmp != NULL; otmp = otmp->nobj) {
            if(otmp->olet == GEM_SYM) {
                objects[otmp->otyp].oc_name_known = 1;
                i = otmp->quan * objects[otmp->otyp].g_val;

                if(i == 0) {
                    worthlessct += otmp->quan;
                    
                    continue;
                }

                u.urexp += i;

                if(done_stopprint == 0) {
                    printf("\t%s (worth %d Zorkmids),\n", doname(otmp), i);
                }
            }
            else if(otmp->olet == AMULET_SYM) {
                otmp->known = 1;
                
                if(otmp->spe < 0) {
                    i = 2;
                }
                else {
                    i = 5000;
                }
                
                u.urexp += i;
                
                if(done_stopprint == 0) {
                    printf("\t%s (worth %d Zorkmids),\n", doname(otmp), i);
                }
                
                if(otmp->spe >= 0) {
                    u.urexp *= 2;
                }
            }
        }

        if(worthlessct != 0) {
            if(done_stopprint == 0) {
                printf("\t%d worthless piece%s of coloured class,\n",
                       worthlessct, plur(worthlessct));
            }
        }

        killer = st1;
    }
    else {
        if(done_stopprint == 0) {
            printf("You %s on dungeon level %d with %lu points,\n",
                   st1,
                   dlevel,
                   u.urexp);
        }
    }

    if(done_stopprint == 0) {
        if(u.ugold == 1) {
            if(moves == 1) {
                printf("and %lu piece%s of gold, after %lu move%s.\n",
                       u.ugold,
                       "",
                       moves,
                       "");
            }
            else {
                printf("and %lu piece%s of gold, after %lu move%s.\n",
                       u.ugold,
                       "",
                       moves,
                       "s");
            }
        }
        else {
            if(moves == 1) {
                printf("and %lu piece%s of gold, after %lu move%s.\n",
                       u.ugold,
                       "s",
                       moves,
                       "");
            }
            else {
                printf("and %lu piece%s of gold, after %lu move%s.\n",
                       u.ugold,
                       "s",
                       moves,
                       "s");
            }
        }
    }

    if(done_stopprint == 0) {
        printf("You were level %d with a maximum %d hit points when you %s.\n",
               u.ulevel,
               u.uhpmax,
               st1);
    }

    if(*st1 == 'e') {
        /* All those pieces of coloured glass ... */
        getret();
        cls();
    }

#ifdef WIZARD
    if(wizard == 0) {
        topten();
    }
#else
    topten();
#endif

    if(done_stopprint != 0) {
        printf("\n\n");
    }

    exit(0);
}

#define newttentry() (struct toptenentry *)alloc(sizeof(struct toptenentry))
#define PERSMAX 1

/* Must be > 0 */
#define POINTSMIN 1

/* Must be >= 10 */
#define ENTRYMAX 100

struct toptenentry *tt_head;

void topten()
{
    int rank;
    int rank0 = -1;
    int rank1 = 0;
    int occ_cnt = PERSMAX;
    struct toptenentry *t0;
    struct toptenentry *t1;
    struct toptenentry *tprev;
    char *recfile = "record";
    FILE *rfile;
    int flg = 0;

    rfile = fopen(recfile, "r");
    
    if(rfile == 0) {
        puts("Cannot open record file!");

        return;
    }

    putchar('\n');

    /* Create a new 'topten' entry */
    t0 = newttentry();
    t0->level = dlevel;
    t0->maxlvl = maxdlevel;
    t0->hp = u.uhp;
    t0->maxhp = u.uhpmax;
    t0->points = u.urexp;
    t0->plchar = pl_character[0];

    strncpy(t0->str, plname, NAMSZ);
    (t0->str)[NAMSZ] = 0;

    strncpy(t0->death, killer, DTHSZ);
    (t0->death)[DTHSZ] = 0;

    /* Assure minimum number of points */
    if(t0->points < POINTSMIN) {
        t0->points = 0;
    }

    tt_head = newttentry();
    t1 = tt_head;
    tprev = 0;

    /* 
     * rank0:
     * -1 undefined
     * 0 not_on_list
     * n n_th on list
     */
    rank = 1;
    
    while(1) {
        if((fscanf(rfile,
                   "%d %d %d %d %ld %c %[^,],%[^\n]",
                   &t1->level,
                   &t1->maxlvl,
                   &t1->hp,
                   &t1->maxhp,
                   &t1->points,
                   &t1->plchar,
                   t1->str,
                   t1->death) != 8)
           || (t1->points < POINTSMIN)) {
            t1->points = 0;
        }

        if((rank0 < 0) && (t1->points < t0->points)) {
            ++rank;
            rank0 = rank;
     
            if(tprev == 0) {
                tt_head = t0;
            }
            else {
                tprev->tt_next = t0;
            }
            
            t0->tt_next = t1;
            --occ_cnt;
            
            /* Ask for a rewrite */
            ++flg;
        }
        else {
            tprev = t1;
        }

        if(t1->points == 0) {
            break;
        }

        if((strncmp(t1->str, t0->str, NAMSZ) == 0)
           && (t1->plchar == t0->plchar)) {
            --occ_cnt;
            
            if(occ_cnt <= 0) {
                if(rank0 < 0) {
                    rank0 = 0;
                    rank1 = rank;
                    printf("You didn't beat your previous score of %ld points.\n\n",
                           t1->points);
                }

                if(occ_cnt < 0) {
                    ++flg;
                
                    continue;
                }
            }
        }

        if(rank <= ENTRYMAX) {
            t1->tt_next = newttentry();
            t1 = t1->tt_next;
            ++rank;
        }

        if(rank > ENTRYMAX) {
            t1->points = 0;

            break;
        }
    }

    /* Rewrite record file */
    if(flg != 0) {
        fclose(rfile);
        rfile = fopen(recfile, "w");

        if(rfile == 0) {
            puts("Cannot write record file\n");

            return;
        }

        if(done_stopprint == 0) {
            if(rank0 > 0) {
                if(rank0 <= 10) {
                    puts("You made the top ten list!\n");
                }
                else {
                    printf("You reached the %d%s place on the top %d list.\n\n",
                           rank0,
                           ordin(rank0),
                           ENTRYMAX);
                }
            }
        }
    }

    if(rank0 == 0) {
        rank0 = rank1;
    }
    
    if(rank0 <= 0) {
        rank0 = rank;
    }
    
    if(done_stopprint == 0) {
        outheader();
    }
    
    t1 = tt_head;
    
    for(rank = 1; t1->points != 0; ++rank) {
        if(flg != 0) {
            fprintf(rfile,
                    "%d %d %d %d %ld %c %s,%s\n",
                    t1->level,
                    t1->maxlvl,
                    t1->hp,
                    t1->maxhp,
                    t1->points,
                    t1->plchar,
                    t1->str,
                    t1->death);
        }
        
        if(done_stopprint != 0) {
            t1 = t1->tt_next;
            
            continue;
        }
        
        if((rank > 5) && ((rank < (rank0 - 4)) || (rank > (rank0 + 4)))) {
            t1 = t1->tt_next;
            
            continue;
        }
        
        if((rank == (rank0 - 4)) && (rank0 > 10)) {
            putchar('\n');
        }
        
        if(rank != rank0) {
            outentry(rank, t1, 0);
        }
        else if(rank1 == 0) {
            outentry(rank, t1, 1);
        }
        else {
            int t0lth = outentry(0, t0, -1);
            int t1lth = outentry(rank, t1, t0lth);
            
            if(t1lth > t0lth) {
                t0lth = t1lth;
            }
            
            outentry(0, t0, t0lth);
        }
        
        t1 = t1->tt_next;
    }
    
    if(rank0 >= rank) {
        outentry(0, t0, 1);
    }

    fclose(rfile);
}

void outheader()
{
    char linebuf[BUFSZ];
    char *bp;

    strcpy(linebuf, "Number Points  Name");
    bp = eos(linebuf);

    while(bp < ((linebuf + COLNO) - 9)) {
        *bp = ' ';
        ++bp;
    }

    strcpy(bp, "Hp [max]");

    puts(linebuf);
}

/*
 * so > 0: Standout line
 * so = 0: Ordinary line
 * so < 0: No output, return length
 */
int outentry(int rank, struct toptenentry *t1, int so)
{
    boolean quit = FALSE;
    boolean killed = FALSE;
    boolean starv = FALSE;
    char linebuf[BUFSZ];

    linebuf[0] = 0;

    if(rank != 0) {
        sprintf(eos(linebuf), "%3d", rank);
    }
    else {
        sprintf(eos(linebuf), "   ");
    }

    sprintf(eos(linebuf), " %6ld %8s", t1->points, t1->str);

    if(t1->plchar == 'X') {
        sprintf(eos(linebuf), " ");
    }
    else {
        sprintf(eos(linebuf), "-%c ", t1->plchar);
    }

    if(strcmp("escaped", t1->death) == 0) {
        sprintf(eos(linebuf), "escaped the dungeon [max level %d]", t1->maxlvl);
    }
    else {
        if(strncmp(t1->death, "quit", 4) == 0) {
            sprintf(eos(linebuf), "quit");
            quit = TRUE;
        }
        else if(strcmp(t1->death, "choked") == 0) {
            sprintf(eos(linebuf), "choked in his food");
        }
        else if(strncmp(t1->death, "starv", 5) == 0) {
            sprintf(eos(linebuf), "started to death");
            starv = TRUE;
        }
        else {
            sprintf(eos(linebuf), "was killed");
            killed = TRUE;
        }

        if((killed != 0) || (starv != 0)) {
            sprintf(eos(linebuf), " on%s level %d", "", t1->level);
        }
        else {
            sprintf(eos(linebuf), " on%s level %d", " dungeon", t1->level);
        }

        if(t1->maxlvl != t1->level) {
            sprintf(eos(linebuf), " [max %d]", t1->maxlvl);
        }

        if((quit != 0) && (t1->death[4] != 0)) {
            sprintf(eos(linebuf), t1->death + 4);
        }
    }

    if(killed != 0) {
        if(strncmp(t1->death, "the ", 4) == 0) {
            sprintf(eos(linebuf), " by %s%s", "", t1->death);
        }
        else {
            if(index(vowels, *t1->death) != 0) {
                sprintf(eos(linebuf), " by %s%s", "an ", t1->death);
            }
            else {
                sprintf(eos(linebuf), " by %s%s", "a ", t1->death);
            }
        }
    }

    sprintf(eos(linebuf), ".");

    if(t1->maxhp != 0) {
        char *bp = eos(linebuf);
        char hpbuf[10];
        int hppos;

        if(t1->hp > 0) {
            sprintf(hpbuf, itoa(t1->hp));
        }
        else {
            sprintf(hpbuf, "-");
        }

        hppos = (COLNO - 7) - strlen(hpbuf);

        if(bp <= (linebuf + hppos)) {
            while(bp < (linebuf + hppos)) {
                *bp = ' ';
                ++bp;
            }

            strcpy(bp, hpbuf);
            sprintf(eos(bp), " [%d]", t1->maxhp);
        }
    }

    if(so == 0) {
        puts(linebuf);
    }
    else if(so > 0) {
        char *bp = eos(linebuf);
        
        if(so >= COLNO) {
            so = COLNO - 1;
        }

        while(bp < (linebuf + so)) {
            *bp = ' ';
            ++bp;
        }

        *bp = 0;

        standoutbeg();
        fputs(linebuf, stdout);
        standoutend();
        putchar('\n');
    }

    return strlen(linebuf);
}

char *itoa(int a)
{
    static char buf[12];
    sprintf(buf, "%d", a);

    return buf;
}

char *ordin(int n)
{
    int d = n % 10;

    if((d == 0) || (d > 3) || ((n / 10) == 1)) {
        return "th";
    }
    else if(d == 1) {
        return "st";
    }
    else if(d == 2) {
        return "nd";
    }
    else {
        return "rd";
    }
}

void clearlocks()
{
    int x;
    signal(SIGHUP, SIG_IGN);
    
    for(x = 1; x <= maxdlevel; ++x) {
        glo(x);

        /* Not all levels need to be present */
        unlink(lock);
    }

    *index(lock, '.') = 0;
    unlink(lock);
}

char *eos(char *s)
{
    while(*s != 0) {
        ++s;
    }

    return s;
}

/* It is the caller's responsibility to check that there is room for c */
void charcat(char *s, char c)
{
    while(*s != 0) {
        ++s;
    }

    *s = c;
    s++;
    *s = 0;
}

void prscore(int argc, char **argv)
{
    extern char *hname;
    char *player0;
    char **players;
    int playerct;
    int rank;
    struct toptenentry *t1;
    char *recfile = "record";
    FILE *rfile;
    int flg = 0;
    int i;

    rfile = fopen(recfile, "r");
    if(rfile == 0) {
        puts("Cannot open record file!");

        return;
    }

    if((argc > 1) && (strncmp(argv[1], "-s", 2) == 0)) {
        if(argv[1][2] == 0) {
            --argc;
            ++argv;
        }
        else if((argv[1][3] == 0) && (index("CFKSTWX", argv[1][2]) != 0)) {
            ++argv[1];
            argv[1][0] = '-';
        }
        else {
            argv[1] += 2;
        }
    }

    if(argc <= 1) {
        player0 = getlogin();
    
        if(player0 == NULL) {
            player0 = "player";
        }

        playerct = 1;
        players = &player0;
    }
    else {
        --argc;
        playerct = argc;
        ++argv;
        players = argv;
    }

    putchar('\n');

    tt_head = newttentry();
    t1 = tt_head;

    rank = 1;
    
    while(1) {
        if(fscanf(rfile,
                  "%d %d %d %d %ld %c %[^,],%[^\n]",
                  &t1->level,
                  &t1->maxlvl,
                  &t1->hp,
                  &t1->maxhp,
                  &t1->points,
                  &t1->plchar,
                  t1->str,
                  t1->death) != 8) {
            t1->points = 0;
        }

        if(t1->points == 0) {
            break;
        }

        for(i = 1; i < playerct; ++i) {
            if((strcmp(players[i], "all") == 0)
               || (strncmp(t1->str, players[i], NAMSZ) == 0)
               || ((players[i][0] == '-')
                   && (players[i][1] == t1->plchar)
                   && (players[i][2] == 0))
               || ((digit(players[i][0]) != 0)
                   && (rank <= atoi(players[i])))) {
                ++flg;
            }
        }

        t1->tt_next = newttentry();
        t1 = t1->tt_next;
        
        ++rank;
    }

    fclose(rfile);

    if(flg == 0) {
        printf("Cannot find any entries for ");
        
        if(playerct > 1) {
            printf("any of ");
        }

        for(i = 0; i < playerct; ++i) {
            if(i < (playerct - 1)) {
                printf("%s%s", players[i], ", ");
            }
            else {
                printf("%s%s", players[i], ".\n");
            }
        }

        printf("Call is: %s -s [playernames]\n", hname);

        return;
    }

    outheader();
    t1 = tt_head;

    for(rank = 1; t1->points != 0; ++rank) {
        for(i = 0; i < playerct; ++i) {
            if((strcmp(players[i], "all") == 0)
               || (strncmp(t1->str, players[i], NAMSZ) == 0)
               || ((players[i][0] == '-')
                   && (players[i][1] == t1->plchar)
                   && (players[i][2] == 0))
               || ((digit(players[i][0]) != 0)
                   && (rank <= atoi(players[i])))) {
                outentry(rank, t1, 0);
                
                return;
            }
        }

        outentry(rank, t1, 0);
        t1 = t1->tt_next;
    }
}
