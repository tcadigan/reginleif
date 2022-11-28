/*
 * Omega copyright (C) 1987-1989 by Laurence Raphael Brothers
 *
 * oglob.h
 *
 * This file contains declarations of global variables used through the
 * program. It includes the other header files, so every program module (except
 * o.c) can just include this file.
 */

#include "odefs.h"

/* One of each monster */
struct monster Monsters[NUMMONSTERS];

/* One of each spell */
struct spell Spells[NUMSPELLS + 1];

/* One of each item */
struct object Objects[TOTALITEMS];

/* Locations of city sites [0] - found, [1] - x, [2] - y */
int CitySiteList[NUMCITYSITES][3];

/* The player */
struct player Player;

/* Level y dimension */
int LENGTH;

/* Level x dimension */
int WIDTH;

/* Game status bit vector */
int GameStatus;

/* How large is level window */
int ScreenLength;

/* The countryside */
struct terrain Country[MAXWIDTH][MAXLENGTH];

/* The city of Rampart */
struct level *City;

/* Player holder */
struct level *TempLevel;

/* Pointer to current dungeon */
struct level *Dungeon;

/* Pointer to current level */
struct level *Level;

/* What is dungeon now (an E_ constant) */
int Current_Dungeon;

/* Current village number */
int Villagenum;

/* Offset of displayed screen to level */
int ScreenOffset;

/* Deepest level allowed in dungeon */
int MaxDungeonLevels;

/* Which environment are we in (an E_ constant) */
int Current_Environment;

/* Which environment were we in last (an E_ constant) */
int Last_Environment;

/* 9 x,y directions */
int Dirs[2][9];

/* Last player command */
char Cmd;

/* How lond does current command take */
int Command_Duration;

/* Opponent in arena */
struct monster *Arena_Monster;

/* Case label of opponent in l_arena() */
int Arena_Opponent;

/* Did player win in arena? */
int Arena_Victory;

/* Amount of time spent in jail */
int Imprisonment;

/* Hours of rain, snow, etc. */
int Precipitation;

/* Phase of the moon */
int Phase;

/* How player is affected by moon */
int Lunarity;

/* Day of the year */
int Date;

/* Pawn shop item generation date */
int PawnDate;

/* Items in pawn shop */
pob Pawnitems[PAWNITEMS];

/* Crystal ball use marker */
int ViewHour;

/* Staff of enchantment use marker */
int ZapHour;

/* Helm of teleportation use marker */
int HelmHour;

/* Holy symbol use marker */
int SymbolUseHour;

/* Dragonlord attack state */
int Constriction;

/* Altar blessing state */
int Blessing;

/* DPW date of dole */
int LastDay;

/* Last use of ritual magic */
int RitualHour;

/* Last room use of ritual magic */
int RitualRoom;

/* Magic stone counter */
int Lawstone;

/* Magic stone counter */
int Chaostone;

/* Magic stone counter */
int Mindstone;

/* Number of times to search on 's' */
int Searchnum;

/* Verbosity level */
int Verbosity;

/* Random seed */
char Seed;

/* Turn number */
int Time;

/* Current second in minute; action coordinator */
int Tick;

/* The last printed strings */
char Stringbuffer[10][80];

/* Credit at Rampart gym */
int Gymcredit;

/* Research allowance at college */
int Spellsleft;

/* Last date of star gem use */
int StarGemUse;

/* Last date of high magic use */
int HiMagicUse;

/* Current level for l_throws */
int HiMagic;

/* Bank account */
int Balance;

/* Points are frozen after adepthood */
int FixedPoints;

/* Previous position in countryside */
int LastCountryLocX;

/* Previous position in countryside */
int LastCountryLocY;

/* Previous position in village or city */
int LastTownLocX;

/* Previous position in village or city */
int LastTownLocY;

/* Autoteller password */
char Password[64];

/* Items in condo */
pol Condoitems;

/* Some string space, random uses */
char Str1[100];
char Str2[100];
char Str3[100];
char Str4[100];

/* High score names, levels, behavior */
int Shadowlordbehavior;
int Archmagebehavior;
int Primebehavior;
int Justiciarbehavior;
int Commandantbehavior;
int Chaoslordbehavior;
int Lawlordbehavior;
int Championbehavior;
int Priestbehavior[7];
int Hibehavior;
int Dukebehavior;
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
int twiddle;
int saved;
int onewithchaos;
int club_hinthour;
int winnings;
int tavern_hinthour;
int scroll_ids[30];
int potion_ids[30];
int stick_ids[30];
int ring_ids[30];
int cloak_ids[30];
int boot_ids[30];

int deepest[E_MAX + 1];
