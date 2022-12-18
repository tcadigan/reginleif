/*
 * Omega copyright (C) 1987-1989 by Laurence Raphael Brothers
 *
 * o.c
 * 
 * This file includes main() and some top-level functions. Most
 * globals originate in o.c. Objects and Monsters are initialized in
 * oinit.c
 *
 * Note: In order to avoid a memory bug I've been told about, I'm
 *       explicitly initializing every global to something
 */
#include "o.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "oaux2.h"
#include "ochar.h"
#include "ocity.h"
#include "ocom1.h"
#include "ocom2.h"
#include "ocountry.h"
#include "ofile.h"
#include "ogen1.h"
#include "oglob.h"
#include "oitem.h"
#include "otime.h"
#include "osave.h"
#include "oscr.h"
#include "ospell.h"
#include "outil.h"

/* One of each spell */
struct spell Spells[NUMSPELLS + 1];

/* Locations of city sites: [0] - found, [1] - x, [2] - y */
int CitySiteList[NUMCITYSITES][3];

/* Current defined in caps since it is now a variable, was a constant */
int LENGTH = MAXLENGTH;
int WIDTH = MAXWIDTH;

/* Game Status bit vector */
int GameStatus = 0;

/* How large is level window */
int ScreenLength;

/* The player */
struct player Player;

/* The countryside */
struct terrain Country[MAXWIDTH][MAXLENGTH];

/* The city of Rampart */
struct level *City = NULL;

/* Place holder */
struct level *TempLevel = NULL;

/* Pointer to current Level */
struct level *Level = NULL;

/* Pointer to current Dungeon */
struct level *Dungeon = NULL;

/* Current Village number */
int Villagenum;

/* Offset of displayed screen to level */
int ScreenOffset;

/* Deepest level allowed in dungeon */
int MaxDungeonLevels;

/* What is Dungeon now */
int Current_Dungeon = -1;

/* Which environment are we in */
int Current_Environment = E_CITY;

/* Which environment were we in */
int Last_Environment = E_COUNTRYSIDE;

/* 9 x,y directions */
int Dirs[2][9];

/* Last player command */
char Cmd = 's';

/* How long does current command take */
int Command_Duration = 0;

/* Opponent in arena */
struct monster *Arena_Monster = NULL;

/* Case label of opponent in l_arena() */
int Arena_Opponent = 0;

/* Did player win in arena? */
int Arena_Victory;

/* Amount of time spent in jail */
int Imprisonment = 0;

/* Hours of rain, snow, etc. */
int Precipitation = 0;

/* Effect of the moon on character */
int Lunarity = 0;

/* Phase of the moon */
int Phase;

/* Starting date */
int Date;

/* Pawn Shop item generation date */
int Pawndate;

pob Pawnitems[PAWNITEMS] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/* Items in the pawn shop */
/* Holy symbol use marker */
int SymbolUseHour = -1;

/* Crystal ball use marker */
int ViewHour = -1;

/* Staff of enchantment use marker */
int ZapHour = -1;

/* Helm of teleportation use marker */
int HelmHour = -1;

/* Dragonlord Attack State */
int Constriction = 0;

/* Altar Blessing State */
int Blessing = FALSE;

/* DPW date of dole */
int LastDay = -1;

/* Laste use of ritual magic */
int RitualHour = -1;

/* Last room of ritual magic */
int RitualRoom = -1;

/* Magic stone counter */
int Lawstone = 0;

/* Magic stone counter */
int Chaostone = 0;

/* Magic stone counter */
int Mindstone = 0;

/* Number of times to search on 's' */
int Searchnum = 1;

/* Verbosity level */
int Verbosity = VERBOSE;

/* Random seed */
char Seed;

/* Turn number */
int Time = 0;

/* 10 a turn; action coordinator */
int Tick = 0;

/* Last 10 strings */
char Stringbuffer[10][80] = {
    "First String",
    "First String",
    "First String",
    "First String",
    "First String",
    "First String",
    "First String",
    "First String",
    "First String",
    "First String"
};

/* Credit at Rampart gym */
int Gymcredit = 0;

/* Research allowance at college */
int Spellsleft = 0;

/* Last date of star gem use */
int StarGemUse = 0;

/* Last date of high magic user */
int HiMagicUse = 0;

/* Current level for l_throne() */
int HiMagic = 0;

/* Bank account */
int Balance = 0;

/* Points are frozen after adepthood */
int FixedPoints = 0;

/* Previous position in village or city */
int LastTownLocX = 0;

/* Previous position in village or city */
int LastTownLocY = 0;

/* Previous position in countryside */
int LastCountryLocX = 0;

/* Previous position in countryside */
int LastCountryLocY = 0;

/* Autoteller password */
char Password[64];

/* Some string space, random use */
char Str1[100];
char Str2[100];
char Str3[100];
char Str4[100];

/* Items in condo */
pol Condoitems = NULL;

/* High score names, levels, behavior */
int Shadowlordbehavior;
int Archmagebehavior;
int Primebehavior;
int Commandantbehavior;
int Championbehavior;
int Priestbehavior[7];
int Hibehavior;
int Dukebehavior;
int Chaoslordbehavior;
int Lawlordbehavior;
int Justiciarbehavior;
char Shadowlord[80];
char Archmage[80];
char Prime[80];
char Commandant[80];
char Duke[80];
char Champion[80];
char Priest[7][80];
char Hiscorer[80];
char Hidescrip[80];
char Chaoslord[80];
char Lawlord[80];
char Justiciar[80];
int Shadowlordlevel;
int Archmagelevel;
int Primelevel;
int Commandantlevel;
int Dukelevel;

