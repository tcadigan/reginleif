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

#include <SDL2/SDL.h>

#define DIFFICULTY_SETTINGS 3
#define VIDEO_LINEAR ((VIDEO_ADDRESS) << 4)
#define DPMI_INT 0x31
#define MAX_LEVELS 500 // Maximum number of scenarios allowed...
#define GRID_SIZE 16

#define PROT_MODE 1 // Comment this out when not in protected mode
#ifdef PROT_MODE
#define init_sound(x, y, z) while (0)
#endif

#ifndef PROT_MODE
// Sound
extern "C" Sint16 init_sound(Uint8 *filename, Sint16 speed, Sint16 which);
extern "C" void play_sound(Sint16 which);
#endif

#define PIX(a, b) (((NUM_FAMILIES) * a) + b)

enum ScenTypeEnum : Uint8 {
    SCEN_TYPE_CAN_EXIT = 1, // Make these go by power of 2. 1, 2, 4, 8
    SCEN_TYPE_GEN_EXIT = 2,
    SCEN_TYPE_SAVE_ALL = 4 // Save named NPCs
};

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
#define MAXOBS 150

// For walkputbuffer mode type
enum ModeEnum : Uint8 {
    NORMAL_MODE = 0,
    INVISIBLE_MODE = 1,
    PHANTOM_MODE = 2,
    OUTLINE_MODE = 3
};

#define OUTLINE_NAMED 7 // #defines for outline colors
#define OUTLINE_INVULNERABLE 224
#define OUTLINE_FLYING 208
#define OUTLINE_INVISIBLE query_team_color()

// For phantomputbuffer
enum ShiftEnum : Uint8 {
    SHIFT_LIGHTER = 0,
    SHIFT_DARKER = 1,
    SHIFT_LEFT = 2,
    SHIFT_RIGHT = 3,
    SHIFT_RIGHT_RANDOM = 4, // Shifts right 1 or 2 spaces (whole image)
    SHIFT_RANDOM = 5, // Shifts 1 or 2 right (on pixel by pixel basis)
    SHIFT_BLOCKY = 6, // Courtroom style
};

// Act types
enum ActEnum : Uint8 {
    ACT_RANDOM = 0,
    ACT_FIRE = 1,
    ACT_CONTROL = 2,
    ACT_GUARD = 3,
    ACT_GENERATE = 4,
    ACT_DIE = 5,
    ACT_SIT = 6
};

// Team types
#define MAX_TEAM 7

// Other screen-type things
#define NUM_SPECIALS 6

// Animation types: Living
enum AnimationLivingEnum : Uint8 {
    ANI_WALK = 0,
    ANI_ATTACK = 1,
    ANI_TELE_OUT = 2,
    ANI_SKEL_GROW = 3,
    ANI_TELE_IN = 3,
    ANI_SLIME_SPLIT = 4
};

// Animation types: weapons
enum AnimationWeaponEnum : Uint8 {
    ANI_GROW = 1, // Trees have no attack animation
    ANI_GLOWGROW = 1, // Neither do sparkles
    GLOWPULSE = 2 // Sparkles cycling
};

// These are for effect objects...
enum AnimationEffectEnum : Uint8 {
    ANI_EXPAND_8 = 1, // 1
    ANI_DOOR_OPEN = 1, // Door opening
    ANI_SCARE = 1, // 2 Ghost scare
    ANI_BOMB = 1, // 3 Thief's bomb
    ANI_EXPLODE = 1, // 4
    ANI_SPIN = 1 // For the marker
};

// Orders
enum OrderEnum : Uint8 {
    ORDER_LIVING = 0,
    ORDER_WEAPON = 1,
    ORDER_TREASURE = 2,
    ORDER_GENERATOR = 3,
    ORDER_FX = 4,
    ORDER_SPECIAL = 5,
    ORDER_BUTTON1 = 6
};

// Living families
enum FamilyLivingEnum : Uint8 {
    FAMILY_SOLDIER = 0,
    FAMILY_ELF = 1,
    FAMILY_ARCHER = 2,
    FAMILY_MAGE = 3,
    FAMILY_SKELETON = 4,
    FAMILY_CLERIC = 5,
    FAMILY_FIRE_ELEMENTAL = 6,
    FAMILY_FAERIE = 7,
    FAMILY_SLIME = 8,
    FAMILY_SMALL_SLIME = 9,
    FAMILY_MEDIUM_SLIME = 10,
    FAMILY_THIEF = 11,
    FAMILY_GHOST = 12,
    FAMILY_DRUID = 13,
    FAMILY_ORC = 14,
    FAMILY_BIG_ORC = 15,
    FAMILY_BARBARIAN = 16,
    FAMILY_ARCHMAGE = 17,
    FAMILY_GOLEM = 18,
    FAMILY_GIANT_SKELETON = 19,
    FAMILY_TOWER1 = 20,
// Number of families; make sure to change the SIZE_FAMILIES in loader.cpp as
// well (or your code will act weird)
    NUM_FAMILIES = 21
};

