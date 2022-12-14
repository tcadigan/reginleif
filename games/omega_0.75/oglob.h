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
extern struct monster Monsters[NUMMONSTERS];

/* One of each spell */
extern struct spell Spells[NUMSPELLS + 1];

/* One of each item */
extern struct object Objects[TOTALITEMS];

/* Locations of city sites [0] - found, [1] - x, [2] - y */
extern int CitySiteList[NUMCITYSITES][3];

/* The player */
extern struct player Player;

/* Level y dimension */
extern int LENGTH;

/* Level x dimension */
extern int WIDTH;

/* Game status bit vector */
extern int GameStatus;

/* How large is level window */
extern int ScreenLength;

/* The countryside */
extern struct terrain Country[MAXWIDTH][MAXLENGTH];

/* The city of Rampart */
extern struct level *City;

/* Player holder */
extern struct level *TempLevel;

/* Pointer to current dungeon */
extern struct level *Dungeon;

/* Pointer to current level */
extern struct level *Level;

/* What is dungeon now (an E_ constant) */
extern int Current_Dungeon;

/* Current village number */
extern int Villagenum;

/* Offset of displayed screen to level */
extern int ScreenOffset;

/* Deepest level allowed in dungeon */
extern int MaxDungeonLevels;

/* Which environment are we in (an E_ constant) */
extern int Current_Environment;

/* Which environment were we in last (an E_ constant) */
extern int Last_Environment;

/* 9 x,y directions */
extern int Dirs[2][9];

/* Last player command */
extern char Cmd;

/* How lond does current command take */
extern int Command_Duration;

/* Opponent in arena */
extern struct monster *Arena_Monster;

/* Case label of opponent in l_arena() */
extern int Arena_Opponent;

/* Did player win in arena? */
extern int Arena_Victory;

/* Amount of time spent in jail */
extern int Imprisonment;

/* Hours of rain, snow, etc. */
extern int Precipitation;

/* Phase of the moon */
extern int Phase;

/* How player is affected by moon */
extern int Lunarity;

/* Day of the year */
extern int Date;

/* Pawn shop item generation date */
extern int Pawndate;

/* Items in pawn shop */
extern pob Pawnitems[PAWNITEMS];

/* Crystal ball use marker */
extern int ViewHour;

/* Staff of enchantment use marker */
extern int ZapHour;

/* Helm of teleportation use marker */
extern int HelmHour;

/* Holy symbol use marker */
extern int SymbolUseHour;

/* Dragonlord attack state */
extern int Constriction;

/* Altar blessing state */
extern int Blessing;

/* DPW date of dole */
extern int LastDay;

/* Last use of ritual magic */
extern int RitualHour;

/* Last room use of ritual magic */
extern int RitualRoom;

/* Magic stone counter */
extern int Lawstone;

/* Magic stone counter */
extern int Chaostone;

/* Magic stone counter */
extern int Mindstone;

/* Number of times to search on 's' */
extern int Searchnum;

/* Verbosity level */
extern int Verbosity;

/* Random seed */
extern char Seed;

/* Turn number */
extern int Time;

/* Current second in minute; action coordinator */
extern int Tick;

/* The last printed strings */
extern char Stringbuffer[10][80];

/* Credit at Rampart gym */
extern int Gymcredit;

/* Research allowance at college */
extern int Spellsleft;

/* Last date of star gem use */
extern int StarGemUse;

/* Last date of high magic use */
extern int HiMagicUse;

/* Current level for l_throws */
extern int HiMagic;

/* Bank account */
extern int Balance;

/* Points are frozen after adepthood */
extern int FixedPoints;

/* Previous position in countryside */
extern int LastCountryLocX;

/* Previous position in countryside */
extern int LastCountryLocY;

/* Previous position in village or city */
extern int LastTownLocX;

/* Previous position in village or city */
extern int LastTownLocY;

/* Autoteller password */
extern char Password[64];

/* Items in condo */
extern pol Condoitems;

/* Some string space, random uses */
extern char Str1[100];
extern char Str2[100];
extern char Str3[100];
extern char Str4[100];

/* High score names, levels, behavior */
extern int Shadowlordbehavior;
extern int Archmagebehavior;
extern int Primebehavior;
extern int Justiciarbehavior;
extern int Commandantbehavior;
extern int Chaoslordbehavior;
extern int Lawlordbehavior;
extern int Championbehavior;
extern int Priestbehavior[7];
extern int Hibehavior;
extern int Dukebehavior;
extern char Shadowlord[80];
extern char Archmage[80];
extern char Prime[80];
extern char Commandant[80];
extern char Duke[80];
extern char Champion[80];
extern char Priest[7][80];
extern char Hiscorer[80];
extern char Hidescrip[80];
extern char Chaoslord[80];
extern char Lawlord[80];
extern char Justiciar[80];
extern int Shadowlordlevel;
extern int Archmagelevel;
extern int Primelevel;
extern int Commandantlevel;
extern int Dukelevel;
extern int Championlevel;
extern int Priestlevel[7];
extern int Hiscore;
extern int Hilevel;
extern int Justiciarlevel;
extern int Chaoslordlevel;
extern int Lawlordlevel;
extern int Chaos;
extern int Law;

/* New globals which used to be statics */
extern int twiddle;
extern int saved;
extern int onewithchaos;
extern int club_hinthour;
extern int winnings;
extern int tavern_hinthour;
extern int scroll_ids[30];
extern int potion_ids[30];
extern int stick_ids[30];
extern int ring_ids[30];
extern int cloak_ids[30];
extern int boot_ids[30];

extern int deepest[E_MAX + 1];
