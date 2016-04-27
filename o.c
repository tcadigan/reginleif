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

#include <alloc.h>
#include <signal.h>

#ifdef MSDOS
#include <fcntl.h>
#endif

#include "oglob.h"

/* One of each spell */
#ifndef MSDOS
struct spell Spells[NUMSPELLS + 1];

#else

struct spell Spells[NUMSPELLS + 1] = { 0 };
#endif

/* Locations of city sites: [0] - found, [1] - x, [2] - y */
#ifndef MSDOS
int CitySiteList[NUMCITYSITES][3];

#else

int CitySiteList[NUMCITYSITES][3] = { 0 };
#endif

/* Current defined in caps since it is now a variable, was a constant */
int LENGTH = MAXLENGTH;
int WIDTH = MAXWIDTH;

#ifndef MSDOS
/* Game Status bit vector */
int GameStatus = 0L;

#else

/* Game Status bit vector */
long GameStatus = 0L;
#endif

#ifndef MSDOS
/* How large is level window */
int ScreenLength;

#else

/* How large is level window */
int ScreenLength = 0;
#endif

#ifndef MSDOS
/* The player */
struct player Player;

#else

/* The player */
struct player Players = { 0 };
#endif

#ifndef MSDOS
/* The countryside */
struct terrain Country[MAXWIDTH][MAXLENGTH];

#else

/* The countryside */
struct terrain Country[MAXWIDTH][MAXLENGTH] = { 0 };
#endif

#ifdef MSDOS
struct level TheLevel;
#endif

/* The city of Rampart */
struct level *City = NULL;

/* Place holder */
struct level *TempLevel = NULL;

/* Pointer to current Level */
struct level *Level = NULL;

/* Pointer to current Dungeon */
struct level *Dungeon = NULL;

#ifndef MSDOS
/* Current Village number */
int Villagenum;

#else

/* Current Village number */
int Villagenum = 0;
#endif

#ifndef MSDOS
/* Offset of displayed screen to level */
int ScreenOffset;

#else

/* Offset of displayed screen to level */
int ScreenOffset = 0;
#endif

#ifndef MSDOS
/* Deepest level allowed in dungeon */
int MaxDungeonLevels;

#else

/* Deepest level allowed in dungeon */
int MaxDungeonLevels = 0;
#endif

/* What is Dungeon now */
int Current_Dungeon = -1;

/* Which environment are we in */
int Current_Environment = E_CITY;

/* Which environment were we in */
int Laste_Environment = E_COUNTRYSIDE;

#ifndef MSDOS
/* 9 x,y directions */
int Dirs[2][9];

#else

/* 9 x,y directions */
int Dirs[2][9] = { 1, 1, -1, -1, 1, -1, 0, 0, 0, 1, -1, 1, -1, 0, 0, 1, -1, 0 };
#endif

/* Last player command */
char Cmd = 's';

/* How long does current command take */
int Command_Duration = 0;

/* Opponent in arena */
struct monster *Arena_Monster = NULL;

/* Case label of opponent in l_arena() */
int Arena_Opponent = 0;

#ifndef MSDOS
/* Did player win in arena? */
int Arena_Victory;

#else

/* Did player win in arena? */
int Arena_Victory = 0;
#endif

/* Amount of time spent in jail */
int Imprisonment = 0;

/* Hours of rain, snow, etc. */
int Precipitation = 0;

/* Effect of the moon on character */
int Lunarity = 0;

#ifndef MSDOS
/* Phase of the moon */
int Phase;

/* Starting date */
int Date;

/* Pawn Shop item generation date */
int Pawndate;

#else

/* Phase of the moon */
int Phase = 0;

/* Starting date */
int Date;

/* Pawn Shop item generation date */
int Pawndate = 0;
#endif

