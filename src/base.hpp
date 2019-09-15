/*
 * Copyright (C) 1995-2002 FSGames. Ported by Sean Ford and Yan Shosh
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __BASE_HPP__
#define __BASE_HPP__

/*
 * Base definitions (perhaps this should be broken up some more)
 *
 * Changelog
 *     buffers, 07/31/02:
 *         * C++ style includes for string and fstream
 *         * Deleted some redundant headers
 *         * Added math.h, ctype.h
 */

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <string>

#include <SDL2/SDL.h>

#include "gparser.hpp"
#include "input.hpp"
#include "pal32.hpp"
#include "pixdefs.hpp"
#include "pixie_data.hpp"
// #include "profiler.hpp"
#include "soundob.hpp" // Sound defines
#include "sounds.hpp"
#include "util.hpp"

#define DIFFICULTY_SETTINGS 3
#define VIDEO_ADDRESS 0xA000
#define VIDEO_LINEAR ((VIDEO_ADDRESS) << 4)
#define DPMI_INT 0x31
#define MAX_LEVELS 500 // Maximum number of scenarios allowed...
#define GRID_SIZE 16

#define PROT_MODE 1 // Comment this out when not in protected mode
#ifdef PROT_MODE
#define init_sound(x, y, z) while (0)
// #define play_sound(x) while (0)
#endif

#ifndef PROT_MODE
// Sound
extern "C" Sint16 init_sound(Uint8 *filename, Sint16 speed, Sint16 which);
extern "C" void play_sound(Sint16 which);
#endif

#define PIX(a, b) (((NUM_FAMILIES) * a) + b)

#define SCEN_TYPE_CAN_EXIT (char)1 // Make these go by power of 2. 1, 2, 4, 8
#define SCEN_TYPE_GEN_EXIT (char)2
#define SCEN_TYPE_SAVE_ALL (char)4 // Save named NPCs

#define ACTION_FOLLOW (char)1

// Used for the help-text system:
#define MAX_LINES 100 // Maximum number of lines in help file
#define HELP_WIDTH 100 // Maximum length of display line

// Color defines:
#define DEFAULT_TEXT_COLOR 88
#define PURE_WHITE 15
#define PURE_BLACK 0
#define WHITE 24
#define BLACK 160
#define GREY 23
#define YELLOW 88
#define RED 40
#define DARK_BLUE 72
#define LIGHT_BLUE 120
#define DARK_GREEN 63
#define LIGHT_GREEN 56

// Color cycling:
#define WATER_START 208
#define WATER_END 223
#define ORANGE_START 224
#define ORANGE_END 231

// Random defines:
// #define PROFILING
#define CHEAT_MODE 1 // Set to 0 for no cheats...

// Picture object class defs
// HP bar color defines
#define BAR_BACK_COLOR 11
#define BOX_COLOR 0
#define LOW_HP_COLOR 42
#define MID_HP_COLOR 237
#define HIGH_HP_COLOR 61
#define MAX_HP_COLOR 56 // When hp is over max :)

// MP bar color defines
#define LOW_MP_COLOR 42
#define MID_MP_COLOR 108
#define HIGH_MP_COLOR 72
#define MAX_MP_COLOR 64 // When mp is over max :)

// Generators are limited by this number
#define MAXOBJS 150

#define NORMAL_MODE 0 // #defines for walkputbuffer mode type
#define INVISIBLE_MODE 1
#define PHANTOM_MODE 2
#define OUTLINE_MODE 3

#define OUTLINE_NAMED 7 // #defines for outline colors
#define OUTLINE_INVULNERABLE 224
#define OUTLINE_FLYING 208
#define OUTLINE_INVISIBLE query_team_color()

#define SHIFT_LIGHTER 0 // #define for phantomputbuffer
#define SHIFT_DARKER 1
#define SHIFT_LEFT 2
#define SHIFT_RIGHT 3
#define SHIFT_RIGHT_RANDOM 4 // Shifts right 1 or 2 spaces (whole image)
#define SHIFT_RANDOM 5 // Shifts 1 or 2 right (on pixel x pixel basis)
#define SHIFT_BLOCKY 6 // Courtroom style

// Act types
#define ACT_RANDOM 0
#define ACT_FIRE 1
#define ACT_CONTROL 2
#define ACT_GUARD 3
#define ACT_GENERATE 4
#define ACT_DIE 5
#define ACT_SIT 6