// Weapon families
enum FamilyWeaponEnum : Uint8 {
    FAMILY_KNIFE = 0,
    FAMILY_ROCK = 1,
    FAMILY_ARROW = 2,
    FAMILY_FIREBALL = 3,
    FAMILY_TREE = 4,
    FAMILY_METEOR = 5,
    FAMILY_SPRINKLE = 6,
    FAMILY_BONE = 7,
    FAMILY_BLOOD = 8,
    FAMILY_BLOB = 9,
    FAMILY_FIRE_ARROW = 10,
    FAMILY_LIGHTNING = 11,
    FAMILY_GLOW = 12,
    FAMILY_WAVE = 13,
    FAMILY_WAVE2 = 14,
    FAMILY_WAVE3 = 15,
    FAMILY_CIRCLE_PROTECTION = 16,
    FAMILY_HAMMER = 17,
    FAMILY_DOOR = 18,
    FAMILY_BOULDER = 19
};

// Treasure families
enum FamilyTreasureEnum : Uint8 {
    FAMILY_STAIN = 0,
    FAMILY_DRUMSTICK = 1,
    FAMILY_GOLD_BAR = 2,
    FAMILY_SILVER_BAR = 3,
    FAMILY_MAGIC_POTION = 4,
    FAMILY_INVIS_POTION = 5,
    FAMILY_INVULNERABLE_POTION = 6,
    FAMILY_FLIGHT_POTION = 7,
    FAMILY_EXIT = 8,
    FAMILY_TELEPORTER = 9,
    FAMILY_LIFE_GEM = 10, // Generated upon death
    FAMILY_KEY = 11,
    FAMILY_SPEED_POTION = 12,
    MAX_TREASURE = 12 // Number of biggest treasure...
};

// Generator families
enum FamilyGeneratorEnum : Uint8 {
    FAMILY_TENT = 0, // Skeletons
    FAMILY_TOWER = 1, // Mages
    FAMILY_BONES = 2, // Ghosts
    FAMILY_TREEHOUSE = 3 // Elves :)
};

// FX families
enum FamilyEffectEnum : Uint8 {
// FAMILY_STAIN = 0,
    FAMILY_EXPAND = 0,
    FAMILY_GHOST_SCARE = 1,
    FAMILY_BOMB = 2,
    FAMILY_EXPLOSION = 3, // Bombs, etc.
    FAMILY_FLASH = 4, // Used for teleporter effects
    FAMILY_MAGIC_SHIELD = 5, // Revoloving protective shield
    FAMILY_KNIFE_BACK = 6, // Returning blade
    FAMILY_BOOMERANG = 7, // Circling boomerang
    FAMILY_CLOUD = 8, // Purple poison cloud
    FAMILY_MARKER = 9, // Marker for Mage's teleport
    FAMILY_CHAIN = 10, // 'Chain lightning' effect
    FAMILY_DOOR_OPEN = 11, // The open door
    FAMILY_HIT = 12 // Show when hit
};

// Special families
#define FAMILY_RESERVED_TEAM 0

// Button graphic families
enum FamilyButtonEnum : Uint8 {
    FAMILY_NORMAL1 = 0,
    FAMILY_PLUS = 1,
    FAMILY_MINUS = 2,
    FAMILY_WRENCH = 3
};

// Facings
enum FacingsEnum : Uint8 {
    FACE_UP = 0,
    FACE_UP_RIGHT = 1,
    FACE_RIGHT = 2,
    FACE_DOWN_RIGHT = 3,
    FACE_DOWN = 4,
    FACE_DOWN_LEFT = 5,
    FACE_LEFT = 6,
    FACE_UP_LEFT = 7,
    NUM_FACINGS = 8
};

// Stats defines
enum CommandEnum : Uint8 {
    COMMAND_WALK = 1,
    COMMAND_FIRE = 2,
    COMMAND_RANDOM_WALK = 3, // Walk random direction...
    COMMAND_DIE = 4, // Bug fixing...
    COMMAND_FOLLOW = 5,
    COMMAND_RUSH = 6, // Rush your enemy!
    COMMAND_MULTIDO = 7, // Do <com1> commands in one round
    COMMAND_QUICK_FIRE = 8, // Fires with no busy or animation
    COMMAND_SET_WEAPON = 9, // Set weapon type
    COMMAND_RESET_WEAPON = 10, // Restores weapon to default
    COMMAND_SEARCH = 11, // Use right-hand rule to find foe
    COMMAND_ATTACK = 12, // Attack/move to a close, current foe
    COMMAND_RIGHT_WALK = 13, // Use right-hand rule ONLY; no direct walk
    COMMAND_UNCHARM = 14, // Recover from being 'charmed'
};

#define REGEN (Sint32)4000 // Used to calculate time between heals

#define STANDARD_TEXT_TIME 75 // How many cycles to display text?
#define TEXT_1 "text.pix" // Standard text pixie
#define TEXT_BIG "textbig.pix" // Standard text pixie

// These are keyboard defines... high level
#define KEY_UP 0
#define KEY_UP_RIGHT 1
#define KEY_RIGHT 2
#define KEY_DOWN_RIGHT 3
#define KEY_DOWN 4
#define KEY_DOWN_LEFT 5
#define KEY_LEFT 6
#define KEY_UP_LEFT 7
#define KEY_FIRE 8
#define KEY_SPECIAL 9
#define KEY_SWITCH 10
#define KEY_SPECIAL_SWITCH 11
#define KEY_YELL 12
#define KEY_SHIFTER 13
#define KEY_PREFS 14
#define KEY_CHEAT 15
#define NUM_KEYS 16

#endif