int Championlevel;
int Priestlevel[7];
int Hiscore;
int Hilevel;
int Justiciarlevel;

int Chaoslordlevel;
int Lawlordlevel;
int Chaos;
int Law;

/* New globals which used to be statics */
int twiddle = FALSE;
int saved = FALSE;
int onewithchaos = FALSE;
int club_hinthour = 0;
int winnings = 0;
int tavern_hinthour;
int scroll_ids[30];
int potion_ids[30];
int stick_ids[30];
int ring_ids[30];
int cloak_ids[30];
int boot_ids[30];

int deepest[E_MAX + 1];

void initrand()
{
    srand(time(NULL) + Seed);
}

int game_restore(int argc, char *argv[])
{
    char savestr[80];
    int ok;

    if(argc == 2) {
        strcpy(savestr, argv[1]);
        ok = restore_game(savestr);
        unlink(savestr);

        if(!ok) {
            endgraf();
            printf("Try again with the right save file, luser!\n");
            exit(0);
        }

        return TRUE;
    }
    else {
        return FALSE;
    }
}

int main(int argc, char *argv[])
{
    int continuing;

    /* Always catch ^c and hang-up signals */
    // signal(SIGINT, quit);

    signal(SIGHUP, signalsave);

    if(CATCH_SIGNALS) {
        signal(SIGQUIT, signalexit);
        signal(SIGILL, signalexit);
        signal(SIGTRAP, signalexit);
        signal(SIGIOT, signalexit);
        signal(SIGFPE, signalexit);
        signal(SIGBUS, signalexit);
        signal(SIGSEGV, signalexit);
        signal(SIGSYS, signalexit);
    }

    /* If filecheck is 0, some necessary data files are missing */
    if(filecheck() == 0) {
        exit(0);
    }

    /* All kinds of initialization */
    initgraf();
    initdirs();
    initrand();
    initspells();

    strcpy(Stringbuffer[0], "First String");
    strcpy(Stringbuffer[1], "First String");
    strcpy(Stringbuffer[2], "First String");
    strcpy(Stringbuffer[3], "First String");
    strcpy(Stringbuffer[4], "First String");
    strcpy(Stringbuffer[5], "First String");
    strcpy(Stringbuffer[6], "First String");
    strcpy(Stringbuffer[7], "First String");
    strcpy(Stringbuffer[8], "First String");
    strcpy(Stringbuffer[9], "First String");

    /* game_restore() attempts to restore game if there is an argument */
    continuing = game_restore(argc, argv);

    /*
     * Monsters initialized in game_restore() if game is being
     * restored, items initialized in ramge_restore() if game is being
     * restored
     */
    if(!continuing) {
        inititem(1);
        initplayer();

        Date = random_range(360);
        Phase = random_range(24);
        moon_check();
        strcpy(Password, "");

        init_world();

        mprint("'?' for help or commandlist, 'Q' to quit.");
    }
    else {
        mprint("Your adventure continues...");
    }

    dataprint();
    timeprint();
    showflags();

    screencheck(Player.y);

    /* Game cycle */
    time_clock(TRUE);

    while(TRUE) {
        if(Current_Environment == E_COUNTRYSIDE) {
            p_country_process();
        }
        else {
            time_clock(FALSE);
        }
    }

    return 0;
}

void signalexit(int signum)
{
    mprint("Yikes!");
    morewait();
    mprint("Sorry, caught a core-dump signal.");
    mprint("Want to try and save the game?");

    if(ynq() == 'y') {
        /* Don't compress */
        save(FALSE);
    }

    mprint("Bye!");
    endgraf();

    exit(0);
}

/* Start up game with new dungeons; start with player in city */
void init_world()
{
    if(Level != NULL) {
        free((char *)Level);
    }

    if(City != NULL) {
        free((char *)City);
    }

    if(TempLevel != NULL) {
        free((char *)TempLevel);
    }

    if(Dungeon != NULL) {
        free_dungeon();
    }

    Dungeon = NULL;
    TempLevel = Dungeon;
    Level = TempLevel;
    City = Level;
    load_country();
    load_city();
    change_environment(E_CITY);
    locprint("The City of Rampart.");
}

/* Set variable item names */
void inititem(int reset)
{
    int i;

    if(reset) {
        scrollname(TRUE, 0);
    }

    for(i = 0; i < NUMSCROLLS; ++i) {
        Objects[SCROLLID + i].objstr = salloc(scrollname(FALSE, i));
    }

    if(reset) {
        potionname(TRUE, 0);
    }

    for(i = 0; i < NUMPOTIONS; ++i) {
        Objects[POTIONID + i].objstr = salloc(potionname(FALSE, i));
    }

    if(reset) {
        stickname(TRUE, 0);
    }

    for(i = 0; i < NUMSTICKS; ++i) {
        Objects[STICKID + i].objstr = salloc(stickname(FALSE, i));
    }

    if(reset) {
        bootname(TRUE, 0);
    }

    for(i = 0; i < NUMBOOTS; ++i) {
        Objects[BOOTID + i].objstr = salloc(bootname(FALSE, i));
    }

    if(reset) {
        cloakname(TRUE, 0);
    }

    for(i = 0; i < NUMCLOAKS; ++i) {
        Objects[CLOAKID + i].objstr = salloc(cloakname(FALSE, i));
    }

    if(reset) {
        ringname(TRUE, 0);
    }

    for(i = 0; i < NUMRINGS; ++i) {
        Objects[RINGID + i].objstr = salloc(ringname(FALSE, i));
    }
}