// Team types
// #define MY_TEAM 0
// #define ELF_TEAM 1
// #define KNIGHT_TEAM 2
// #define MAX_TEAM 2
#define MAX_TEAM 7

// Other screen-type things
#define NUM_SPECIALS 6

// Animation types: Living
#define ANI_WALK 0
#define ANI_ATTACK 1
#define ANI_TELE_OUT 2
#define ANI_SKEL_GROW 3
#define ANI_TELE_IN 3
#define ANI_SLIME_SPLIT 4

// Animation types: weapons
#define ANI_GROW 1 // Trees have no attack animation
#define ANI_GLOWGROW 1 // Neither do sparkles
#define GLOWPULSE 2 // Sparkles cycling

// These are for effect objects...
#define ANI_EXAPND_8 1 // 1
#define ANI_DOOR_OPEN 1 // Door opening
#define ANI_SCARE 1 // 2 Ghost scare
#define ANI_BOMB 1 // 3 Thief's bomb
#define ANI_EXPLODE 1 // 4
#define ANI_SPIN 1 // For the marker

// Orders
#define ORDER_LIVING 0
#define ORDER_WEAPON 1
#define ORDER_TREASURE 2
#define ORDER_GENERATOR 3
#define ORDER_FX 4
#define ORDER_SPECIAL 5
#define ORDER_BUTTON1 6

// Living families
#define FAMILY_SOLDIER 0
#define FAMILY_ELF 1
#define FAMILY_ARCHER 2
#define FAMILY_MAGE 3
#define FAMILY_SKELETON 4
#define FAMILY_CLERIC 5
#define FAMILY_FIRE_ELEMENTAL 6
#define FAMILY_FAERIE 7
#define FAMILY_SLIME 8
#define FAMILY_SMALL_SLIME 9
#define FAMILY_MEDIUM_SLIME 10
#define FAMILY_THIEF 11
#define FAMILY_GHOST 12
#define FAMILY_DRUID 13
#define FAMILY_ORC 14
#define FAMILY_BIG_ORC 15
#define FAMILY_BARBARIAN 16
#define FAMILY_ARCHMAGE 17
#define FAMILY_GOLEM 18
#define FAMILY_GIANT_SKELETON 19
#define FAMILY_TOWER1 20
// Number of families; make sure to change the SIZE_FAMILIES in loader.cpp as
// well (or your code will act weird)
#define NUM_FAMILIES 21

// Weapon families
#define FAMILY_KNIFE 0
#define FAMILY_ROCK 1
#define FAMILY_ARROW 2
#define FAMILY_FIREBALL 3
#define FAMILY_TREE 4
#define FAMILY_METEOR 5
#define FAMILY_SPRINKLE 6
#define FAMILY_BONE 7
#define FAMILY_BLOOD 8
#define FAMILY_BLOB 9
#define FAMILY_FIRE_ARROW 10
#define FAMILY_LIGHTNING 11
#define FAMILY_GLOW 12
#define FAMILY_WAVE 13
#define FAMILY_WAVE2 14
#define FAMILY_WAVE3 15
#define FAMILY_CIRCLE_PROTECTION 16
#define FAMILY_HAMMER 17
#define FAMILY_DOOR 18
#define FAMILY_BOULDER 19

// Treasure families
#define FAMILY_STAIN 0
#define FAMILY_DRUMSITCH 1
#define FAMILY_GOLD_BAR 2
#define FAMILY_SILVER_BAR 3
#define FAMILY_MAGIC_POTION 4
#define FAMILY_INVIS_POTION 5
#define FAMILY_INVULNERABLE_POTION 6
#define FAMILY_FLIGHT_POTION 7
#define FAMILY_EXIT 8
#define FAMILY_TELEPORTER 9
#define FAMILY_LIFE_GEM 10 // Generated upon death
#define FAMILY_KEY 11
#define FAMILY_SPEED_POTION 12
#define MAX_TREASURE 12 // Number of biggest treasure...

// Generator families
#define FAMILY_TENT 0 // Skeletons
#define FAMILY_TOWER 1 // Mages
#define FAMILY_BONES 2 // Ghosts
#define FAMILY_TREEHOUSE 3 // Elves :)