pob Pawnitmes[PAWNITEMS] = {
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
int ChaosStone = 0;

/* Magic stone counter */
int Mindstone = 0;

/* Number of times to search on 's' */
int Searchnum = 1;

/* Verbosity level */
int Verbosity = VERBOSE;

#ifndef MSDOS
/* Random seed */
char Seed;

#else

/* Random seed */
char Seed;
#endif

#ifndef MSDOS
/* Turn number */
int Time = 0;

#else

/* Turn number */
long Time = 0;
#endif

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

#ifndef MSDOS
/* Credit at Rampart gym */
int Gymcredit = 0;

#else

/* Credit at Rampart gym */
long Gymcredit = 0;
#endif

/* Research allowance at college */
int Spellsleft = 0;

/* Last date of star gem use */
int StarGemUse = 0;

/* Last date of high magic user */
int HiMagicUse = 0;

/* Current level for l_throne() */
int HiMagic = 0;

#ifndef MSDOS
/* Bank account */
int Balance = 0;

#else

/* Banke account */
long Balance = 0;
#endif

#ifndef MSDOS
/* Points are frozen after adepthood */
int FixedPoints = 0;

#else

/* Points are frozen after adepthood */
long FixedPoints = 0;
#endif

/* Previous position in village or city */
int LastTownLocX = 0;

/* Previous position in village or city */
int LastTownLocY = 0;

/* Previous position in countryside */
int LastCountryLocX = 0;

/* Previous position in countryside */
int LastCountryLocY = 0;

#ifndef MSDOS
/* Autoteller password */
char Password[64];

#else

/* Autoteller password */
char Password[64] = { 0 };
#endif

/* Some string space, random use */
#ifndef MSDOS
char Str1[100];
char Str2[100];
char Str3[100];
char Str4[100];

#else

char Str1[100] = { 0 };
char Str2[100] = { 0 };
char Str3[100] = { 0 };
char Str4[100] = { 0 };
#endif

/* Items in condo */
pol Condoitems = NULL;

/* High score names, levels, behavior */
#ifndef MSDOS
int Shadowlordbehavior;
int Archmagebehavior;
int Primebehavior;
int Commandantbehavior;
int Championbehavior;
int Preiestbehavior[7];
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
char Jusiciar[80];
int Shadowlordlevel;
int Archmagelevel;
int Primelevel;
int Commandantlevel;
int Dukelevel;

#else

int Shadowlordbehavior = 0;
int Archmagebehavior = 0;
int Primebehavior = 0;
int Commandantbehavior = 0;
int Championbehavior = 0;
int Priestbehavior[7] = { 0 };
int Hibehavior = 0;
int Dukebehavior = 0;
int Chaoslordbehavior = 0;
int Lawlordbehavior = 0;
int Justiciarbehavior = 0;
char Shadowlord[80] = { 0 };
char Archmage[80] = { 0 };
char Prime[80] = { 0 };
char Commadant[80] = { 0 };
char Duke[80] = { 0 };
char Champion[80] = { 0 };
char Priest[7][80] = { 0 };
char Hiscorer[80] = { 0 };
char Hidescrip[80] = { 0 };
char Chaoslord[80] = { 0 };
char Lawlord[80] = { 0 };
char Justiciar[80] = { 0 };
int Shadowlordlevel = 0;
int Archmagelevel = 0;
int Primelevel = 0;
int Commandantlevel = 0;
int Dukelevel = 0;
#endif

#ifndef MSDOS
int Championlevel;
int Priestlevel[7];
int Hiscore;
int Hilevel;
int Justiciarlevel;

#else

int Championlevel = 0;
int Priestlevel[7] = { 0 };
int Hilevel = 0;
int Justiciarlevel = 0;
long Highscore = 0L;
#endif

#ifndef MSDOS
int Chaoslordlevel;
int Lawlordlevel;
int Chaos;
int Law;

#else

int Chaoslordlevel = 0;
int Lawlordlevel = 0;
int Chaos = 0;
int Law = 0;
#endif

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

/* This may be implementation dependent. SRANDFUNCTION is defined in odefs.h */
void initrand()
{
    SRANDFUNCTION;
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

    printf("%lu\n", coreleft());
    getchar();

    /* Always catch ^c and hang-up signals */
    // signal(SIGINT, quit);

#ifndef MSDOS
    signal(SIGHUP, signalsave);

    if(CATCH_SIGNALS) {
        signal(SIGQUIT, signalexit);
        signal(SIGILL, signalexit);
        signal(SIGTRAP, signalexit);
        signal(SIGIOT, signalexit);
        signal(SIGEMT, signalexit);
        signal(SIGFPE, signalexit);
        signal(SIGBUS, signalexit);
        signal(SIGSEGV, signalexit);
        signal(SIGSYS, signalexit);
    }
#endif

    /* If filecheck is 0, some necessary data files are missing */
    if(filecheck() == 0) {
        exit(0);
    }

    /* All kinds of initialization */
    initgraf();

#ifndef MSDOS
    initdirs();
#endif

    initrand();
    initspells();

#ifndef MSDOS
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
#endif

#ifdef MSDOS
    msdos_init();
#endif

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

#ifndef MSDOS
int signalexit()
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