// FX families
// #define FAMILY_STAIN 0
#define FAMILY_EXPAND 0
#define FAMILY_GHOST_SCARE 1
#define FAMILY_BOMB 2
#define FAMILY_EXPLOSION 3 // Bombs, etc.
#define FAMILY_FLASH 4 // Used for teleporter effects
#define FAMILY_MAGIC_SHIELD 5 // Revoloving protective shield
#define FAMILY_KNIFE_BACK 6 // Returning blade
#define FAMILY_BOOMERANG 7 // Circling boomerang
#define FAMILY_CLOUD 8 // Purple poison cloud
#define FAMILY_MARKER 9 // Marker for Mage's teleport
#define FAMILY_CHAIN 10 // 'Chain lightning' effect
#define FAMILY_DOOR_OPEN 11 // The open door
#define FAMILY_HIT 12 // Show when hit

// Special families
#define FAMILY_RESERVED_TEAM 0

// Button graphic families
#define FAMILY_NORMAL1 0
#define FAMILY_PLUS 1
#define FAMILY_MINUS 2
#define FAMILY_WRENCH 3

// Facings
#define FACE_UP 0
#define FACE_UP_RIGHT 1
#define FACE_RIGHT 2
#define FACE_DOWN_RIGHT 3
#define FACE_DOWN 4
#define FACE_DOWN_LEFT 5
#define FACE_LEFT 6
#define FACE_UP_LEFT 7
#define NUM_FACINGS 8

// Stats defines
#define COMMAND_WALK 1
#define COMMAND_FIRE 2
#define COMMAND_RANDOM_WALK 3 // Walk random direction...
#define COMMAND_DIE 4 // Bug fixing...
#define COMMAND_FOLLOW 5
#define COMMAND_RUSH 6 // Rush your enemy!
#define COMMAND_MULTIDO 7 // Do <com1> commands in one round
#define COMMAND_QUICK_FIRE 8 // Fires with no busy or animation
#define COMMAND_SET_WEAPON 9 // Set weapon type
#define COMMAND_RESET_WEAPON 10 // Restores weapon to default
#define COMMAND_SEARCH 11 // Use right-hand rule to find foe
#define COMMAND_ATTACK 12 // Attack/move to a close, current foe
#define COMMAND_RIGHT_WALK 13 // Use right-hand rule ONLY; no direct walk
#define COMMAND_UNCHARM 14 // Recover from being 'charmed'
#define REGEN (Sint32)4000 // Used to calculate time between heals

#define STANDARD_TEXT_TIME 75 // How many cycles to display text?
#define TEXT_1 "text.pix" // Standard text pixie
#define TEXT_BIG "textbig.pix" // Standard text pixie

#define DONT_DELETE 1

class video;
class screen;
class viewscreen;
class pixie;
class pixieN;

class walker;
class living;
class weap;
class treasure;
class effect;

class text;
class loader;
class statistics;
class command;
class guy;
class radar;

class soundob;
class smoother;

class PixieData;

extern screen *myscreen; // Global, available to anyone

// Some stuff for palette
typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
} rgb;

struct meminfo {
    Uint32 LargestBlockAvail;
    Uint32 MaxUnlockedPage;
    Uint32 LargestLockablePage;
    Uint32 LinAddrSpace;
    Uint32 NumFreePagesAvail;
    Uint32 NumPhysicalPagesFree;
    Uint32 TotalPhysicalPages;
    Uint32 FreeLinAddrSpace;
    Uint32 SizeOfPageFile;
    Uint32 Reserved[3];
};

typedef rgb palette[256];

Uint32 random(Uint32 x);
Sint16 fill_help_array(Uint8 somearray[HELP_WIDTH][MAX_LINES], SDL_RWops *infile);
Sint16 read_campaign_intro(screen *myscreen);
Sint16 read_scenario(screen *myscreen);
Uint8 *read_one_line(SDL_RWops *infiles, Sint16 length);

// Most of these are grpahlib and are being ported to video
void load_map_data(PixieData *whereto);
Uint8 *get_cfg_item(Uint8 *section, Uint8 *item);

// functions in game.cpp
Sint16 load_saved_game(char const *filename, screen *myscreen);

PixieData read_pixie_file(char const *filename);

void set_vga_palette(palette p);
rgb set_rgb(Uint8 r, Uint8 g, Uint8 b);
Sint16 read_palette(FILE *f, palette p);

#endif
