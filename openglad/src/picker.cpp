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

#include "picker.hpp"

#include "button.hpp"
#include "campaign_result.hpp"
#include "glad.hpp"
#include "gloader.hpp"
#include "gparser.hpp"
#include "graphlib.hpp"
#include "guy.hpp"
#include "help.hpp"
#include "input.hpp"
#include "io.hpp"
#include "level_editor.hpp"
#include "level_picker.hpp"
#include "mouse_state.hpp"
#include "options.hpp"
#include "pal32.hpp"
#include "pixien.hpp"
#include "util.hpp"
#include "version.hpp"
#include "video_screen.hpp"
#include "view.hpp"
#include "virtual_button.hpp"
#include "walker.hpp"

#include <cstring>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>

#define DOWN(x) (72 + ((x) * 15))
#define VIEW_DOWN(x) (10 + ((x) * 20))
#define RAISE 1.85 // Please also change in guy.cpp

#define EXIT 1 // These are leftclick return values, exit means leave picker
#define REDRAW 2 // We just exited a menu, so redraw your buttons
#define OK 4 // This function was successful, continue normal operation

#define BUTTON_HEIGHT 15
#define BUTTON_PADDING 8
#define BUTTON_PITCH (BUTTON_HEIGHT + BUTTON_PADDING)

#define YES 5
#define NO 6

// Left edge margin...
#define DETAIL_LM 11
// Center margin...
#define DETAIL_MM 164
// Vertical line for text
#define DETAIL_LD(x) (90 + ((x) * 6))

#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
#define GET_RAND_ELEM(array) ((array)[rand() % ARRAY_SIZE((array))])

#define WL(p, m)                                                        \
    if ((m)[1] != ' ') {                                                \
        mytext.write_xy(DETAIL_LM, DETAIL_LD((p)), (m), RED, 1);        \
    } else {                                                            \
        mytext.write_xy(DETAIL_LM, DETAIL_LD((p)), (m), DARK_BLUE, 1);  \
    }

#define WR(p, m)                                                        \
    if ((m)[1] != ' ') {                                                \
        mytext.write_xy(DETAIL_MM, DETAIL_LD((p)), (m), RED, 1);        \
    } else {                                                            \
        mytext.write_xy(DETAIL_MM, DETAIL_LD((p)), (m), DARK_BLUE, 1);  \
    }

#define STAT_NUM_OFFSET 42
#define STAT_COLOR DARK_BLUE // Color for normal stat text
#define STAT_CHANGED RED // Color for changed stat text
#define STAT_LEVELED LIGHT_BLUE // Color for leveled up stat text
#define STAT_DISABLED BLACK // Color for disabled stat text
#define STAT_DERIVED (DARK_BLUE + 3)

#define MENU_NAV_DEFAULT false

// Shows the current guy...
std::string get_saved_name(std::string const &filename);
Sint32 mainmenu(Sint32 arg1);
Uint32 calculate_hire_cost();
Uint32 calculate_train_cost(Guy *oldguy);
void statscopy(Guy *dest, Guy *source); // Copy stats from source => dest

PixieN *backdrops[5];
Sint32 current_difficulty = 1; // Setting "normal"
Sint32 difficulty_level[DIFFICULTY_SETTINGS] = { 50, 100, 200 };

Guy *current_guy = nullptr;
Guy *old_guy = nullptr;

// Global for editing guys...
Sint32 editguy = 0;
PixieN *main_title_logo_pix;
PixieN *main_columns_pix;

bool menu_nav_enabled = MENU_NAV_DEFAULT;
Uint32 menu_nav_enabled_time = 0;

// Global so we can delete the buttons anywhere
VirtualButton *localbuttons;
Sint16 current_team_num = 0;

Sint32 allowable_guys[] = {
    FAMILY_SOLDIER,
    FAMILY_BARBARIAN,
    FAMILY_ELF,
    FAMILY_ARCHER,
    FAMILY_MAGE,
    FAMILY_CLERIC,
    FAMILY_THIEF,
    FAMILY_DRUID,
    FAMILY_ORC,
    FAMILY_SKELETON,
    FAMILY_FIRE_ELEMENTAL,
    FAMILY_SMALL_SLIME,
    FAMILY_FAERIE,
    FAMILY_GHOST
};

// Guy type we're looking at
Sint32 current_type = 0;

// Used to label new hires, like "SOLDIER5"
Sint32 numbought[NUM_FAMILIES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

#ifndef DISABLE_MULTIPLAYER
Button mainmenu_buttons[] = {
    // BEGIN NEW GAME
    Button("", KEYSTATE_UNKNOWN, 80, 50, 140, 20, BEGIN_MENU, 1, MenuNav::Down(1), false),
    Button("CONTINUE GAME", KEYSTATE_UNKNOWN, 80, 75, 140, 20, CREATE_TEAM_MENU, -1, MenuNav::UpDown(0, 5)),
    Button("4 PLAYER", KEYSTATE_4, 152, 125, 68, 20, SET_PLAYER_MODE, 4, MenuNav::UpDownLeft(4, 6, 3)),
    Button("3 PLAYER", KEYSTATE_3, 80, 125, 68, 20, SET_PLAYER_MODE, 3, MenuNav::UpDownRight(5, 6, 2)),
    Button("2 PLAYER", KEYSTATE_2, 150, 100, 68, 20, SET_PLAYER_MODE, 2, MenuNav::UpDownLeft(1, 2, 5)),
    Button("1 PLAYER", KEYSTATE_1, 80, 100, 68, 20, SET_PLAYER_MODE, 1, MenuNav::UpDownRight(1, 3, 4)),
    Button("DIFFICULTY", KEYSTATE_UNKNOWN, 80, 148, 140, 10, SET_DIFFICULTY, -1, MenuNav::UpDown(3, 7)),
    Button("PvP: Allied", KEYSTATE_UNKNOWN, 80, 160, 68, 10, ALLIED_MODE, -1, MenuNav::UpDownRight(6, 9, 8)),
    Button("Level Edit", KEYSTATE_UNKNOWN, 152, 160, 68, 10, DO_LEVEL_EDIT, -1, MenuNav::UpDownLeft(6, 9, 7)),
    Button("QUIT", KEYSTATE_ESCAPE, 120, 175, 60, 20, QUIT_MENU, 0, MenuNav::UpLeft(7, 10)),
    Button("", KEYSTATE_UNKNOWN, 90, 175, 20, 20, MAIN_OPTIONS, -1, MenuNav::UpRight(7, 9))
};

#define OPTIONS_BUTTON_INDEX 10

#else

// Modified main screen with no multiplayer
Button mainmenu_buttons[] = {
    // BEGIN NEW GAME
    Button("", KEYSTATE_UNKNOWN, 80, 70, 140, 20, BEGINMENU, 1, MenuNav::Down(1), false),
    Button("CONTINUE GAME", KEYSTATE_UNKNOWN, 80, 95, 140, 20, CREATE_TEAM_MENU, -1, MenuNav::UpDown(0, 2)),
    Button("DIFFICULTY", KEYSTATE_UNKNOWN, 80, 120, 140, 15, SET_DIFFICULTY, -1, MenuNav::UpDown(1, 3)),
    Button("Level Edit", KEYSTATE_UNKNOWN, 80, 137, 140, 15, DO_LEVEL_EDIT, -1, MenuNav::UpDown(2, 4)),
    Button("QUIT", KEYSTATE_ESCAPE, 120, 154, 60, 20, QUIT_MENU, 0, MenuNav::UpLeft(3, 5)),
    Button("", KEYSTATE_UNKNOWN, 90, 154, 20, 20, MAIN_OPTIONS, -1, MenuNav::UpRight(4, 5))
};

#define OPTIONS_BUTTON_INDEX 5

#endif

Button main_options_buttons[] = {
    Button("BACK", KEYSTATE_ESCAPE, 40, 10, 50, 15, RETURN_MENU, EXIT, MenuNav::UpDownRight(12, 1, 14)),
    Button("Sound", KEYSTATE_UNKNOWN, 135, 10 + BUTTON_PITCH, 50, 15, TOGGLE_SOUND, -1, MenuNav::UpDown(0, 2)),
    Button("NORMAL", KEYSTATE_UNKNOWN, 130, 10 + (2 * BUTTON_PITCH), 60, 15, TOGGLE_RENDERING_ENGINE, -1, MenuNav::UpDownRight(1, 4, 3)),
    Button("Fullscreen", KEYSTATE_UNKNOWN, 210, 10 + (2 * BUTTON_PITCH), 90, 15, TOGGLE_FULLSCREEN, -1, MenuNav::UpDownLeft(1, 5, 2)),
    Button("-", KEYSTATE_UNKNOWN, 130, 10 + (3 * BUTTON_PITCH), 30, 15, 0, -1, MenuNav::UpDownRight(2, 6, 5)),
    Button("+", KEYSTATE_UNKNOWN, 170, 10 + (3 * BUTTON_PITCH), 30, 15, 0 , 1, MenuNav::UpDownLeft(3, 7, 4)),
    Button("Mini HP bar", KEYSTATE_UNKNOWN, 80, 10 + (4 * BUTTON_PITCH), 90, 15, TOGGLE_MINI_HP_BAR, -1, MenuNav::UpDownRight(4, 8, 7)),
    Button("Hit flash", KEYSTATE_UNKNOWN, 210, 10 + (4 * BUTTON_PITCH), 90, 15, TOGGLE_HIT_FLASH, -1, MenuNav::UpDownLeft(5, 9, 6)),
    Button("Hit recoil", KEYSTATE_UNKNOWN, 80, 10 + (5 * BUTTON_PITCH), 90, 15, TOGGLE_HIT_RECOIL, -1, MenuNav::UpDownRight(6, 10, 9)),
    Button("Attack lunge", KEYSTATE_UNKNOWN, 210, 10 + (5 * BUTTON_PITCH), 90, 15, TOGGLE_ATTACK_LUNGE, -1, MenuNav::UpDownLeft(7, 11, 8)),
    Button("Hit sparks", KEYSTATE_UNKNOWN, 80, 10 + (6 * BUTTON_PITCH), 90, 15, TOGGLE_HIT_ANIM, -1, MenuNav::UpDownRight(8, 12, 11)),
    Button("Damage numbers", KEYSTATE_UNKNOWN, 210, 10 + (6 * BUTTON_PITCH), 90, 15, TOGGLE_DAMAGE_NUMBERS, -1, MenuNav::UpDownLeft(9, 13, 10)),
    Button("Healing numbers", KEYSTATE_UNKNOWN, 80, 10 + (7 * BUTTON_PITCH), 90, 15, TOGGLE_HEAL_NUMBERS, -1, MenuNav::UpDownRight(10, 0, 13)),
    Button("Gore", KEYSTATE_UNKNOWN, 210, 10 + (7 * BUTTON_PITCH), 90, 15, TOGGLE_GORE, -1, MenuNav::UpDownLeft(11, 0, 12)),
    Button("RESTORE DEFAULTS", KEYSTATE_UNKNOWN, 170, 10, 120, 15, RESTORE_DEFAULT_SETTINGS, -1, MenuNav::UpDownLeft(12, 1, 0))
};

// beginmenu (first menu of new game), create_team_menu
Button createmenu_buttons[] = {
    Button("VIEW TEAM", KEYSTATE_UNKNOWN, 30, 70, 80, 15, CREATE_VIEW_MENU, -1, MenuNav::DownRight(3, 1)),
    Button("TRAIN TEAM", KEYSTATE_UNKNOWN, 120, 70, 80, 15, CREATE_TRAIN_MENU, -1, MenuNav::DownLeftRight(4, 0, 2)),
    Button("HIRE TROOPS", KEYSTATE_UNKNOWN, 210, 70, 80, 15, CREATE_HIRE_MENU, -1, MenuNav::DownLeft(5, 1)),
    Button("LOAD TEAM", KEYSTATE_UNKNOWN, 30, 100, 80, 15, CREATE_LOAD_MENU, -1, MenuNav::UpDownRight(0, 6, 4)),
    Button("SAVE TEAM", KEYSTATE_UNKNOWN, 120, 100, 80, 15, CREATE_SAVE_MENU, -1, MenuNav::UpLeftRight(1, 3, 5)),
    Button("GO", KEYSTATE_UNKNOWN, 210, 100, 80, 15, GO_MENU, -1, MenuNav::UpDownLeft(2, 7, 4)),
    Button("BACK", KEYSTATE_ESCAPE, 30, 140, 60, 30, RETURN_MENU, EXIT, MenuNav::UpRight(3, 7)),
    Button("SET LEVEL", KEYSTATE_UNKNOWN, 210, 140, 80, 20, DO_SET_SCEN_LEVEL, EXIT, MenuNav::UpDownLeft(5, 8, 6)),
    Button("SET CAMPAIGN", KEYSTATE_UNKNOWN, 210, 170, 80, 20, DO_PICK_CAMPAIGN, EXIT, MenuNav::UpLeft(7, 6))
};

Button viewteam_buttons[] = {
    Button("GO", KEYSTATE_UNKNOWN, 270, 170, 40, 20, GO_MENU, -1, MenuNav::Left(1)),
    Button("BACK", KEYSTATE_ESCAPE, 10, 170, 44, 20, RETURN_MENU, EXIT, MenuNav::Right(0))
};

Button details_buttons[] = {
    Button("BACK", KEYSTATE_ESCAPE, 10, 170, 40, 20, RETURN_MENU, EXIT, MenuNav::UpRight(1, 1)),
    // PROMOTE
    Button(160, 4, 315 - 160, 66 - 4, 0, -1, MenuNav::DownLeft(0, 0), false, true)
};

Button trainmenu_buttons[] = {
    Button("PREV", KEYSTATE_UNKNOWN, 10, 40, 40, 20, CYCLE_TEAM_GUY, -1, MenuNav::DownRight(2, 1)),
    Button("NEXT", KEYSTATE_UNKNOWN, 110, 40, 40, 20, CYCLE_TEAM_GUY, 1, MenuNav::DownLeftRight(3, 0, 16)),
    Button("", KEYSTATE_UNKNOWN, 16, 70, 16, 10, DECREASE_STAT, BUT_STR, MenuNav::UpDownRight(0, 4, 3)),
    Button("", KEYSTATE_UNKNOWN, 126, 70, 16, 12, INCREASE_STAT, BUT_STR, MenuNav::UpDownLeft(1, 5, 2)),
    Button("", KEYSTATE_UNKNOWN, 16, 85, 16, 10, DECREASE_STAT, BUT_DEX, MenuNav::UpDownRight(2, 6, 5)),
    Button("", KEYSTATE_UNKNOWN, 126, 85, 16, 12, INCREASE_STAT, BUT_DEX, MenuNav::UpDownLeft(3, 7, 4)),
    Button("", KEYSTATE_UNKNOWN, 16, 100, 16, 10, DECREASE_STAT, BUT_CON, MenuNav::UpDownRight(4, 8, 7)),
    Button("", KEYSTATE_UNKNOWN, 126, 100, 16, 12, INCREASE_STAT, BUT_CON, MenuNav::UpDownLeft(5, 9, 6)),
    Button("", KEYSTATE_UNKNOWN, 16, 115, 16, 10, DECREASE_STAT, BUT_INT, MenuNav::UpDownRight(6, 10, 9)),
    Button("", KEYSTATE_UNKNOWN, 126, 115, 16, 12, INCREASE_STAT, BUT_INT, MenuNav::UpDownLeft(7, 11, 8)),
    Button("", KEYSTATE_UNKNOWN, 16, 130, 16, 10, DECREASE_STAT, BUT_ARMOR, MenuNav::UpDownRight(8, 12, 11)),
    Button("", KEYSTATE_UNKNOWN, 126, 130, 16, 12, INCREASE_STAT, BUT_ARMOR, MenuNav::UpDownLeft(9, 13, 10)),
    Button("", KEYSTATE_UNKNOWN, 16, 145, 16, 10, DECREASE_STAT, BUT_LEVEL, MenuNav::UpDownRight(10, 19, 13)),
    Button("", KEYSTATE_UNKNOWN, 126, 145, 16, 12, INCREASE_STAT, BUT_LEVEL, MenuNav::UpDownLeftRight(11, 15, 12, 18)),
    Button("VIEW TEAM", KEYSTATE_UNKNOWN, 190, 170, 90, 20, CREATE_VIEW_MENU, -1, MenuNav::UpLeft(18, 15)),
    Button("ACCEPT", KEYSTATE_UNKNOWN, 80, 170, 80, 20, EDIT_GUY, -1, MenuNav::UpLeftRight(13, 19, 14)),
    Button("RENAME", KEYSTATE_UNKNOWN, 174, 8, 64, 22, NAME_GUY, 1, MenuNav::DownLeftRight(18, 1, 17)),
    Button("DETAILS...", KEYSTATE_UNKNOWN, 240, 8, 64, 22, CREATE_DETAIL_MENU, 0, MenuNav::DownLeft(18, 16)),
    Button("Playing on Team X", KEYSTATE_UNKNOWN, 174, 138, 133, 22, CHANGE_TEAM, 1, MenuNav::UpDownLeft(17, 14, 13)),
    Button("BACK", KEYSTATE_ESCAPE, 10, 170, 40, 20, RETURN_MENU, EXIT, MenuNav::UpRight(12, 15))
};

Button hiremenu_buttons[] = {
    Button("PREV", KEYSTATE_UNKNOWN, 10, 40, 40, 20, CYCLE_GUY, -1, MenuNav::DownRight(4, 1)),
    Button("NEXT", KEYSTATE_UNKNOWN, 110, 40, 40, 20, CYCLE_GUY, 1, MenuNav::DownLeftRight(3, 0, 3)),
    Button("Hiring for Team X", KEYSTATE_UNKNOWN, 109, 170, 110, 20, CHANGE_HIRE_TEAM, 1, MenuNav::UpLeft(1, 3)),
    Button("HIRE ME", KEYSTATE_UNKNOWN, 82, 166, 88, 28, ADD_GUY, -1, MenuNav::UpLeftRight(1, 4, 2)),
    Button("BACK", KEYSTATE_ESCAPE, 10, 170, 40, 20, RETURN_MENU, EXIT, MenuNav::UpRight(0, 3))
};

Button saveteam_buttons[] = {
    Button("SLOT ONE", KEYSTATE_UNKNOWN, 25, 25, 220, 10, DO_SAVE, 1, MenuNav::UpDown(10, 1)),
    Button("SLOT TWO", KEYSTATE_UNKNOWN, 25, 40, 220, 10, DO_SAVE, 2, MenuNav::UpDown(0, 2)),
    Button("SLOT THREE", KEYSTATE_UNKNOWN, 25, 55, 220, 10, DO_SAVE, 3, MenuNav::UpDown(1, 3)),
    Button("SLOT FOUR", KEYSTATE_UNKNOWN, 25, 70, 220, 10, DO_SAVE, 4, MenuNav::UpDown(2, 4)),
    Button("SLOT FIVE", KEYSTATE_UNKNOWN, 25, 85, 220, 10, DO_SAVE, 5, MenuNav::UpDown(3, 5)),
    Button("SLOT SIX", KEYSTATE_UNKNOWN, 25, 100, 220, 10, DO_SAVE, 6, MenuNav::UpDown(4, 6)),
    Button("SLOT SEVEN", KEYSTATE_UNKNOWN, 25, 115, 220, 10, DO_SAVE, 7, MenuNav::UpDown(5, 7)),
    Button("SLOT EIGHT", KEYSTATE_UNKNOWN, 25, 130, 220, 10, DO_SAVE, 8, MenuNav::UpDown(6, 8)),
    Button("SLOT NINE", KEYSTATE_UNKNOWN, 25, 145, 220, 10, DO_SAVE, 9, MenuNav::UpDown(7, 9)),
    Button("SLOT TEN", KEYSTATE_UNKNOWN, 25, 160, 220, 10, DO_SAVE, 10, MenuNav::UpDown(8, 10)),
    Button("BACK", KEYSTATE_ESCAPE, 25, 175, 40, 20, RETURN_MENU, EXIT, MenuNav::UpDown(9, 0))
};

Button loadteam_buttons[] = {
    Button("SLOT ONE", KEYSTATE_UNKNOWN, 25, 25, 220, 10, DO_LOAD, 1, MenuNav::UpDown(10, 1)),
    Button("SLOT TWO", KEYSTATE_UNKNOWN, 25, 40, 220, 10, DO_LOAD, 2, MenuNav::UpDown(0, 2)),
    Button("SLOT THREE", KEYSTATE_UNKNOWN, 25, 55, 220, 10, DO_LOAD, 3, MenuNav::UpDown(1, 3)),
    Button("SLOT FOUR", KEYSTATE_UNKNOWN, 25, 70, 220, 10, DO_LOAD, 4, MenuNav::UpDown(2, 4)),
    Button("SLOT FIVE", KEYSTATE_UNKNOWN, 25, 85, 220, 10, DO_LOAD, 5, MenuNav::UpDown(3, 5)),
    Button("SLOT SIX", KEYSTATE_UNKNOWN, 25, 100, 220, 10, DO_LOAD, 6, MenuNav::UpDown(4, 6)),
    Button("SLOT SEVEN", KEYSTATE_UNKNOWN, 25, 115, 220, 10, DO_LOAD, 7, MenuNav::UpDown(5, 7)),
    Button("SLOT EIGHT", KEYSTATE_UNKNOWN, 25, 130, 220, 10, DO_LOAD, 8, MenuNav::UpDown(6, 8)),
    Button("SLOT NINE", KEYSTATE_UNKNOWN, 25, 145, 220, 10, DO_LOAD, 9, MenuNav::UpDown(7, 9)),
    Button("SLOT TEN", KEYSTATE_UNKNOWN, 25, 160, 220, 10, DO_LOAD, 10, MenuNav::UpDown(8, 10)),
    Button("BACK", KEYSTATE_ESCAPE, 25, 175, 40, 20, RETURN_MENU, EXIT, MenuNav::UpDown(9, 0))
};

Button yes_or_no_buttons[] = {
    Button("YES", KEYSTATE_UNKNOWN, 70, 130, 50, 20, YES_OR_NO, YES, MenuNav::Right(1)),
    Button("NO", KEYSTATE_UNKNOWN, (320 - 50) - 70, 130, 50, 20, YES_OR_NO, NO, MenuNav::Left(0))
};

Button no_or_yes_buttons[] = {
    Button("NO", KEYSTATE_UNKNOWN, 70, 130, 50, 20, YES_OR_NO, NO, MenuNav::Right(1)),
    Button("YES", KEYSTATE_UNKNOWN, (320 - 50) - 70, 130, 50, 20, YES_OR_NO, YES, MenuNav::Left(0))
};

Button popup_dialog_buttons[] = {
    Button("OK", KEYSTATE_ESCAPE, 160 - 25, 130, 50, 20, YES_OR_NO, YES, MenuNav::None())
};

// Difficulty settings...in percent, so 100 == normal
std::string difficulty_names[DIFFICULTY_SETTINGS] = { "Skirmish", "Battle", "Slaughter" };

std::string archer_names[] = {
    "Robin",
    "Green Arrow",
    "Legolas",
    "Yeoman",
    "Strider",
    "Longshot",
    "Bowyer",
    "Hunter",
    "Archy"
};

std::string cleric_names[] = {
    "Tuck",
    "Brother",
    "Pater",
    "Drake",
    "Friar",
    "Francis",
    "John Paul",
    "Medic"
};

std::string druid_names[] = {
    "Roland",
    "Merlin",
    "Hippy",
    "Green Thumb",
    "Treefall",
    "Rain"
};

std::string elf_names[] = {
    "Legolas",
    "Took",
    "Elrond",
    "Tanis",
    "Acorn",
    "Lightfoot",
    "Treewee"
};

std::string mage_names[] = {
    "Gandalf",
    "Saruman",
    "Radagast",
    "Alatar",
    "Pallando",
    "Raistlin",
    "Fizban",
    "Mordenkainen",
    "Merlin",
    "Harry",
    "Manannan",
    "Mordack",
    "Jace"
};

std::string soldier_names[] = {
    "Lothar",
    "Arthur",
    "Uther",
    "Achilles",
    "Lu Bu",
    "Wallace",
    "Leonidas",
    "Attila",
    "Alexander",
    "Ajax",
    "Nestor",
    "Priam",
    "Hector",
    "Tom",
    "Bigfoot"
};

std::string thief_names[] = {
    "Shinobi",
    "Dismas",
    "Shadow",
    "Stabby",
    "Swiftstrike",
    "Scourge",
    "Rogue"
};

std::string orc_names[] = {
    "Grom",
    "Thrull",
    "Vernix",
    "Lanugo",
    "Grok",
    "Horde",
    "Grog",
    "Krosh"
};

std::string barbarian_names[] = {
    "Thor",
    "Conan",
    "Beowulf",
    "Cronus",
    "Pallas",
    "Atlas",
    "Prometheus",
    "Titan"
};

std::string elemental_names[] = {
    "Furnace",
    "Molten",
    "Burns",
    "Fire Eli",
    "Fireball",
    "Sunny",
    "Lava",
    "Heatwave",
    "Torch",
    "Scorch"
};

std::string skeleton_names[] = {
    "Drybones",
    "Blackbeard",
    "Boney",
    "Femur",
    "Patella",
    "Humerus",
    "Scapula"
};

std::string slime_names[] = {
    "Grimer",
    "Goop",
    "Slurp",
    "Glopp",
    "Sludge",
    "Blob"
};

std::string faerie_names[] = {
    "Tink",
    "Gem",
    "Glitter",
    "Jewel",
    "Blossom",
    "Ruby",
    "Muffin",
    "Flutter",
    "Sparkle",
    "Sprint",
    "Spirit",
    "Eve",
    "Twinkle",
    "Violet",
    "Daisy",
    "Lily"
};

std::string ghost_names[] = {
    "Casper",
    "Slimer",
    "Reaper",
    "Ecto",
    "Pepper",
    "Boo",
    "Banshee",
    "Nyx"
};

VirtualButton *allbuttons[MAX_BUTTONS];

void picker_main()
{
    Sint32 i;

    for (i = 0; i < MAX_BUTTONS; ++i) {
        allbuttons[i] = nullptr;
    }

    // Set backdrops to NULL
    for (i = 0; i < 5; ++i) {
        backdrops[i] = nullptr;
    }

    PixieData backpics[] = {
        PixieData(std::filesystem::path("mainul.pix")),
        PixieData(std::filesystem::path("mainur.pix")),
        PixieData(std::filesystem::path("mainll.pix")),
        PixieData(std::filesystem::path("mainlr.pix"))
    };

    backdrops[0] = new PixieN(backpics[0]);
    backdrops[0]->setxy(0, 0);
    backdrops[1] = new PixieN(backpics[1]);
    backdrops[1]->setxy(160, 0);
    backdrops[2] = new PixieN(backpics[2]);
    backdrops[2]->setxy(0, 100);
    backdrops[3] = new PixieN(backpics[3]);
    backdrops[3]->setxy(160, 100);

    myscreen->viewob[0]->resize(PREF_VIEW_FULL);

    myscreen->clearbuffer();

    PixieData main_title_logo_data = PixieData(std::filesystem::path("title.pix")); // Marbled gladiator title
    main_title_logo_pix = new PixieN(main_title_logo_data);

    PixieData main_columns_data = PixieData(std::filesystem::path("columns.pix"));
    main_columns_pix = new PixieN(main_columns_data);

    // Get the mouse, timer, and keyboard...
    grab_mouse();
    clear_keyboard();

    // Load the current saved game, if it exists ... (save0.gtl)
    SDL_RWops *loadgame = open_read_file(std::filesystem::path("save/save0.gtl"));

    if (loadgame) {
        SDL_RWclose(loadgame);
        myscreen->save_data.load("save0");
    }

    mainmenu(1);
}

void picker_quit()
{
    Sint32 i;

    for (i = 0; i < MAX_BUTTONS; ++i) {
        if (allbuttons[i]) {
            delete allbuttons[i];
        }
    }

    delete myscreen;
    delete main_columns_pix;
    delete main_title_logo_pix;
}

// Shows the current guy...
void picker_show_guy(Sint32 frames, Sint32 who, Sint16 centerx, Sint16 centery)
{
    Walker *mywalker;
    Sint32 i;
    Sint32 newfamily;

    if (current_guy == nullptr) {
        return;
    }

    frames = abs(frames);

    // Use current_type of guy
    if (who == 0) {
        newfamily = allowable_guys[current_type];
    } else {
        newfamily = myscreen->save_data.team_list[editguy]->family;
    }

    newfamily = current_guy->family;

    mywalker = create_walker(ORDER_LIVING, newfamily);
    mywalker->stats.bit_flags = 0;
    mywalker->curdir = ((frames / 192) + FACE_DOWN) % 8;
    mywalker->ani_type = ANI_WALK;

    for (i = 0; i < ((frames / 12) % 4); ++i) {
        mywalker->animate();
    }

    mywalker->team_num = current_guy->teamnum;

    mywalker->setxy(centerx - (mywalker->sizex / 2), centery - (mywalker->sizey / 2));
    myscreen->draw_button((centerx - 80) + 54, (centery - 45) + 26, (centerx - 80) + 106, (centery - 45) + 64, 1, 1);
    myscreen->draw_text_bar((centerx - 80) + 56, (centery - 45) + 28, (centerx - 80) + 104, (centery - 45) + 62);

    mywalker->draw(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                   myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                   myscreen->viewob[0]->endx, myscreen->viewob[0]->endy,
                   myscreen->viewob[0]->control);

    delete mywalker;
}

Sint32 leftmouse(Button *buttons)
{
    Sint32 i = 0;
    Sint32 somebutton = -1;

    grab_mouse();
    MouseState &mymouse = query_mouse();

    while (allbuttons[i]) {
        if ((buttons != nullptr) && !buttons[i].hidden) {
            allbuttons[i]->mouse_on();

            if (keystates[allbuttons[i]->hotkey]) {
                somebutton = i;
            }
        }

        ++i;
    }

    if (somebutton != -1) {
        // Simulate left-click
        return 1;
    }

    if (mymouse.left) {
        // Wait for release
        while (mymouse.left) {
            mymouse = query_mouse();
        }

        return 1;
    } else if (mymouse.right) {
        // Wait for release
        while (mymouse.right) {
            mymouse = query_mouse();
        }

        return 2;
    } else {
        return 0;
    }
}

void view_team(Sint16 left, Sint16 top, Sint16 right, Sint16 bottom)
{
    Uint8 text_down = top + 3;
    Sint32 i;
    std::stringstream buf;
    std::string message;
    Uint8 namecolor;
    Uint8 numguys = 0;
    Text &mytext = myscreen->text_normal;

    myscreen->redrawme = 1;
    myscreen->draw_button(left, top, right, bottom, 2, 1);

    buf << "  Name  ";
    message = buf.str();
    buf.clear();
    message.resize(30);
    mytext.write_xy(left + 5, text_down, message, BLACK, 1);

    buf << "STR  DEX  CON  INT  ARM";
    message = buf.str();
    buf.clear();
    message.resize(30);
    mytext.write_xy(left + 80, text_down, message, BLACK, 1);

    buf << "Level";
    message = buf.str();
    buf.clear();
    message.resize(30);
    mytext.write_xy(left + 230, text_down, message, BLACK, 1);

    text_down += 6;

    for (i = 0; i < myscreen->save_data.team_size; ++i) {
        Guy **ourteam = myscreen->save_data.team_list;

        if (ourteam[i]) {
            ++numguys;

            message = ourteam[i]->name;
            message.resize(30);

            // Pick a nice dark color based on family type
            namecolor = ((ourteam[i]->family + 1) << 4) & 255;
            mytext.write_xy(left + 5, text_down, message, namecolor, 1);

            std::ios_base::fmtflags flags(buf.flags());
            buf << std::setw(4) << ourteam[i]->strength
                << " " << ourteam[i]->dexterity
                << " " << ourteam[i]->constitution
                << " " << ourteam[i]->intelligence
                << " " << ourteam[i]->armor;

            message = buf.str();
            buf.clear();
            buf.flags(flags);
            message.resize(30);
            mytext.write_xy(left + 70, text_down, message, BLACK, 1);

            flags = buf.flags();
            buf << std::setw(2) << ourteam[i]->get_level();
            message = buf.str();
            buf.clear();
            buf.flags(flags);
            message.resize(30);
            mytext.write_xy(left + 235, text_down, message, BLACK, 1);

            message = ourteam[i]->family;
            message.resize(30);
            mytext.write_xy(left + 260, text_down, message, namecolor, 1);

            text_down += 6;
        }
    }

    if (numguys == 0) {
        message = "*** YOU HAVE NO TEAM! ***";
        mytext.write_xy(left + 80, 60, message, ORANGE_START, 1);
    }

    return;
}

void draw_version_number()
{
    Text &mytext = myscreen->text_normal;

    myscreen->redrawme = 1;
    Sint32 w = strlen(OPENGLAD_VERSION_STRING) * 6;
    Sint32 h = 8;
    Sint32 x = (320 - w) - 80;
    Sint32 y = 200 - 12;

    myscreen->fastbox(x, y, w, h, PURE_BLACK);
    mytext.write_xy(x, y, OPENGLAD_VERSION_STRING, DARK_BLUE, 1);
}

void draw_highlight_interior(Button const &b)
{
    if (!menu_nav_enabled) {
        return;
    }

    float t = (1.0f + sinf(SDL_GetTicks() / 300.0f)) / 2.0f;
    float size = 3;

    myscreen->draw_box(b.x + (t * size), b.y + (t * size), (b.x + b.sizex) - (t * size), (b.y + b.sizey) - (t * size), YELLOW, 0);
}

void draw_highlight(Button const &b)
{
    if (!menu_nav_enabled) {
        return;
    }

    float t = (1.0f + sinf(SDL_GetTicks() / 300.f)) / 2.0f;
    float size = 3;

    myscreen->draw_box(b.x - (t * size), b.y - (t * size), (b.x + b.sizex) + (t * size), (b.y + b.sizey) + (t * size), YELLOW, 0);
}

bool handle_menu_nav(Button *buttons, Sint32 &highlighted_button, Sint32 &retvalue, bool use_global_vbuttons)
{
    Sint32 next_button = -1;
    bool pressed = false;
    bool activated = false;

    if (isPlayerHoldingKey(0, KEY_UP)) {
        while (isPlayerHoldingKey(0, KEY_UP)) {
            get_input_events(POLL);
        }

        next_button = buttons[highlighted_button].nav.up;

        pressed = true;
    }

    if (isPlayerHoldingKey(0, KEY_DOWN)) {
        while (isPlayerHoldingKey(0, KEY_DOWN)) {
            get_input_events(POLL);
        }

        next_button = buttons[highlighted_button].nav.down;

        pressed = true;
    }

    if (isPlayerHoldingKey(0, KEY_LEFT)) {
        while (isPlayerHoldingKey(0, KEY_LEFT)) {
            get_input_events(POLL);
        }

        next_button = buttons[highlighted_button].nav.left;

        pressed = true;
    }

    if (isPlayerHoldingKey(0, KEY_RIGHT)) {
        while (isPlayerHoldingKey(0, KEY_RIGHT)) {
            get_input_events(POLL);
        }

        next_button = buttons[highlighted_button].nav.right;

        pressed = true;
    }

    if (isPlayerHoldingKey(0, KEY_FIRE)) {
        while (isPlayerHoldingKey(0, KEY_FIRE)) {
            get_input_events(POLL);
        }

        if (!menu_nav_enabled) {
            pressed = true;
        } else {
            myscreen->soundp->play_sound(SOUND_BOW);

            if (use_global_vbuttons) {
                allbuttons[highlighted_button]->vdisplay(1);
                allbuttons[highlighted_button]->vdisplay();

                if (allbuttons[highlighted_button]->myfunc) {
                    retvalue = allbuttons[highlighted_button]->do_call(allbuttons[highlighted_button]->myfunc, allbuttons[highlighted_button]->arg);
                }
            } else {
                retvalue = OK;
            }

            pressed = true;
            activated = true;
        }
    }

    if ((next_button >= 0) && !buttons[next_button].hidden) {
        highlighted_button = next_button;
    }

    // Turn menu_nav on if something was pressed
    if (pressed) {
        menu_nav_enabled = true;
        menu_nav_enabled_time = SDL_GetTicks();
    } else if (menu_nav_enabled) {
        // Turn it off if its been a while since something was pressed
        if ((SDL_GetTicks() - menu_nav_enabled_time) > 5000) {
            menu_nav_enabled = MENU_NAV_DEFAULT;
        }
    }

    return activated;
}

bool reset_buttons(VirtualButton *localbuttons, Button *buttons, Sint32 num_buttons, Sint32 &retvalue)
{
    if (localbuttons && ((retvalue == OK) || (retvalue == REDRAW))) {
        delete localbuttons;
        localbuttons = init_buttons(buttons, num_buttons);

        retvalue = 0;

        return true;
    }

    return false;
}

void redraw_mainmenu()
{
    Sint32 count = 0;
    std::stringstream buf;
    std::string message;

    main_title_logo_pix->set_frame(0);
    main_title_logo_pix->setxy(15, 8);
    main_title_logo_pix->drawMix(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                                 myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                                 myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    main_title_logo_pix->set_frame(1);
    main_title_logo_pix->setxy(151, 8);
    main_title_logo_pix->drawMix(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                                 myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                                 myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    main_columns_pix->set_frame(0);
    main_columns_pix->setxy(12, 40);
    main_columns_pix->drawMix(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                              myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                              myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    main_columns_pix->set_frame(1);
    main_columns_pix->setxy(242, 40);
    main_columns_pix->drawMix(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                              myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                              myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);

#ifndef DISABLE_MULTIPLAYER
    if (myscreen->save_data.numplayers == 4) {
        allbuttons[2]->do_outline = 1;
        allbuttons[3]->do_outline = 0;
        allbuttons[4]->do_outline = 0;
        allbuttons[5]->do_outline = 0;
        allbuttons[2]->vdisplay();
        allbuttons[3]->vdisplay();
        allbuttons[4]->vdisplay();
        allbuttons[5]->vdisplay();
    } else if (myscreen->save_data.numplayers == 3) {
        allbuttons[2]->do_outline = 0;
        allbuttons[3]->do_outline = 1;
        allbuttons[4]->do_outline = 0;
        allbuttons[5]->do_outline = 0;
        allbuttons[2]->vdisplay();
        allbuttons[3]->vdisplay();
        allbuttons[4]->vdisplay();
        allbuttons[5]->vdisplay();
    } else if (myscreen->save_data.numplayers == 2) {
        allbuttons[2]->do_outline = 0;
        allbuttons[3]->do_outline = 0;
        allbuttons[4]->do_outline = 1;
        allbuttons[5]->do_outline = 0;
        allbuttons[2]->vdisplay();
        allbuttons[3]->vdisplay();
        allbuttons[4]->vdisplay();
        allbuttons[5]->vdisplay();
    } else {
        allbuttons[2]->do_outline = 0;
        allbuttons[3]->do_outline = 0;
        allbuttons[4]->do_outline = 0;
        allbuttons[5]->do_outline = 1;
        allbuttons[2]->vdisplay();
        allbuttons[3]->vdisplay();
        allbuttons[4]->vdisplay();
        allbuttons[5]->vdisplay();
    }

    buf << "Difficulty: " << difficulty_names[current_difficulty];
    message = buf.str();
    buf.clear();
    message.resize(80);
    allbuttons[6]->label = message;

    // Show the allied mode
    if (myscreen->save_data.allied_mode) {
        message = "PvP: Ally";
    } else {
        message = "PvP: Enemy";
    }

    allbuttons[7]->label = message;
#else

    buf << "Difficulty: " << difficulty_names[current_difficulty];
    message = buf.str();
    buf.clear();
    message.resize(80);
    allbuttons[2]->label = message;

#endif

    count = 0;
    while (allbuttons[count]) {
        allbuttons[count]->vdisplay();
        ++count;
    }

    allbuttons[0]->set_graphic(FAMILY_NORMAL1);
    allbuttons[OPTIONS_BUTTON_INDEX]->set_graphic(FAMILY_WRENCH);

    draw_version_number();
}

Sint32 mainmenu(Sint32 arg1)
{
    Sint32 retvalue = 0;

    if (arg1) {
        arg1 = 1;
    }

    if (localbuttons != nullptr) {
        // We'll make a new set
        delete localbuttons;
    }

    Button *buttons = mainmenu_buttons;
    Sint32 num_buttons = ARRAY_SIZE(mainmenu_buttons);
    Sint32 highlighted_button = 1;
    localbuttons = init_buttons(buttons, num_buttons);

    allbuttons[0]->set_graphic(FAMILY_NORMAL1);
    allbuttons[OPTIONS_BUTTON_INDEX]->set_graphic(FAMILY_WRENCH);

    redraw_mainmenu();

    clear_keyboard();
    reset_timer();

    while (query_timer() < 1) {
    }

    myscreen->fadeblack(1);

    grab_mouse();

    while (!(retvalue & EXIT)) {
        // Input
        if (leftmouse(buttons)) {
            retvalue = localbuttons->leftclick();
        }

        handle_menu_nav(buttons, highlighted_button, retvalue);

        // Reset buttons
        if (reset_buttons(localbuttons, buttons, num_buttons, retvalue)) {
            allbuttons[0]->set_graphic(FAMILY_NORMAL1);
            allbuttons[OPTIONS_BUTTON_INDEX]->set_graphic(FAMILY_WRENCH);
        }

        // Draw
        myscreen->clearbuffer();
        draw_buttons(buttons, num_buttons);
        redraw_mainmenu();
        draw_highlight(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);
    }

    return retvalue;
}

// Reset game data and go to create_team_menu()
Sint32 beginmenu(Sint32 arg1)
{
    // Do we have a team already? Then prompt to reset.
    if (myscreen->save_data.team_size > 0) {
        if (!yes_or_no_prompt("NEW GAME", "There is already a game loaded.\nDo you want to restart?", false)) {
            return REDRAW;
        }
    }

    myscreen->clear();

    // Starting new game..
    release_mouse();
    myscreen->clearbuffer();
    myscreen->swap();
    read_campaign_intro(myscreen);
    myscreen->refresh();
    grab_mouse();
    myscreen->clear();

    // Reset the save data so we have a fresh, new team
    myscreen->save_data.reset();
    current_guy = nullptr;

    // Clear the labeling counter
    for (Sint32 i = 0; i < NUM_FAMILIES; ++i) {
        numbought[i] = 0;
    }

    return create_team_menu(1);
}

std::string get_family_string(Sint16 family)
{
    switch (family) {
    case FAMILY_ARCHER:

        return "ARCHER";
    case FAMILY_CLERIC:

        return "CLERIC";
    case FAMILY_DRUID:

        return "DRUID";
    case FAMILY_ELF:

        return "ELF";
    case FAMILY_MAGE:

        return "MAGE";
    case FAMILY_SOLDIER:

        return "SOLDIER";
    case FAMILY_THIEF:

        return "THIEF";
    case FAMILY_ARCHMAGE:

        return "ARCHMAGE";
    case FAMILY_ORC:

        return "ORC";
    case FAMILY_BIG_ORC:

        return "ORC_CAPTAIN";
    case FAMILY_BARBARIAN:

        return "BARBARIAN";
    case FAMILY_FIRE_ELEMENTAL:

        return "ELEMENTAL";
    case FAMILY_SKELETON:

        return "SKELETON";
    case FAMILY_SLIME:
    case FAMILY_MEDIUM_SLIME:
    case FAMILY_SMALL_SLIME:

        return "SLIME";
    case FAMILY_FAERIE:

        return "FAERIE";
    case FAMILY_GHOST:

        return "GHOST";
    default:

        return "BEAST";
    }
}

void family_name_copy(std::string *name, Sint16 family)
{
    switch (family) {
    case FAMILY_ARCHER:
        name->assign("ARCHER");

        break;
    case FAMILY_CLERIC:
        name->assign("CLERIC");

        break;
    case FAMILY_DRUID:
        name->assign("DRUID");

        break;
    case FAMILY_ELF:
        name->assign("ELF");

        break;
    case FAMILY_MAGE:
        name->assign("MAGE");

        break;
    case FAMILY_SOLDIER:
        name->assign("SOLDIER");

        break;
    case FAMILY_THIEF:
        name->assign("THIEF");

        break;
    case FAMILY_ARCHMAGE:
        name->assign("ARCHMAGE");

        break;
    case FAMILY_ORC:
        name->assign("ORC");

        break;
    case FAMILY_BIG_ORC:
        name->assign("ORC CAP.");

        break;
    case FAMILY_BARBARIAN:
        name->assign("BARBAR.");

        break;
    default:
        name->assign("BEAST");
    }
}

Sint32 create_team_menu(Sint32 arg1)
{
    Sint32 retvalue = 0;

    if (arg1 == 1) {
        // Go straight to the hiring screen if we just started a new game.
        retvalue = create_hire_menu(arg1);
    }

    if (localbuttons) {
        delete localbuttons;
    }

    myscreen->fadeblack(0);

    Text &mytext = myscreen->text_normal;
    Button *buttons = createmenu_buttons;
    Sint32 num_buttons = 9;
    Sint32 highlighted_button = 1;
    localbuttons = init_buttons(buttons, num_buttons);
    draw_backdrop();
    draw_buttons(buttons, num_buttons);

    Sint32 last_level_id = -1;

    myscreen->fadeblack(1);
    std::stringstream buf;

    while (!(retvalue & EXIT)) {
        // Input
        if (leftmouse(buttons)) {
            retvalue = localbuttons->leftclick();
        }

        handle_menu_nav(buttons, highlighted_button, retvalue);

        // Reset buttons
        bool buttons_were_reset = reset_buttons(localbuttons, buttons, num_buttons, retvalue);

        if ((last_level_id != myscreen->save_data.scen_num) || buttons_were_reset) {
            retvalue = 0;
            last_level_id = myscreen->save_data.scen_num;
            myscreen->level_data.id = last_level_id;
            myscreen->level_data.load();
        }

        // Draw
        myscreen->clearbuffer();
        draw_backdrop();
        draw_buttons(buttons, num_buttons);

        // Level name
        Sint32 len = myscreen->level_data.title.size();
        myscreen->draw_rect_filled(((buttons[7].x + buttons[7].sizex) - (6 * len)) - 2, (buttons[7].y - 8) - 1, (6 * len) + 4, 8, PURE_BLACK, 150);
        buf << myscreen->level_data.title;
        mytext.write_xy((buttons[7].x + buttons[7].sizex) - (6 * len), buttons[7].y - 8, WHITE, buf);
        buf.clear();

        // Campaign name
        len = myscreen->save_data.current_campaign.string().size();

        myscreen->draw_rect_filled(((buttons[8].x + buttons[8].sizex) - (6 * len)) - 2, (buttons[8].y - 8) - 1, (6 * len) + 4, 8, PURE_BLACK, 150);
        buf << myscreen->save_data.current_campaign;
        mytext.write_xy((buttons[8].x + buttons[8].sizex) - (6 * len), buttons[8].y - 8, WHITE, buf);
        buf.clear();

        draw_highlight(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);
    }

    return REDRAW;
}

Sint32 create_view_menu(Sint32 arg1)
{
    Sint32 retvalue = 0;

    if (arg1) {
        arg1 = 1;
    }

    myscreen->clearbuffer();

    if (localbuttons) {
        delete localbuttons;
    }

    Button *buttons = viewteam_buttons;
    Sint32 num_buttons = 2;
    Sint32 highlighted_button = 1;
    localbuttons = init_buttons(buttons, num_buttons);

    while (!(retvalue & EXIT)) {
        // Input
        if (leftmouse(buttons)) {
            retvalue = localbuttons->leftclick();
        }

        handle_menu_nav(buttons, highlighted_button, retvalue);

        // Reset buttons
        reset_buttons(localbuttons, buttons, num_buttons, retvalue);

        // Draw
        myscreen->clearbuffer();
        draw_backdrop();
        draw_buttons(buttons, num_buttons);
        view_team(5, 5, 314, 160);
        draw_highlight(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);
    }

    myscreen->clearbuffer();

    return REDRAW;
}

std::string get_class_description(Uint8 family)
{
    std::stringstream buffer;

    switch (family) {
    case FAMILY_SOLDIER:
        buffer << "Your basic grunt, can     " << std::endl
               << "absorb and deal damage and" << std::endl
               << "move moderately fast. A   " << std::endl
               << "good all-around fighter. A" << std::endl
               << "soldier's normal weapon is" << std::endl
               << "a magical returning blade." << std::endl
               << std::endl
               << "Special: Charge";

            break;
    case FAMILY_ELF:
        buffer << "Elves are small and weak, " << std::endl
               << "but are harder to hit than" << std::endl
               << "most classes. Alone of all" << std::endl
               << "the classes, elves possess" << std::endl
               << "the 'ForestWalk' ability. " << std::endl
               << std::endl
               << "Special: Rocks";

        break;
    case FAMILY_ARCHER:
        buffer << "Archers are fleet of foot," << std::endl
               << "and their arrows have a   " << std::endl
               << "long range. Although      " << std::endl
               << "they're not as strong as  " << std::endl
               << "other fighters, they can  " << std::endl
               << "be a good squad backbone. " << std::endl
               << std::endl
               << "Special: Fire Arrows";

        break;
    case FAMILY_MAGE:
        buffer << "Mages are slow, can't     " << std::endl
               << "stand much damage, and are" << std::endl
               << "horrible at hand-to-hand  " << std::endl
               << "combat, but their magical " << std::endl
               << "fireballs pack a big      " << std::endl
               << "punch.                    " << std::endl
               << std::endl
               << "Special: Teleport";

        break;
    case FAMILY_SKELETON:
        buffer << "Skeletons are the pathetic" << std::endl
               << "remains of those who once " << std::endl
               << "were among the living.    " << std::endl
               << "They are not particularly " << std::endl
               << "dangerous, but they move  " << std::endl
               << "with blinding speed.      " << std::endl
               << std::endl
               << "Special: Tunnel";

        break;
    case FAMILY_CLERIC:
        buffer << "Clerics, like mages, are  " << std::endl
               << "slow, but have a stronger " << std::endl
               << "hand-to-hand attack.      " << std::endl
               << "Clerics possess abilities " << std::endl
               << "related to healing and    " << std::endl
               << "interaction with the dead." << std::endl
               << std::endl
               << "Special: Heal";

        break;
    case FAMILY_FIRE_ELEMENTAL:
        buffer << "Strong and quick, fire    " << std::endl
               << "elementals can expel      " << std::endl
               << "flaming meteors in all    " << std::endl
               << "directions to decimate    " << std::endl
               << "enemies.                  " << std::endl
               << std::endl
               << "Special: Starburst";

        break;
    case FAMILY_FAERIE:
        buffer << "The faerie are small,     " << std::endl
               << "flying above friends and  " << std::endl
               << "enemies alike unnoticed.  " << std::endl
               << "Although they are delicate" << std::endl
               << "and easily destroyed,     " << std::endl
               << "faeries can sprinkle a    " << std::endl
               << "magic powder which freezes" << std::endl
               << "their enemies.            ";

        break;
    case FAMILY_SLIME:
    case FAMILY_SMALL_SLIME:
    case FAMILY_MEDIUM_SLIME:
        buffer << "Slimes are patches of ooze" << std::endl
               << "which grow and split into " << std::endl
               << "two smaller slimes, over- " << std::endl
               << "whelming the enemy. Their " << std::endl
               << "nebulous nature makes them" << std::endl
               << "more susceptible to magic." << std::endl
               << std::endl
               << "Special: Grow";

        break;
    case FAMILY_THIEF:
        buffer << "Thieves are fast, though  " << std::endl
               << "not so potent as the      " << std::endl
               << "soldier. Theives can throw" << std::endl
               << "small blades rapidly and  " << std::endl
               << "damage whole groups of    " << std::endl
               << "enemies with their bombs. " << std::endl
               << std::endl
               << "Special: Drop Bomb";

        break;
    case FAMILY_GHOST:
        buffer << "Ghosts can pass through   " << std::endl
               << "walls, trees, and anything" << std::endl
               << "else that gets in the way." << std::endl
               << "Their chilling touch can  " << std::endl
               << "bring death quickly at    " << std::endl
               << "close range.              " << std::endl
               << std::endl
               << "Special: Scare";

        break;
    case FAMILY_DRUID:
        buffer << "Druids are the magicians  " << std::endl
               << "of nature, and have power " << std::endl
               << "over natural events. They " << std::endl
               << "throw lightning bolts at  " << std::endl
               << "their foes; the fast bolts" << std::endl
               << "have long range.          " << std::endl
               << std::endl
               << "Special: Plant Tree";

        break;
    case FAMILY_ORC:
        buffer << "Orcs are a basic 'grunt'; " << std::endl
               << "strong and hard to hurt,  " << std::endl
               << "they don't do much more   " << std::endl
               << "than inflict pain. Orcs   " << std::endl
               << "can't attack at range.    " << std::endl
               << std::endl
               << "Special: Howl";

        break;
    case FAMILY_BIG_ORC:
        buffer << "Orc captains are stronger " << std::endl
               << "and smarter than the basic" << std::endl
               << "orc. They throw blades    " << std::endl
               << "across the battlefield to " << std::endl
               << "deal damage from afar.    ";

        break;
    case FAMILY_BARBARIAN:
        buffer << "Barbarians are powerful   " << std::endl
               << "and resist some magic     " << std::endl
               << "damage, but have more will" << std::endl
               << "than skill. They are      " << std::endl
               << "tough, tending to bash    " << std::endl
               << "their way through trouble " << std::endl
               << "with heavy iron hammers.  " << std::endl
               << std::endl
               << "Special: Hurl Boulder";

        break;
    case FAMILY_ARCHMAGE:
        buffer << "An Archmage takes the     " << std::endl
               << "learnings of the Magi one " << std::endl
               << "step further, possessing  " << std::endl
               << "extraordinary firepower at" << std::endl
               << "the expense of physical   " << std::endl
               << "weakness.                 " << std::endl
               << std::endl
               << "Special: Teleport";

        break;
    default:

        break;
    }

    return buffer.str();
}

// State: str 0, dex 1, con 2, int 3, armor 4
std::string get_training_cost_rating(Uint8 family, Sint32 stat)
{
    Sint32 value = 55 / statcosts[family][stat];
    Sint32 rating = (value / 11) * 5;

    switch (rating) {
    case 0:

        return "";
    case 1:

        return "*";
    case 2:

        return "**";
    case 3:

        return "***";
    case 4:

        return "****";
    case 5:

        return "*****";
    default:

        return "";
    }
}

Sint32 create_hire_menu(Sint32 arg1)
{

    Sint32 linesdown;
    Sint32 retvalue = 0;
    Sint32 start_time = query_timer();
    // Normally STAT_COLOR or STAT_CHANGED
    Uint8 showcolor;
    Uint32 current_cost;
    Sint32 clickvalue;

    SDL_Rect stat_box = { 196, (50 - 6) - 32, 104, 82 + 32 };
    SDL_Rect stat_box_inner = {
        stat_box.x + 4,
        (stat_box.y + 4) + 6,
        stat_box.w - 8,
        (stat_box.h - 8) - 6
    };

    SDL_Rect stat_box_content = {
        stat_box_inner.x + 4,
        stat_box_inner.y + 4,
        stat_box_inner.w - 8,
        stat_box_inner.h - 8
    };

    SDL_Rect cost_box = { 196, 130, 104, 31 };
    SDL_Rect cost_box_inner = {
        cost_box.x + 4,
        cost_box.y + 4,
        cost_box.w - 8,
        cost_box.h - 8
    };

    SDL_Rect cost_box_content = {
        cost_box_inner.x + 4,
        cost_box_inner.y + 4,
        cost_box_inner.w - 8,
        cost_box_inner.h - 8
    };

    SDL_Rect description_box = { 11, 71, 180, 90 };
    SDL_Rect description_box_inner = {
        description_box.x + 4,
        description_box.y + 4,
        description_box.w - 8,
        description_box.h - 8
    };

    SDL_Rect description_box_content = {
        description_box_inner.x + 4,
        description_box_inner.y + 4,
        description_box_inner.w - 8,
        description_box_inner.h - 8
    };

    SDL_Rect name_box = {
        (description_box.x + (description_box.w / 2)) - ((126 - 34) / 2),
        (description_box.y - 71) + 8,
        126 - 34,
        24 - 8
    };

    SDL_Rect name_box_inner = {
        name_box.x + 2,
        name_box.y + 2,
        name_box.w - 4,
        name_box.h - 4
    };

    hiremenu_buttons[0].x = (((description_box.x + (description_box.w / 2)) - hiremenu_buttons[0].sizex) - 4) - 30;
    hiremenu_buttons[0].y = ((name_box.y + name_box.h) + ((description_box.y - (name_box.y + name_box.h)) / 2)) - (hiremenu_buttons[0].sizey / 2);
    hiremenu_buttons[1].x = ((description_box.x + (description_box.w / 2)) + 4) + 30;
    hiremenu_buttons[1].y = ((name_box.y + name_box.h) + ((description_box.y - (name_box.y + name_box.h)) / 2)) - (hiremenu_buttons[1].sizey / 2);
    hiremenu_buttons[2].hidden = (myscreen->save_data.numplayers == 1);

    myscreen->clearbuffer();

    if (localbuttons) {
        delete localbuttons;
    }

    hiremenu_buttons[2].hidden = true;

    Button *buttons = hiremenu_buttons;
    Sint32 num_buttons = 5;
    Sint32 highlighted_button = 1;
    localbuttons = init_buttons(buttons, num_buttons);

    cycle_guy(0);
    change_hire_teamnum(0);

    Uint8 last_family = current_guy->family;
    std::string description(get_class_description(last_family));
    std::list<std::string> desc = explode(description);
    std::string family_name(get_family_string(last_family));

    grab_mouse();

    while (!(retvalue & EXIT)) {
        // Input
        clickvalue = leftmouse(buttons);

        if (clickvalue == 1) {
            retvalue = localbuttons->leftclick();
        } else if (clickvalue == 2) {
            retvalue = localbuttons->rightclick();
        }

        handle_menu_nav(buttons, highlighted_button, retvalue);

        // Reset buttons
        if ((retvalue == OK) || (retvalue == REDRAW)) {
            if (localbuttons) {
                delete localbuttons;
            }

            localbuttons = init_buttons(buttons, num_buttons);

            // Update our team-number display...
            change_hire_teamnum(0);
            retvalue = 0;
        }

        // Draw
        myscreen->clearbuffer();

        draw_backdrop();
        draw_buttons(buttons, num_buttons);

        if (current_guy == nullptr) {
            cycle_guy(0);
        }

        // Name box
        myscreen->draw_button(name_box, 1);
        myscreen->draw_button_inverted(name_box_inner);

        Text &mytext = myscreen->text_normal;
        mytext.write_xy((name_box.x + (name_box.w / 2)) - (3 * family_name.size()), name_box.y + 6, family_name, DARK_BLUE, 1);

        // 0 means current_guy
        picker_show_guy(query_timer() - start_time, 0, description_box.x + (description_box.w / 2), (name_box.y + name_box.h) + (description_box.y - (name_box.y + name_box.h)) / 2);

        change_hire_teamnum(0);

        // Description box
        myscreen->draw_button(description_box, 1);
        myscreen->draw_button_inverted(description_box_inner);

        if (current_guy->family != last_family) {
            // Update description
            last_family = current_guy->family;
            description = get_class_description(last_family);
            desc = explode(description);
            family_name = get_family_string(last_family);
        }

        Sint32 i = 0;
        std::stringstream buf;
        std::streamsize orig_precision = buf.precision();
        for (auto const &e : desc) {
            buf << e;
            mytext.write_xy(description_box_content.x, description_box_content.y + (i * 10), DARK_BLUE, buf);
            buf.clear();
            ++i;
        }

        // Cost box
        myscreen->draw_button(cost_box, 1);
        myscreen->draw_button_inverted(cost_box_inner);

        buf << "CASH: " << myscreen->save_data.m_totalcash[current_team_num];
        mytext.write_xy(cost_box_content.x, cost_box_content.y, buf.str(), DARK_BLUE, 1);
        buf.clear();

        current_cost = calculate_hire_cost();
        mytext.write_xy(cost_box_content.x, cost_box_content.y + 10, "COST: ", DARK_BLUE, 1);
        buf << "      " << current_cost;

        if (current_cost > myscreen->save_data.m_totalcash[current_team_num]) {
            mytext.write_xy(cost_box_content.x + 10, cost_box_content.y + 10,
                            buf.str(), STAT_CHANGED, 1);
        } else {
            mytext.write_xy(cost_box_content.x + 10, cost_box_content.y + 10,
                            buf.str(), STAT_COLOR, 1);
        }

        buf.clear();

        // Stat box
        myscreen->draw_button(stat_box, 1);
        buf << "Train";
        mytext.write_xy(stat_box.x + 65, stat_box.y + 2, DARK_BLUE, buf);
        buf.clear();
        myscreen->draw_button_inverted(stat_box_inner);

        // Stat box content
        linesdown = 0;
        Sint32 line_height = 10;
        showcolor = STAT_COLOR;

        // Strength
        mytext.write_xy(stat_box_content.x,
                        stat_box_content.y + (linesdown * line_height),
                        "STR:", STAT_COLOR, 1);
        buf << current_guy->strength;
        mytext.write_xy(stat_box_content.x + STAT_NUM_OFFSET,
                        stat_box_content.y + (linesdown * line_height),
                        buf.str(), showcolor, 1);
        buf.clear();
        mytext.write_xy((stat_box_content.x + STAT_NUM_OFFSET) + 18,
                        stat_box_content.y + (linesdown * line_height),
                        get_training_cost_rating(last_family, 0), showcolor, 1);

        ++linesdown;

        // Dexterity
        mytext.write_xy(stat_box_content.x,
                        stat_box_content.y + (linesdown * line_height),
                        "DEX:", STAT_COLOR, 1);
        buf << current_guy->dexterity;
        mytext.write_xy(stat_box_content.x + STAT_NUM_OFFSET,
                        stat_box_content.y + (linesdown * line_height),
                        buf.str(), showcolor, 1);
        buf.clear();

        mytext.write_xy((stat_box_content.x + STAT_NUM_OFFSET) + 18,
                        stat_box_content.y + (linesdown * line_height),
                        get_training_cost_rating(last_family, 1), showcolor, 1);

        ++linesdown;

        // Constitution
        mytext.write_xy(stat_box_content.x,
                        stat_box_content.y + (linesdown * line_height),
                        "CON:", STAT_COLOR, 1);
        buf << current_guy->constitution;
        mytext.write_xy(stat_box_content.x + STAT_NUM_OFFSET,
                        stat_box_content.y + (linesdown * line_height),
                        buf.str(), showcolor, 1);
        buf.clear();
        mytext.write_xy((stat_box_content.x + STAT_NUM_OFFSET) + 18,
                        stat_box_content.y + (linesdown * line_height),
                        get_training_cost_rating(last_family, 2), showcolor, 1);

        ++linesdown;

        // Intelligence
        mytext.write_xy(stat_box_content.x,
                        stat_box_content.y + (linesdown * line_height),
                        "INT:", STAT_COLOR, 1);
        buf << current_guy->intelligence;
        mytext.write_xy(stat_box_content.x + STAT_NUM_OFFSET,
                        stat_box_content.y + (linesdown * line_height),
                        buf.str(), showcolor, 1);
        buf.clear();
        mytext.write_xy((stat_box_content.x + STAT_NUM_OFFSET) + 18,
                        stat_box_content.y + (linesdown * line_height),
                        get_training_cost_rating(last_family, 3), showcolor, 1);

        ++linesdown;

        // Armor
        mytext.write_xy(stat_box_content.x,
                        stat_box_content.y + (linesdown + line_height),
                        "ARMOR:", STAT_COLOR, 1);
        buf << current_guy->armor;
        mytext.write_xy(stat_box_content.x + STAT_NUM_OFFSET,
                        stat_box_content.y + (linesdown * line_height),
                        buf.str(), showcolor, 1);
        buf.clear();

        // Separator bar
        SDL_Rect r = {
            stat_box_content.x + 10,
            (stat_box_content.y + ((linesdown + 1) * line_height)) - 2,
            stat_box_content.w - 20,
            2
        };

        myscreen->draw_button_inverted(r);

        Sint32 derived_offset = (3 * STAT_NUM_OFFSET) / 4;
        ++linesdown;

        float val;

        mytext.write_xy(stat_box_content.x,
                        (stat_box_content.y + (linesdown * line_height)) + 4,
                        "HP:", STAT_DERIVED, 1);

        val = ceilf(myscreen->level_data.myloader.hitpoints[PIX(ORDER_LIVING, last_family)]
                    + current_guy->get_hp_bonus());

        if (val != 0) {
            buf << val;
        }

        mytext.write_xy((stat_box_content.x + derived_offset) - 9,
                        (stat_box_content.y + (linesdown * line_height)) + 4,
                        HIGH_HP_COLOR, buf);
        buf.clear();

        mytext.write_xy((stat_box_content.x + derived_offset) + 18,
                        (stat_box_content.y + (linesdown * line_height)) + 4,
                        "MP:", STAT_DERIVED, 1);

        val = ceilf(current_guy->get_mp_bonus());
        if (val != 0) {
            buf << val;
        }
        mytext.write_xy(((stat_box_content.x + (2 * derived_offset)) + 18) - 9,
                        (stat_box_content.y + (linesdown * line_height)) + 4,
                        MAX_MP_COLOR, buf);
        buf.clear();

        ++linesdown;

        mytext.write_xy(stat_box_content.x,
                        (stat_box_content.y + (linesdown * line_height)) + 4,
                        "ATK:", STAT_DERIVED, 1);

        val = myscreen->level_data.myloader.damage[PIX(ORDER_LIVING, last_family)]
            + current_guy->get_damage_bonus();
        if (val != 0) {
            buf << val;
        }
        mytext.write_xy((stat_box_content.x + derived_offset) - 3,
                        (stat_box_content.y + (linesdown * line_height)) + 4,
                        showcolor, buf);
        buf.clear();

        mytext.write_xy((stat_box_content.x + derived_offset) + 18,
                        (stat_box_content.y + (linesdown * line_height)) + 4,
                        "DEF:", STAT_DERIVED, 1);

        val = current_guy->get_armor_bonus();
        if (val != 0) {
            buf << val;
        }

        mytext.write_xy(((stat_box_content.x + (2 * derived_offset)) + 18) - 3,
                        (stat_box_content.y + linesdown * line_height) + 4,
                        showcolor, buf);
        buf.clear();

        ++linesdown;

        mytext.write_xy(stat_box_content.x,
                        (stat_box_content.y + (linesdown * line_height)) + 4,
                        "SPD:", STAT_DERIVED, 1);

        val = myscreen->level_data.myloader.stepsizes[PIX(ORDER_LIVING, last_family)]
            + current_guy->get_speed_bonus();
        buf << std::setprecision(1) << val;

        mytext.write_xy(stat_box_content.x + derived_offset,
                        (stat_box_content.y + (linesdown * line_height)) + 4,
                        showcolor, buf);
        buf.clear();
        buf << std::setprecision(orig_precision);

        ++linesdown;

        mytext.write_xy(stat_box_content.x,
                        (stat_box_content.y + (linesdown * line_height)) + 4,
                        "ATK SPD:", STAT_DERIVED, 1);

        // The 10.0f / fire_frequency is somewhat arbitrary, but it makes for
        // good comparison info.
        float fire_freq =
            std::max(myscreen->level_data.myloader.fire_frequency[PIX(ORDER_LIVING, last_family)]
                     - current_guy->get_fire_frequency_bonus(),
                     1.0f);

        val = 10.0f / fire_freq;
        buf << std::setprecision(1) << val;
        mytext.write_xy((stat_box_content.x + derived_offset) + 21,
                        (stat_box_content.y + (linesdown * line_height)) + 4,
                        showcolor, buf);
        buf.clear();
        buf << std::setprecision(orig_precision);

        draw_highlight(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);

        if (arg1 == 1) {
            // Show popup on new game
            arg1 = -1;
            popup_dialog("HIRE TROOPS", "Get your team started here\nby hiring some fresh new recruits.");

            if (localbuttons) {
                delete localbuttons;
            }

            localbuttons = init_buttons(buttons, num_buttons);
        }
    }

    myscreen->clearbuffer();

    return REDRAW;
}

Sint32 create_train_menu(Sint32 arg1)
{
    Sint32 linesdown = 0;
    Sint32 i;
    Sint32 retvalue = 0;
    Uint8 showcolor;
    Sint32 start_time = query_timer();
    Uint32 current_cost;
    Sint32 clickvalue;

    SDL_Rect stat_box = { 38, 66, 82, 94 };
    SDL_Rect stat_box_inner = {
        stat_box.x + 4,
        stat_box.y + 4,
        stat_box.w - 8,
        stat_box.h - 8
    };

    SDL_Rect stat_box_content = {
        stat_box_inner.x + 4,
        stat_box_inner.y + 4,
        stat_box_inner.w - 8,
        stat_box_inner.h - 8
    };

    SDL_Rect info_box_inner = { 176, 34, 304 -176, (122 + 22) - 34 };
    SDL_Rect info_box_content = {
        info_box_inner.x + 4,
        info_box_inner.y + 4,
        info_box_inner.w - 8,
        info_box_inner.h - 8
    };

    if (arg1) {
        arg1 = 1;
    }

    // Make sure we have a valid team
    if (myscreen->save_data.team_size < 1) {
        popup_dialog("NEED A TEAM!", "You need to\nhire a team\nto train");

        return OK;
    }

    myscreen->clearbuffer();

    if (localbuttons) {
        delete localbuttons;
    }

    trainmenu_buttons[18].hidden = true;

    Button *buttons = trainmenu_buttons;
    Sint32 num_buttons = 20;
    Sint32 highlighted_button = 1;
    localbuttons = init_buttons(buttons, num_buttons);

    for (i = 2; i < 14; ++i) {
        // 2, 4, ..., 12
        if (i % 2 == 0) {
            allbuttons[i]->set_graphic(FAMILY_MINUS);
        } else {
            allbuttons[i]->set_graphic(FAMILY_PLUS);
        }
    }

    Guy **ourteam = myscreen->save_data.team_list;

    // Set to first guy on list using global variable...
    cycle_team_guy(0);
    Guy *here = ourteam[editguy];
    current_cost = calculate_train_cost(here);

    grab_mouse();
    clear_keyboard();
    clear_key_press_event();

    while (!(retvalue & EXIT)) {
        // Input
        clickvalue = leftmouse(buttons);
        if (clickvalue == 1) {
            retvalue = localbuttons->leftclick();
        } else if (clickvalue == 2) {
            retvalue = localbuttons->rightclick();
        }

        handle_menu_nav(buttons, highlighted_button, retvalue);

        // Reset buttons
        if (localbuttons && ((retvalue == OK) || (retvalue == REDRAW))) {
            if (retvalue == REDRAW) {
                delete localbuttons;
                localbuttons = init_buttons(buttons, num_buttons);

                for (i = 2; i < 14; ++i) {
                    // 2, 4, ..., 12
                    if (i % 2 == 0) {
                        allbuttons[i]->set_graphic(FAMILY_MINUS);
                    } else {
                        allbuttons[i]->set_graphic(FAMILY_PLUS);
                    }
                }

                cycle_team_guy(0);
            }

            if (current_guy == nullptr) {
                cycle_team_guy(0);
            }

            if (here != ourteam[editguy]) {
                here = ourteam[editguy];
            }

            current_cost = calculate_train_cost(here);
            retvalue = 0;
        }

        // Draw
        myscreen->clearbuffer();

        draw_backdrop();
        draw_buttons(buttons, num_buttons);

        // 1 means ourteam[editguy]
        picker_show_guy(query_timer() - start_time, 1, 80, 45);

        linesdown = 0;

        // Name box
        myscreen->draw_button(34, 8, 126, 24, 1, 1);
        myscreen->draw_text_bar(36, 10, 124, 22);

        Text &mytext = myscreen->text_normal;
        mytext.write_xy(80 - mytext.query_width(current_guy->name) / 2, 14,
                        current_guy->name, DARK_BLUE, 1);

        // Stats box
        myscreen->draw_button(38, 66, 120, 160, 1, 1);
        myscreen->draw_text_bar(42, 70, 116, 156);

        bool level_increased = (old_guy->get_level() < current_guy->get_level());
        bool stat_increased;

        if (level_increased) {
            stat_increased = false;
        } else {
            stat_increased = ((old_guy->strength < current_guy->strength)
                              || (old_guy->dexterity < current_guy->dexterity)
                              || (old_guy->constitution < current_guy->constitution)
                              || (old_guy->intelligence < current_guy->intelligence)
                              || (old_guy->armor < current_guy->armor));
        }

        // Strength
        std::stringstream buf;
        std::streamsize orig_precision = buf.precision();
        buf << current_guy->strength;
        mytext.write_xy(stat_box_content.x, DOWN(linesdown),
                        "  STR:", STAT_COLOR, 1);

        if (level_increased) {
            showcolor = STAT_LEVELED;
        } else if (here->strength < current_guy->strength) {
            showcolor = STAT_CHANGED;
        } else {
            showcolor = STAT_COLOR;
        }

        mytext.write_xy(stat_box_content.x + STAT_NUM_OFFSET, DOWN(linesdown),
                        buf.str(), showcolor, 1);
        buf.clear();
        ++linesdown;

        // Dexterity
        buf << current_guy->dexterity;
        mytext.write_xy(stat_box_content.x, DOWN(linesdown),
                        "  DEX:", STAT_COLOR, 1);

        if (level_increased) {
            showcolor = STAT_LEVELED;
        } else if (here->dexterity < current_guy->dexterity) {
            showcolor = STAT_CHANGED;
        } else {
            showcolor = STAT_COLOR;
        }

        mytext.write_xy(stat_box_content.x + STAT_NUM_OFFSET, DOWN(linesdown),
                        buf.str(), showcolor, 1);
        buf.clear();
        ++linesdown;

        // Constitution
        buf << current_guy->constitution;
        mytext.write_xy(stat_box_content.x, DOWN(linesdown),
                        "  CON:", STAT_COLOR, 1);

        if (level_increased) {
            showcolor = STAT_LEVELED;
        } else if (here->constitution < current_guy->constitution) {
            showcolor = STAT_CHANGED;
        } else {
            showcolor = STAT_COLOR;
        }

        mytext.write_xy(stat_box_content.x + STAT_NUM_OFFSET, DOWN(linesdown),
                        buf.str(), showcolor, 1);
        buf.clear();
        ++linesdown;

        // Intelligence
        buf << current_guy->intelligence;
        mytext.write_xy(stat_box_content.x, DOWN(linesdown),
                        "  INT:", STAT_COLOR, 1);

        if (level_increased) {
            showcolor = STAT_LEVELED;
        } else if (here->intelligence < current_guy->intelligence) {
            showcolor = STAT_CHANGED;
        } else {
            showcolor = STAT_COLOR;
        }

        mytext.write_xy(stat_box_content.x + STAT_NUM_OFFSET, DOWN(linesdown),
                        buf.str(), showcolor, 1);
        buf.clear();
        ++linesdown;

        // Armor
        buf << current_guy->armor;
        mytext.write_xy(stat_box_content.x, DOWN(linesdown),
                        "ARMOR:", STAT_COLOR, 1);

        if (level_increased) {
            showcolor = STAT_LEVELED;
        } else if (here->armor < current_guy->armor) {
            showcolor = STAT_CHANGED;
        } else {
            showcolor = STAT_COLOR;
        }

        mytext.write_xy(stat_box_content.x + STAT_NUM_OFFSET, DOWN(linesdown),
                        buf.str(), showcolor, 1);
        buf.clear();
        ++linesdown;

        // Level
        buf << current_guy->get_level();
        mytext.write_xy(stat_box_content.x, DOWN(linesdown), "LEVEL:",
                        STAT_COLOR, 1);

        if (level_increased) {
            showcolor = STAT_CHANGED;
        } else if (stat_increased) {
            showcolor = STAT_DISABLED;
        } else {
            showcolor = STAT_COLOR;
        }

        mytext.write_xy(stat_box_content.x + STAT_NUM_OFFSET, DOWN(linesdown),
                        buf.str(), showcolor, 1);
        buf.clear();
        ++linesdown;

        // Info box
        myscreen->draw_button(174, 32, 306, 114 + 22, 1, 1);
        // Main text box
        myscreen->draw_text_bar(176, 34, 304, 112 + 22);

        showcolor = DARK_BLUE;
        linesdown = 0;
        Sint32 line_height = 10;
        Sint32 derived_offset = (3 * STAT_NUM_OFFSET) / 4;

        buf << "Total Kills: " << current_guy->kills;
        mytext.write_xy(180, info_box_content.y + (linesdown * line_height),
                        buf.str(), DARK_BLUE, 1);
        buf.clear();

        ++linesdown;

        // Have we at least hit something? :)
        if (current_guy->total_hits && current_guy->total_shots) {
            buf << "   Accuracy: "
                << (current_guy->total_hits * 100) / current_guy->total_shots
                << "% ";

            mytext.write_xy(180, info_box_content.y + (linesdown * line_height),
                            buf.str(), DARK_BLUE, 1);
            buf.clear();
        } else {
            // Haven't ever hit anyone
            buf << "   Accuracy: N/A ";
            mytext.write_xy(180, info_box_content.y + (linesdown * line_height),
                            buf.str(), DARK_BLUE, 1);
            buf.clear();
        }

        ++linesdown;

        buf << " EXPERIENCE: " << current_guy->exp;
        mytext.write_xy(180, info_box_content.y + (linesdown * line_height),
                        buf.str(), DARK_BLUE, 1);
        buf.clear();

        ++linesdown;

        // Separator bar
        SDL_Rect r = {
            info_box_content.x + 10,
            (info_box_content.y + (linesdown * line_height)) - 2,
            info_box_content.w - 20,
            2
        };

        myscreen->draw_button_inverted(r);

        float val;

        mytext.write_xy(info_box_content.x,
                        info_box_content.y + (linesdown * line_height),
                        "HP:", STAT_DERIVED, 1);

        val = ceilf(myscreen->level_data.myloader.hitpoints[PIX(ORDER_LIVING, current_guy->family)]
                    + current_guy->get_hp_bonus());
        if (val != 0) {
            buf << val;
        }

        mytext.write_xy((info_box_content.x + derived_offset) - 9,
                        info_box_content.y + (linesdown * line_height),
                        HIGH_HP_COLOR, buf);
        buf.clear();

        mytext.write_xy((info_box_content.x + derived_offset) + 18,
                        info_box_content.y + (linesdown * line_height),
                        "MP:", STAT_DERIVED, 1);

        val =  ceilf(current_guy->get_mp_bonus());
        if (val != 0) {
            buf << val;
        }

        mytext.write_xy(((info_box_content.x + (2 * derived_offset)) + 18) - 9,
                        info_box_content.y + (linesdown * line_height),
                        MAX_MP_COLOR, buf);
        buf.clear();

        ++linesdown;

        mytext.write_xy(info_box_content.x,
                        info_box_content.y + (linesdown * line_height),
                        "ATK:", STAT_DERIVED, 1);

        val = myscreen->level_data.myloader.damage[PIX(ORDER_LIVING, current_guy->family)]
            + current_guy->get_damage_bonus();
        if (val != 0) {
            buf << val;
        }

        mytext.write_xy((info_box_content.x + derived_offset) - 3,
                        info_box_content.y + (linesdown * line_height),
                        showcolor, buf);
        buf.clear();

        mytext.write_xy((info_box_content.x + derived_offset) + 18,
                        info_box_content.y + (linesdown * line_height),
                        "DEF:", STAT_DERIVED, 1);

        val = current_guy->get_armor_bonus();
        if (val != 0) {
            buf << val;
        }

        mytext.write_xy(((info_box_content.x + (2 * derived_offset)) + 18) - 3,
                        info_box_content.y + (linesdown * line_height),
                        showcolor, buf);
        buf.clear();

        ++linesdown;

        mytext.write_xy(info_box_content.x,
                        info_box_content.y + (linesdown * line_height),
                        "SPD:", STAT_DERIVED, 1);

        val = myscreen->level_data.myloader.stepsizes[PIX(ORDER_LIVING, current_guy->family)]
            + current_guy->get_speed_bonus();
        if (val != 0) {
            buf << std::setprecision(1) << val;
        }

        mytext.write_xy(info_box_content.x + derived_offset,
                        info_box_content.y + (linesdown * line_height),
                        showcolor, buf);
        buf.clear();
        buf << std::setprecision(orig_precision);

        ++linesdown;
        mytext.write_xy(info_box_content.x,
                        info_box_content.y + (linesdown * line_height),
                        "ATK SPD:", STAT_DERIVED, 1);

        float fire_freq = std::max(myscreen->level_data.myloader.fire_frequency[PIX(ORDER_LIVING, current_guy->family)] - current_guy->get_fire_frequency_bonus(), 1.0f);

        // The 10.0f / fire_frequency is somewhat arbitrary, but it makes for
        // good comparison info.
        val = 10.0f / fire_freq;
        if (val != 0) {
            buf << std::setprecision(1) << val;
        }

        mytext.write_xy((info_box_content.x + derived_offset) + 21,
                        info_box_content.y + (linesdown * line_height),
                        showcolor, buf);
        buf.clear();
        buf << std::setprecision(orig_precision);

        ++linesdown;

        // Separator bar
        SDL_Rect r2 = {
            info_box_content.x + 10,
            (info_box_content.y + (linesdown * line_height)) - 2,
            info_box_content.w - 20,
            2
        };

        myscreen->draw_button_inverted(r2);

        linesdown += 0.4f;
        buf << "CASH: " << myscreen->save_data.m_totalcash[current_guy->teamnum];
        mytext.write_xy(180, info_box_content.y + (linesdown * line_height),
                        buf.str(), DARK_BLUE, 1);
        buf.clear();

        ++linesdown;
        mytext.write_xy(180, info_box_content.y + (linesdown * line_height),
                        "COST: ", DARK_BLUE, 1);
        buf << "      " << current_cost;

        if (current_cost > myscreen->save_data.m_totalcash[current_guy->teamnum]) {
            mytext.write_xy(180, info_box_content.y + (linesdown * line_height),
                            buf.str(), STAT_CHANGED, 1);
            buf.clear();
        } else {
            mytext.write_xy(180, info_box_content.y + (linesdown * line_height),
                            buf.str(), STAT_COLOR, 1);
            buf.clear();
        }

        // Display our team setting...
        buf << "Playing on Team " << current_guy->teamnum + 1;
        allbuttons[18]->label = buf.str();
        buf.clear();
        allbuttons[18]->vdisplay();

        draw_highlight(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);
    }

    myscreen->clearbuffer();

    return REDRAW;
}

Sint32 create_load_menu(Sint32 arg1)
{
    Sint32 retvalue = 0;
    Sint32 i;
    Text &loadtext = myscreen->text_normal;

    if (arg1) {
        arg1 = 1;
    }

    if (localbuttons) {
        delete localbuttons;
    }

    Button *buttons = loadteam_buttons;
    Sint32 num_buttons = 11;
    Sint32 highlighted_button = 10;
    localbuttons = init_buttons(buttons, num_buttons);

    while (!(retvalue & EXIT)) {
        // Input
        if (leftmouse(buttons)) {
            retvalue = localbuttons->leftclick();

            if (retvalue == REDRAW) {
                return REDRAW;
            }
        }

        handle_menu_nav(buttons, highlighted_button, retvalue);

        if (retvalue == REDRAW) {
            return REDRAW;
        }

        // Reset buttons
        reset_buttons(localbuttons, buttons, num_buttons, retvalue);

        // Draw
        myscreen->clearbuffer();
        draw_backdrop();
        draw_buttons(buttons, num_buttons);

        myscreen->draw_button(15, 9, 255, 199, 1, 1);
        myscreen->draw_text_bar(19, 13, 251, 21);

        std::string message("Gladiator: Load Game");
        loadtext.write_xy(135 - (message.size() * 3), 15, message, RED, 1);

        std::stringstream buf;
        for (i = 0; i < 10; ++i) {
            buf << "save" << i + 1;
            allbuttons[i]->label = get_saved_name(buf.str());
            buf.clear();
            myscreen->draw_text_bar(23, 23 + (i * BUTTON_HEIGHT), 246, 36 + (BUTTON_HEIGHT * i));
            allbuttons[i]->vdisplay();
            myscreen->draw_box(allbuttons[i]->xloc - 1,
                               allbuttons[i]->yloc - 1,
                               allbuttons[i]->xend,
                               allbuttons[i]->yend, 0, 0, 1);
        }

        myscreen->draw_text_bar(23, allbuttons[10]->yloc - 2, 66, allbuttons[10]->yend + 1);
        allbuttons[10]->vdisplay();
        myscreen->draw_box(allbuttons[10]->xloc - 1,
                           allbuttons[10]->yloc - 1,
                           allbuttons[10]->xend,
                           allbuttons[10]->yend, 0, 0, 1);

        draw_highlight(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);
    }

    return REDRAW;
}

void timed_dialog(std::string const &message, float delay_seconds)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s\n", message.c_str());

    myscreen->darken_screen();

    Text &gladtext = myscreen->text_normal;
    Sint32 pix_per_char = 6;
    Sint32 len = message.size();
    Sint32 width = len * pix_per_char;
    Sint32 leftside = (160 - (width / 2)) - 12;
    Sint32 rightside = (160 + (width / 2)) + 12;

    myscreen->draw_button(leftside, 80, rightside, 110, 1);
    gladtext.write_xy(160 - (width / 2), 94, message, DARK_BLUE, 1);

    // Refresh screen
    myscreen->buffer_to_screen(0, 0, 320, 200);

    grab_mouse();
    clear_keyboard();
    clear_key_press_event();

    Uint32 start_time = SDL_GetTicks();

    while (((SDL_GetTicks() - start_time) / 1000.0f) < delay_seconds) {
        get_input_events(POLL);

        if (query_mouse().left || query_key_press_event()) {
            break;
        }

        SDL_Delay(10);
    }
}

bool yes_or_no_prompt(std::string const &title, std::string const &message, bool default_value)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s, %s:\n", title.c_str(), message.c_str());

    myscreen->darken_screen();

    Text &gladtext = myscreen->text_normal;
    Sint32 pix_per_char = 6;

    // Break message into lines
    std::list<std::string> ls = explode(message, '\n');

    // Get the max dimensions needed to display it
    Uint32 w = title.size() * 9;
    Sint32 h = 30 + (10 * ls.size());

    for (auto const &e : ls) {
        if ((e.size() * pix_per_char) > w) {
            w = e.size() * pix_per_char;
        }
    }

    // Centered bounds
    Sint32 leftside = (160 - (w / 2)) - 12;
    Sint32 rightside = (160 + (w / 2)) + 12;
    Sint32 j;
    Sint32 dumbcount;

    if (localbuttons) {
        delete localbuttons;
    }

    Button *buttons = yes_or_no_buttons;
    Sint32 num_buttons = 2;
    Sint32 highlighted_button = default_value ? 0 : 1;
    localbuttons = init_buttons(buttons, num_buttons);

    grab_mouse();
    clear_keyboard();
    clear_key_press_event();

    Sint32 retvalue = 0;

    while (retvalue == 0) {
        get_input_events(POLL);

        // Input
        if (query_key_press_event()) {
            if (keystates[KEYSTATE_y]) {
                retvalue = YES;
            } else if (keystates[KEYSTATE_n]) {
                retvalue = NO;
            } else if (keystates[KEYSTATE_ESCAPE]) {
                break;
            }
        }

        if (leftmouse(buttons)) {
            retvalue = localbuttons->leftclick();
        }

        handle_menu_nav(buttons, highlighted_button, retvalue);

        // Reset buttons
        reset_buttons(localbuttons, buttons, num_buttons, retvalue);

        // Draw
        dumbcount = myscreen->draw_dialog(leftside, 80 - (h / 2), rightside, 80 + (h / 2), title);
        j = 0;

        for (auto const &e : ls) {
            gladtext.write_xy(dumbcount + ((3 * pix_per_char) / 2),
                              (104 - (h / 2)) + (10 * j),
                              e, DARK_BLUE, 1);
            ++j;
        }

        draw_buttons(buttons, num_buttons);
        draw_highlight_interior(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);
    }

    if (retvalue == YES) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "YES\n");

        return true;
    }

    if (retvalue == NO) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "NO\n");

        return false;
    }

    return default_value;
}

bool no_or_yes_prompt(std::string const &title, std::string const &message, bool default_value)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s, %s:\n", title.c_str(), message.c_str());

    myscreen->darken_screen();

    Text &gladtext = myscreen->text_normal;
    Sint32 pix_per_char = 6;

    // Break message into lines
    std::list<std::string> ls = explode(message, '\n');

    // Get the max dimensions needed to display it
    Uint32 w = title.size() * 9;
    Sint32 h = 30 + (10 * ls.size());

    for (auto const &e : ls) {
        if ((e.size() * pix_per_char) > w) {
            w = e.size() * pix_per_char;
        }
    }

    // Cenetered bounds
    Sint32 leftside = (160 - (w / 2)) - 12;
    Sint32 rightside = (160 + (w / 2)) + 12;
    Sint32 j = 0;
    Sint32 dumbcount;

    if (localbuttons) {
        delete localbuttons;
    }

    Button *buttons = no_or_yes_buttons;
    Sint32 num_buttons = 2;
    Sint32 highlighted_button = default_value ? 1 : 0;
    localbuttons = init_buttons(buttons, num_buttons);

    grab_mouse();
    clear_keyboard();
    clear_key_press_event();

    Sint32 retvalue = 0;

    while (retvalue == 0) {
        get_input_events(POLL);

        // Input
        if (query_key_press_event()) {
            if (keystates[KEYSTATE_y]) {
                retvalue = YES;
            } else if (keystates[KEYSTATE_n]) {
                retvalue = NO;
            } else if (keystates[KEYSTATE_ESCAPE]) {
                break;
            }
        }

        if (leftmouse(buttons)) {
            retvalue = localbuttons->leftclick();
        }

        handle_menu_nav(buttons, highlighted_button, retvalue);

        // Reset buttons
        reset_buttons(localbuttons, buttons, num_buttons, retvalue);

        // Draw
        dumbcount = myscreen->draw_dialog(leftside, 80 - (h / 2), rightside, 80 + (h / 2), title);

        j = 0;

        for (auto const &e : ls) {
            gladtext.write_xy(dumbcount + ((3 * pix_per_char) / 2),
                              (104 - (h / 2)) + (10 * j),
                              e, DARK_BLUE, 1);
            ++j;
        }

        draw_buttons(buttons, num_buttons);
        draw_highlight_interior(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);
    }

    if (retvalue == YES) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "YES\n");

        return true;
    }

    if (retvalue == NO) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "NO\n");

        return false;
    }

    return default_value;
}

void popup_dialog(std::string const &title, std::string const &message)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s, %s\n", title.c_str(), message.c_str());

    myscreen->darken_screen();

    Text &gladtext = myscreen->text_normal;
    Sint32 pix_per_char = 6;

    // Break message into lines
    std::list<std::string> ls = explode(message, '\n');

    // Get the max dimensions needed to display it
    Uint32 w = title.size() * 9;
    Sint32 h = 20 + (10 * ls.size());

    for (auto const &e : ls) {
        if ((e.size() * pix_per_char) > w) {
            w = e.size() * pix_per_char;
        }
    }

    // Cenetered bounds
    Sint32 leftside = (160 - (w / 2)) - 12;
    Sint32 rightside = (160 + (w / 2)) + 12;

    // Draw background
    Sint32 dumbcount;

    // Draw message
    Sint32 j = 0;

    if (localbuttons) {
        delete localbuttons;
    }

    Button *buttons = popup_dialog_buttons;
    Sint32 num_buttons = 1;
    Sint32 highlighted_button = 0;
    localbuttons = init_buttons(buttons, num_buttons);

    grab_mouse();
    clear_keyboard();
    clear_key_press_event();

    Sint32 retvalue = 0;

    while (retvalue == 0) {
        // Input
        get_input_events(POLL);

        if (query_key_press_event()) {
            if (keystates[KEYSTATE_RETURN]
                || keystates[KEYSTATE_SPACE]
                || keystates[KEYSTATE_ESCAPE]) {
                break;
            }
        }

        if (leftmouse(buttons)) {
            retvalue = localbuttons->leftclick();
        }

        handle_menu_nav(buttons, highlighted_button, retvalue);

        // Reset buttons
        reset_buttons(localbuttons, buttons, num_buttons, retvalue);

        // Draw
        dumbcount = myscreen->draw_dialog(leftside, 80 - (h / 2), rightside, 80 + (h / 2), title);
        j = 0;

        for (auto const &e : ls) {
            gladtext.write_xy(((dumbcount + ((3 * pix_per_char) / 2)) + (w / 2)) - ((e.size() * pix_per_char) / 2),
                              (104 - (h / 2)) + (10 * j), e, DARK_BLUE, 1);
            ++j;
        }

        draw_buttons(buttons, num_buttons);
        draw_highlight_interior(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);
    }
}

Sint32 create_save_menu(Sint32 arg1)
{
    Sint32 retvalue = 0;
    Sint32 i;
    Text &savetext = myscreen->text_normal;

    if (arg1) {
        arg1 = 1;
    }

    if (localbuttons) {
        delete localbuttons;
    }

    Button *buttons = saveteam_buttons;
    Sint32 num_buttons = 11;
    Sint32 highlighted_button = 10;
    localbuttons = init_buttons(buttons, num_buttons);

    while (!(retvalue & EXIT)) {
        // Input
        if (leftmouse(buttons)) {
            retvalue = localbuttons->leftclick();

            if (retvalue == REDRAW) {
                return REDRAW;
            }
        }

        handle_menu_nav(buttons, highlighted_button, retvalue);

        if (retvalue == REDRAW) {
            return REDRAW;
        }

        // Reset buttons
        reset_buttons(localbuttons, buttons, num_buttons, retvalue);

        // Draw
        myscreen->clearbuffer();
        draw_backdrop();
        draw_buttons(buttons, num_buttons);

        myscreen->draw_button(15, 9, 255, 199, 1, 1);
        myscreen->draw_text_bar(19, 13, 251, 21);
        std::string message("Gladiator: Save Game");
        message.resize(80);
        savetext.write_xy(135 - (message.size() * 2), 15, message, RED, 1);

        std::string temp_filename;
        std::stringstream buf;
        for (i = 0; i < 10; ++i) {
            buf << "save" << i + 1;
            temp_filename = buf.str();
            buf.clear();
            temp_filename.resize(20);
            allbuttons[i]->label = get_saved_name(temp_filename);
            myscreen->draw_text_bar(23, 23 + (i * BUTTON_HEIGHT), 246, 36 + (BUTTON_HEIGHT * i));
            allbuttons[i]->vdisplay();
            myscreen->draw_box(allbuttons[i]->xloc - 1,
                               allbuttons[i]->yloc - 1,
                               allbuttons[i]->xend,
                               allbuttons[i]->yend, 0, 0, 1);
        }

        myscreen->draw_text_bar(23, allbuttons[10]->yloc - 2, 66, allbuttons[10]->yend + 1);
        allbuttons[10]->vdisplay();
        myscreen->draw_box(allbuttons[10]->xloc - 1,
                           allbuttons[10]->yloc - 1,
                           allbuttons[10]->xend,
                           allbuttons[10]->yend, 0, 0, 1);

        draw_highlight(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);
    }

    return REDRAW;
}

Sint32 increase_stat(Sint32 whatstat, Sint32 howmuch)
{
    bool level_increased = (old_guy->get_level() < current_guy->get_level());
    bool stat_increased;

    if (level_increased) {
        stat_increased = false;
    } else {
        stat_increased = ((old_guy->strength < current_guy->strength)
                          || (old_guy->dexterity < current_guy->dexterity)
                          || (old_guy->constitution < current_guy->constitution)
                          || (old_guy->intelligence < current_guy->intelligence)
                          || (old_guy->armor < current_guy->armor));
    }

    switch (whatstat) {
    case BUT_STR:
        if (!level_increased) {
            current_guy->strength += howmuch;
        }

        break;
    case BUT_DEX:
        if (!level_increased) {
            current_guy->dexterity += howmuch;
        }

        break;
    case BUT_CON:
        if (!level_increased) {
            current_guy->constitution += howmuch;
        }

        break;
    case BUT_INT:
        if (!level_increased) {
            current_guy->intelligence += howmuch;
        }

        break;
    case BUT_ARMOR:
        if (!level_increased) {
            current_guy->armor += howmuch;
        }

        break;
    case BUT_LEVEL:
        if (!stat_increased) {
            Sint16 newlevel = current_guy->get_level() + howmuch;
            current_guy->upgrade_to_level(newlevel);
        }

        break;
    default:

        break;
    }

    return OK;
}

Sint32 decrease_stat(Sint32 whatstat, Sint32 howmuch)
{
    bool level_increased = (old_guy->get_level() < current_guy->get_level());
    bool stat_increased;

    if (level_increased) {
        stat_increased = false;
    } else {
        stat_increased = ((old_guy->strength < current_guy->strength)
                          || (old_guy->dexterity < current_guy->dexterity)
                          || (old_guy->constitution < current_guy->constitution)
                          || (old_guy->intelligence < current_guy->intelligence)
                          || (old_guy->armor < current_guy->armor));
    }

    switch (whatstat) {
    case BUT_STR:
        if (!level_increased) {
            current_guy->strength -= howmuch;
        }

        break;
    case BUT_DEX:
        if (!level_increased) {
            current_guy->dexterity -= howmuch;
        }

        break;
    case BUT_CON:
        if (!level_increased) {
            current_guy->constitution -= howmuch;
        }

        break;
    case BUT_INT:
        if (!level_increased) {
            current_guy->intelligence -= howmuch;
        }

        break;
    case BUT_ARMOR:
        if (!level_increased) {
            current_guy->armor -= howmuch;
        }

        break;
    case BUT_LEVEL:
        if (!stat_increased) {
            Sint16 newlevel = current_guy->get_level() - howmuch;

            if ((newlevel > 0)
                && (newlevel >= myscreen->save_data.team_list[editguy]->get_level())) {
                current_guy->upgrade_to_level(newlevel);

                if (current_guy->get_level() == myscreen->save_data.team_list[editguy]->get_level()) {
                    current_guy->exp = myscreen->save_data.team_list[editguy]->exp;
                }
            }
        }

        break;
    default:

        break;
    }

    return OK;
}

Uint32 calculate_hire_cost()
{
    Guy *ob = current_guy;
    Sint32 temp;
    Sint32 myfamily;

    if (!ob) {
        return 0;
    }

    myfamily = ob->family;
    temp = costlist[myfamily];

    // Long check of various things...
    if (ob->strength < statlist[myfamily][BUT_STR]) {
        ob->strength = statlist[myfamily][BUT_STR];
    }

    if (ob->dexterity < statlist[myfamily][BUT_DEX]) {
        ob->dexterity = statlist[myfamily][BUT_DEX];
    }

    if (ob->constitution < statlist[myfamily][BUT_CON]) {
        ob->constitution = statlist[myfamily][BUT_CON];
    }

    if (ob->intelligence < statlist[myfamily][BUT_INT]) {
        ob->intelligence = statlist[myfamily][BUT_INT];
    }

    if (ob->armor < statlist[myfamily][BUT_ARMOR]) {
        ob->armor = statlist[myfamily][BUT_ARMOR];
    }

    // Now figure out costs...
    temp += (pow(ob->strength - statlist[myfamily][BUT_STR], RAISE) * statcosts[myfamily][BUT_STR]);
    temp += (pow(ob->dexterity - statlist[myfamily][BUT_DEX], RAISE) * statcosts[myfamily][BUT_DEX]);
    temp += (pow(ob->constitution - statlist[myfamily][BUT_CON], RAISE) * statcosts[myfamily][BUT_CON]);
    temp += (pow(ob->intelligence - statlist[myfamily][BUT_INT], RAISE) * statcosts[myfamily][BUT_INT]);
    temp += (pow(ob->armor - statlist[myfamily][BUT_ARMOR], RAISE) * statcosts[myfamily][BUT_ARMOR]);

    if (ob->get_level() < statlist[myfamily][BUT_LEVEL]) {
        ob->upgrade_to_level(statlist[myfamily][BUT_LEVEL]);
    }

    // Overflow
    if (calculate_exp(ob->get_level()) < 0) {
        ob->upgrade_to_level(1);
    }

    temp += calculate_exp(ob->get_level());

    if (temp < 0) {
        cycle_guy(0);

        // This used to be an error code check by picker.cpp line 2416
        temp = 0;
    }

    return temp;
}

// This version compares current_guy versus the old version...
Uint32 calculate_train_cost(Guy *oldguy)
{
    Guy *ob = current_guy;
    Sint32 temp;
    Sint32 myfamily;

    if (!ob || !oldguy) {
        return 0;
    }

    myfamily = ob->family;
    temp = 0;

    // Long check of various things...
    if (ob->strength < oldguy->strength) {
        ob->strength = oldguy->strength;
    }

    if (ob->dexterity < oldguy->dexterity) {
        ob->dexterity = oldguy->dexterity;
    }

    if (ob->constitution < oldguy->constitution) {
        ob->constitution = oldguy->constitution;
    }

    if (ob->intelligence < oldguy->intelligence) {
        ob->intelligence = oldguy->intelligence;
    }

    if (ob->armor < oldguy->armor) {
        ob->armor = oldguy->armor;
    }

    // Now figure out costs...
    // Add on extra level cost..
    if (ob->get_level() < oldguy->get_level()) {
        ob->upgrade_to_level(oldguy->get_level());
    }

    if (calculate_exp(ob->get_level()) > oldguy->exp) {
        temp += (calculate_exp(ob->get_level()) - oldguy->exp);
    }

    // Only count these costs if the level is not being upgraded
    if (ob->get_level() <= old_guy->get_level()) {
        // First we have our 'total increased value...'
        temp += (pow(ob->strength - statlist[myfamily][BUT_STR], RAISE) * statcosts[myfamily][BUT_STR]);
        temp += (pow(ob->dexterity - statlist[myfamily][BUT_DEX], RAISE) * statcosts[myfamily][BUT_DEX]);
        temp += (pow(ob->constitution - statlist[myfamily][BUT_CON], RAISE) * statcosts[myfamily][BUT_CON]);
        temp += (pow(ob->intelligence - statlist[myfamily][BUT_INT], RAISE) * statcosts[myfamily][BUT_INT]);
        temp += (pow(ob->armor - statlist[myfamily][BUT_ARMOR], RAISE) * statcosts[myfamily][BUT_ARMOR]);

        // Now subtract what we've already paid for...
        temp -= (pow(oldguy->strength - statlist[myfamily][BUT_STR], RAISE) * statcosts[myfamily][BUT_STR]);
        temp -= (pow(oldguy->dexterity - statlist[myfamily][BUT_DEX], RAISE) * statcosts[myfamily][BUT_DEX]);
        temp -= (pow(oldguy->constitution - statlist[myfamily][BUT_CON], RAISE) * statcosts[myfamily][BUT_CON]);
        temp -= (pow(oldguy->intelligence - statlist[myfamily][BUT_INT], RAISE) * statcosts[myfamily][BUT_INT]);
        temp -= (pow(oldguy->armor - statlist[myfamily][BUT_ARMOR], RAISE) * statcosts[myfamily][BUT_ARMOR]);
    }

    if (temp < 0) {
        temp = 0;
        statscopy(current_guy, oldguy);
        cycle_team_guy(0);
    }

    return temp;
}

std::string get_random_name(Uint8 family)
{
    switch (family) {
    case FAMILY_ARCHER:
        return GET_RAND_ELEM(archer_names);
    case FAMILY_CLERIC:
        return GET_RAND_ELEM(cleric_names);
    case FAMILY_DRUID:
        return GET_RAND_ELEM(druid_names);
    case FAMILY_ELF:
        return GET_RAND_ELEM(elf_names);
    case FAMILY_MAGE:
        return GET_RAND_ELEM(mage_names);
    case FAMILY_SOLDIER:
        return GET_RAND_ELEM(soldier_names);
    case FAMILY_THIEF:
        return GET_RAND_ELEM(thief_names);
    case FAMILY_ARCHMAGE:
        return GET_RAND_ELEM(mage_names);
    case FAMILY_ORC:
        return GET_RAND_ELEM(orc_names);
    case FAMILY_BIG_ORC:
        return GET_RAND_ELEM(orc_names);
    case FAMILY_BARBARIAN:
        return GET_RAND_ELEM(barbarian_names);
    case FAMILY_FIRE_ELEMENTAL:
        return GET_RAND_ELEM(elemental_names);
    case FAMILY_SKELETON:
        return GET_RAND_ELEM(skeleton_names);
    case FAMILY_SLIME:
    case FAMILY_MEDIUM_SLIME:
    case FAMILY_SMALL_SLIME:
        return GET_RAND_ELEM(slime_names);
    case FAMILY_FAERIE:
        return GET_RAND_ELEM(faerie_names);
    case FAMILY_GHOST:
        return GET_RAND_ELEM(ghost_names);
    default:
        return GET_RAND_ELEM(soldier_names);
    }
}

bool has_name_in_team(std::string const &name)
{
    Guy **ourteam = myscreen->save_data.team_list;
    Sint32 team_size = myscreen->save_data.team_size;

    for (Sint32 i = 0; i < team_size; ++i) {
        if (ourteam[i]->name == name) {
            return true;
        }
    }

    return false;
}

std::string get_new_name(Uint8 family) {
    std::stringstream buf;
    std::string result = get_random_name(family);

    // Try a few times to get a unique name
    Sint32 i = 0;
    while (has_name_in_team(result) && (i < 10)) {
        result = get_random_name(family);
        ++i;
    }

    // A bare name is a duplicate?
    if (has_name_in_team(result)) {
        // Append a number
        i = 2;
        buf << result << i;
        ++i;

        while (has_name_in_team(buf.str())) {
            buf.clear();
            buf << result << i;
            ++i;
        }

        result = buf.str();
        buf.clear();
        result.resize(50);
    }

    return result;
}

Sint32 cycle_guy(Sint32 whichway)
{
    Sint32 newfamily;

    if (current_guy == nullptr) {
        newfamily = allowable_guys[0];
    } else {
        current_type = (current_type + whichway) + (sizeof(allowable_guys) / 4);
        current_type %= (sizeof(allowable_guys) / 4);

        if (current_type < 0) {
            current_type = (sizeof(allowable_guys) / 4) - 1;
        }

        newfamily = allowable_guys[current_type];
    }

    // Delete the old guy...
    if (current_guy) {
        delete current_guy;
        current_guy = nullptr;
    }

    // Make the new guy
    current_guy = new Guy(newfamily);
    current_guy->teamnum = current_team_num;
    current_guy->name = get_new_name(newfamily);
    current_guy->name.resize(12);

    picker_show_guy(0, 0, 80, 45);

    grab_mouse();

    return OK;
}

// Sets current_guy to 'whichguy' in the teamlist, and returns a COPY of him
// as the function result
Sint32 cycle_team_guy(Sint32 whichway)
{
    if (myscreen->save_data.team_size < 1) {
        return -1;
    }

    Guy **ourteam = myscreen->save_data.team_list;

    editguy += whichway;

    if (editguy < 0) {
        editguy += MAX_TEAM_SIZE;

        while (!ourteam[editguy]) {
            --editguy;
        }
    }

    if ((editguy < 0) || (editguy >= MAX_TEAM_SIZE)) {
        editguy = 0;
    }

    if (!whichway && !ourteam[editguy]) {
        whichway = 1;
    }

    while (!ourteam[editguy]) {
        editguy += whichway;

        if ((editguy < 0) || (editguy >= MAX_TEAM_SIZE)) {
            editguy = 0;
        }
    }

    if (current_guy) {
        delete current_guy;
    }

    current_guy = new Guy(ourteam[editguy]->family);
    statscopy(current_guy, ourteam[editguy]);
    old_guy = ourteam[editguy];

    picker_show_guy(0, 0, 80, 45);

    current_team_num = current_guy->teamnum;

    // Set our team button back to normal color...
    // Zardus: FIX: Added a check for null pointers
    if (allbuttons[18]) {
        allbuttons[18]->do_outline = 0;
    }

    return OK;
}

Sint32 add_guy(Guy *newguy)
{
    Sint32 i;

    for (i = 0; i < MAX_TEAM_SIZE; ++i) {
        if (!myscreen->save_data.team_list[i]) {
            myscreen->save_data.team_list[i] = newguy;
            ++myscreen->save_data.team_size;

            return i;
        }
    }

    // Failed the case; too many guys
    return -1;
}

// 0 == current_guy, 1 == ourteam[editguy]
Sint32 name_guy(Sint32 arg)
{
    Text &nametext = myscreen->text_normal;
    Guy *someguy;

    if (arg) {
        someguy = myscreen->save_data.team_list[editguy];
    } else {
        someguy = current_guy;
    }

    if (someguy != nullptr) {
        return REDRAW;
    }

    release_mouse();

    myscreen->draw_button(174, 8, 306, 30, 1, 1); // Text box
    nametext.write_xy(176, 12, "NAME THIS CHARACTER:", DARK_BLUE, 1);
    myscreen->buffer_to_screen(0, 0, 320, 200);

    clear_keyboard();
    std::string new_text(nametext.input_string(176, 20, 11, someguy->name, DARK_BLUE, 13));

    if (new_text.empty()) {
        new_text = someguy->name;
    }

    someguy->name = new_text;
    myscreen->draw_button(174, 8, 306, 1, 1); // Text box

    myscreen->buffer_to_screen(0, 0, 320, 200);
    grab_mouse();

    return REDRAW;
}

Sint32 add_guy(Sint32 ignoreme)
{
    Sint32 newfamily = current_guy->family;
    // buffers: Changed typename to type_name due to some compile error
    std::string type_name;
    Sint32 i;

    // Abort abort!
    if (myscreen->save_data.team_size >= MAX_TEAM_SIZE) {
        return -1;
    }

    // We should be adding current guy
    if (current_guy == nullptr) {
        return -1;
    }

    Uint32 cost = calculate_hire_cost();

    if ((cost == 0) || (cost > myscreen->save_data.m_totalcash[current_team_num])) {
        return OK;
    }

    myscreen->save_data.m_totalcash[current_team_num] -= cost;

    Guy ** ourteam = myscreen->save_data.team_list;

    for (i = 0; i < MAX_TEAM_SIZE; ++i) {
        // Found an empty slot
        if (!ourteam[i]) {
            current_guy->teamnum = current_team_num;
            ourteam[i] = current_guy;
            ++myscreen->save_data.team_size;
            current_guy = nullptr;
            release_mouse();

            std::string name;
            if (prompt_for_string("NAME THIS CHARACTER", name)) {
                ourteam[i]->name.assign(name);
                ourteam[i]->name.resize(12);
            }

            grab_mouse();

            // Increment the next guy's number
            ++numbought[newfamily];

            // Ensure we have the right exp for our level
            ourteam[i]->exp = calculate_exp(ourteam[i]->get_level());

            // Grab a new, generic, guy to be edited/bought
            current_guy = new Guy(newfamily);
            type_name = current_guy->name;
            type_name.resize(30);
            // Set to same stats as just bought
            statscopy(current_guy, ourteam[i]);
            current_guy->name = type_name;
            current_guy->name.resize(12);

            // Return okay status
            return OK;
        }
    }

    return OK;
}

// Accept changes...
Sint32 edit_guy(Sint32 arg1)
{
    Guy *here;
    MouseState &cheatmouse = query_mouse();

    if (arg1) {
        arg1 = 1;
    }

    if (current_guy == nullptr) {
        return -1;
    }

    here = myscreen->save_data.team_list[editguy];

    if (!here) {
        // Error case; should never happen
        return -1;
    }

    // This is for cheating! Only CHEAT :)
    // When holding down the right mouse button, can always accept free changes
    if (CHEAT_MODE && cheatmouse.right) {
        if (here->get_level() != current_guy->get_level()) {
            current_guy->upgrade_to_level(current_guy->get_level());
        }

        statscopy(here, current_guy);

        return OK;
    }

    Uint32 cost = calculate_train_cost(here);

    // Compare cost of here to current guy
    if ((cost > myscreen->save_data.m_totalcash[current_guy->teamnum]) || (cost < 0)) {
        return OK;
    }

    // Cost of new - ols (current_guy - here)
    myscreen->save_data.m_totalcash[current_guy->teamnum] -= cost;

    if (here->get_level() != current_guy->get_level()) {
        current_guy->upgrade_to_level(current_guy->get_level());
    }

    statscopy(here, current_guy);

    // Color our team button normall
    allbuttons[18]->do_outline = 0;

    return OK;
}

// How many guys of family X on the team?
Sint32 how_many(Sint32 whatfamily)
{
    Sint32 counter = 0;
    Sint32 i;

    for (i = 0; i < MAX_TEAM_SIZE; ++i) {
        if (myscreen->save_data.team_list[i] && (myscreen->save_data.team_list[i]->family == whatfamily)) {
            ++counter;
        }
    }

    return counter;
}

Sint32 do_save(Sint32 arg1)
{
    release_mouse();
    clear_keyboard();

    std::string name = allbuttons[arg1 - 1]->label;

    if (prompt_for_string("NAME YOUR SAVED GAME", name)) {
        myscreen->save_data.save_name = name;

        std::stringstream buf("save");
        buf << arg1;
        std::string newname(buf.str());
        buf.clear();
        newname.resize(8);

        if (myscreen->save_data.save(newname)) {
            timed_dialog("GAME SAVED");
        } else {
            timed_dialog("SAVE FAILED");
        }
    } else {
        timed_dialog("SAVE CANCELED");
    }

    grab_mouse();

    return REDRAW;
}

Sint32 do_load(Sint32 arg1)
{
    std::stringstream buf("save");
    buf << arg1;
    std::string newname(buf.str());
    buf.clear();
    newname.resize(8);

    if (myscreen->save_data.load(newname)) {
        timed_dialog("GAME LOADED");
    } else {
        timed_dialog("LOAD FAILED");
    }

    return REDRAW;
}

std::string get_saved_name(std::string const &filename)
{
    SDL_RWops *infile;
    char temptext[10] = "GTL";
    char savedgame[40];
    Sint8 temp_version = 1;
    Sint16 temp_registered;

    /*
     * This only uses the first segment of the save format.
     * See load_team_list() for full format
     *
     * Format of a team list file is:
     * 3-byte header: 'GTL'
     * 1-byte version number (from graph.hpp)
     * 2-byte registered mark (version 7+ only)
     * 40-byte saved-game name (version 2 and up only!)
     * ...
     */
    // buffers: PORT: changed .GTL to .gtl
    std::filesystem::path temp_filename("save/" + filename);
    temp_filename.replace_extension(".gtl"); // Gladiator team list

    infile = open_read_file(temp_filename);

    // Open for read
    if (infile == nullptr) {
        return "EMPTY SLOT";
    }

    // Read ID header
    SDL_RWread(infile, temptext, 3, 1);

    if (strncmp(temptext, "GTL", strlen("GTL")) == 0) {
        SDL_RWclose(infile);
        strcpy(savedgame, "EMPTY SLOT");

        return savedgame;
    }

    // Read version number
    SDL_RWread(infile, &temp_version, 1, 1);

    if (temp_version != 1) {
        if (temp_version >= 2) {
            if (temp_version >= 7) {
                SDL_RWread(infile, &temp_registered, 2, 1);
            }

            SDL_RWread(infile, savedgame, 40, 1);
        } else {
            SDL_RWclose(infile);
            strcpy(savedgame, "SAVED GAME");

            return savedgame;
        }
    } else {
        // Fake the game name
        strcpy(savedgame, "SAVED GAME");
    }

    SDL_RWclose(infile);

    return savedgame;
}

Sint32 delete_all()
{
    Sint32 counter = myscreen->save_data.team_size;

    for (Sint32 i = 0; i < myscreen->save_data.team_size; ++i) {
        delete myscreen->save_data.team_list[i];
        myscreen->save_data.team_list[i] = nullptr;
    }

    myscreen->save_data.team_size = 0;

    return counter;
}

Sint32 go_menu(Sint32 arg1)
{
    // Save the current team in memory to save0.gtl, and run galdiator
    if (arg1) {
        arg1 = 1;
    }

    // Make sure we have a valid team
    if (myscreen->save_data.team_size < 1) {
        popup_dialog("NEED A TEAM!", "Please hire a\nteam before\nstarting the level");

        return REDRAW;
    }

    myscreen->save_data.save("save0");
    release_mouse();

    if (current_guy) {
        delete current_guy;
    }

    current_guy = nullptr;

    // Reset viewscreen prefs
    myscreen->ready_for_battle(myscreen->save_data.numplayers);

    glad_main(myscreen->save_data.numplayers);

    /*
     * *************************
     * Fade out from ACTION loop
     * *************************
     *
     * Zardus: PORT: New fade code
     */
    myscreen->fadeblack(0);

    // Zardus: PORT: Doesn't seem to be necessary
    myscreen->clearbuffer();

    grab_mouse();
    myscreen->reset(1);
    myscreen->viewob[0]->resize(PREF_VIEW_FULL);

    SDL_RWops *loadgame = open_read_file(std::filesystem::path("save/save0.gtl"));

    if (loadgame) {
        SDL_RWclose(loadgame);
        myscreen->save_data.load("save0");
    }

    while (myscreen->retry) {
        // Make sure we have valid team
        if (myscreen->save_data.team_size < 1) {
            popup_dialog("NEED A TEAM!", "Please hire a\nteam before\nstarting the level");

            return REDRAW;
        }

        myscreen->save_data.save("save0");
        release_mouse();

        if (current_guy) {
            delete current_guy;
        }

        current_guy = nullptr;

        // Reset viewscreen prefs
        myscreen->ready_for_battle(myscreen->save_data.numplayers);

        myscreen->fadeblack(0);

        // Zardus: PORT: Doesn't seem to be necessary
        myscreen->clearbuffer();

        grab_mouse();

        myscreen->reset(1);
        myscreen->viewob[0]->resize(PREF_VIEW_FULL);

        SDL_RWops *loadgame = open_read_file(std::filesystem::path("save/save0.gtl"));

        if (loadgame) {
            SDL_RWclose(loadgame);
            myscreen->save_data.load("save0");
        }
    }

    return CREATE_TEAM_MENU;
}

void statscopy(Guy *dest, Guy *source)
{
    dest->family = source->family;
    dest->strength = source->strength;
    dest->dexterity = source->dexterity;
    dest->constitution = source->constitution;
    dest->intelligence = source->intelligence;
    dest->set_level_number(source->get_level());
    dest->armor = source->armor;
    dest->exp = source->exp;
    dest->kills = source->kills;
    dest->level_kills = source->level_kills;
    dest->total_damage = source->total_damage;
    dest->total_hits = source->total_hits;
    dest->total_shots = source->total_shots;
    dest->teamnum = source->teamnum;
    dest->scen_damage = source->scen_damage;
    dest->scen_kills = source->scen_kills;
    dest->scen_damage_taken = source->scen_damage_taken;
    dest->scen_min_hp = source->scen_min_hp;
    dest->scen_shots = source->scen_shots;
    dest->scen_hits = source->scen_hits;

    dest->name = source->name;
}

void quit(Sint32 arg1)
{
    myscreen->refresh();
    delete theprefs;
    // Deletes the screen objects
    picker_quit();
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "quit(%d)\n", arg1);

    exit(0);
}

void draw_toggle_effect_button(Button &b, std::string const &category, std::string const &setting)
{
    if (b.hidden || b.no_draw) {
        return;
    }

    if (cfg.is_on(category, setting)) {
        myscreen->draw_button_colored(b.x - 1, b.y - 1, b.x + b.sizex, b.y + b.sizey,
                                      1, LIGHT_GREEN);
    } else {
        myscreen->draw_button_colored(b.x - 1, b.y - 1, b.x + b.sizex, b.y + b.sizey, 1, RED);
    }

    Text &mytext = myscreen->text_normal;
    std::stringstream buf;
    buf << b.label;
    mytext.write_xy_center(b.x + (b.sizex / 2), (b.y + (b.sizey / 2)) - 3, DARK_BLUE, buf);
    buf.clear();
}

Sint32 main_options()
{
    Text &mytext = myscreen->text_normal;

    if (localbuttons != NULL) {
        // We'll make a new set
        delete localbuttons;
    }

#if defined(ANDROID)
    main_options_buttons[3].no_draw = true;
    main_options_buttons[3].hidden = true;
    main_options_buttons[2].nav.right = -1;
    main_options_buttons[5].nav.up = 2;
#endif

    Button *buttons = main_options_buttons;
    Sint32 num_buttons = ARRAY_SIZE(main_options_buttons);
    Sint32 highlighted_button = 0;
    localbuttons = init_buttons(buttons, num_buttons);

    clear_keyboard();

    Sint32 retvalue = 0;

    while (!(retvalue & EXIT)) {
        // Input
        if (leftmouse(buttons)) {
            if (localbuttons->leftclick() == EXIT) {
                break;
            }
        }

        handle_menu_nav(buttons, highlighted_button, retvalue);

        if (retvalue == EXIT) {
            break;
        }

        // Reset buttons
        reset_buttons(localbuttons, buttons, num_buttons, retvalue);
        buttons[2].label = cfg.get_setting("graphics", "render");
        allbuttons[2]->label = buttons[2].label;

        // Draw
        // Clearing entire window because the overscan may have been adjusted
        myscreen->clear_window();

        myscreen->draw_button(0, 0, 320, 200, 0);
        myscreen->draw_button_inverted(4, 4, 312, 192);

        draw_buttons(buttons, num_buttons);

        draw_toggle_effect_button(buttons[1], "sound", "sound");
        myscreen->hor_line(60, buttons[2].y - (BUTTON_PADDING / 2), 200, PURE_WHITE);

        mytext.write_xy(20, buttons[2].y + 3, DARK_BLUE, "Rendering engine:");
        mytext.write_xy(20, (buttons[2].y + 3) + 10, DARK_BLUE, " (needs restart)");
        draw_toggle_effect_button(buttons[3], "graphics", "fullscreen");
        myscreen->hor_line(60, buttons[6].y - (BUTTON_PADDING / 2), 200, PURE_WHITE);

        mytext.write_xy(20, buttons[6].y + 3, DARK_BLUE, "Effects:");
        draw_toggle_effect_button(buttons[6], "effects", "mini_hp_bar");
        draw_toggle_effect_button(buttons[7], "effects", "hit_flash");
        draw_toggle_effect_button(buttons[8], "effects", "hit_recoil");
        draw_toggle_effect_button(buttons[9], "effects", "attack_lunge");
        draw_toggle_effect_button(buttons[10], "effects", "hit_anim");
        draw_toggle_effect_button(buttons[11], "effects", "damage_numbers");
        draw_toggle_effect_button(buttons[12], "effects", "heal_numbers");
        draw_toggle_effect_button(buttons[13], "effects", "gore");

        draw_highlight(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
        SDL_Delay(10);
    }

    myscreen->soundp->set_sound(!cfg.is_on("sound", "sound"));
    cfg.save_settings();

    return REDRAW;
}

Sint32 set_player_mode(Sint32 howmany)
{
    Sint32 count = 0;
    myscreen->save_data.numplayers = howmany;

    while (allbuttons[count]) {
        allbuttons[count]->vdisplay();
        ++count;
    }

    return OK;
}

// New functions
Sint32 return_menu(Sint32 arg)
{
    return arg;
}

Sint32 create_detail_menu(Guy *arg1)
{
    Sint32 retvalue = 0;
    Guy *thisguy;
    Sint32 start_time = query_timer();

    if (arg1) {
        thisguy = arg1;
    } else {
        thisguy = myscreen->save_data.team_list[editguy];
    }

    release_mouse();

    if (localbuttons) {
        delete localbuttons;
    }

    Button *buttons = details_buttons;
    Sint32 num_buttons = 2;
    Sint32 highlighted_button = 0;

    buttons[1].hidden = (((thisguy->family != FAMILY_MAGE) || (thisguy->get_level() < 6)) && ((thisguy->family != FAMILY_ORC) || (thisguy->get_level() < 5)));
    localbuttons = init_buttons(buttons, num_buttons);

    while (!(retvalue & EXIT)) {
        // 1 means ourteam[editguy]
        picker_show_guy(query_timer() - start_time, 1, 80, 45);

        bool pressed = handle_menu_nav(buttons, highlighted_button, retvalue);
        MouseState &detailmouse = query_mouse();
        bool do_click = false;

        if (leftmouse(buttons)) {
            do_click = true;
        }

        bool do_promote = (!buttons[1].hidden && ((do_click && (detailmouse.x >= 160) && (detailmouse.x <= 315) && (detailmouse.y >= 4) && (detailmouse.y <= 66)) || (pressed && (highlighted_button == 1))));

        if (do_promote) {
            if ((thisguy->family == FAMILY_MAGE) && (thisguy->get_level() >= 6)) {
                // Become an archmage!
                thisguy->upgrade_to_level(((thisguy->get_level() - 6) / 2) + 1);
                thisguy->family = FAMILY_ARCHMAGE;
                myscreen->soundp->play_sound(SOUND_EXPLODE);
                myscreen->soundp->play_sound(SOUND_EXPLODE);
                myscreen->soundp->play_sound(SOUND_EXPLODE);

                return REDRAW;
                // End of mage -> archmage
            } else if ((thisguy->family == FAMILY_ORC) && (thisguy->get_level() >= 5)) {
                // Become an Orcish Captain!
                thisguy->upgrade_to_level(1);
                // Fake for now
                thisguy->family = FAMILY_BIG_ORC;
                myscreen->soundp->play_sound(SOUND_DIE1);
                myscreen->soundp->play_sound(SOUND_DIE2);
                myscreen->soundp->play_sound(SOUND_DIE1);

                return REDRAW;
                // End of orc -> orc captain
            }
        }

        if (do_click) {
            retvalue = localbuttons->leftclick(buttons);
        }

        draw_backdrop();

        // Name box
        myscreen->draw_button(34, 8, 126, 24, 1, 1);
        myscreen->draw_text_bar(36, 10, 124, 22);

        Text &mytext = myscreen->text_normal;
        mytext.write_xy(80 - (mytext.query_width(current_guy->name) / 2), 14,
                        current_guy->name, DARK_BLUE, 1);
        myscreen->draw_dialog(5, 68, 315, 167, "Character Special Abilities");
        myscreen->draw_text_bar(160, 90, 162, 160);

        std::stringstream buf;

        // Text stuff, determined by character class & level
        switch (thisguy->family) {
        case FAMILY_SOLDIER:
            buf << "Level " << thisguy->get_level() << " soldier has:";
            mytext.write_xy(DETAIL_LM + 1, DETAIL_LD(0) + 1, buf.str(), 10, 1);
            mytext.write_xy(DETAIL_LM, DETAIL_LD(0), buf.str(), DARK_BLUE, 1);
            buf.clear();

            // Level 1 things (charge)
            WL(2, " Charge       ");
            WL(3, "  Charge causes you to ");
            WL(4, "  run forward, damaging");
            WL(5, "  anything in your way.");

            // Level 4 things (boomerang)
            if (thisguy->get_level() >= 4) {
                WL(7, " Boomerang");
                WL(8, "  The boomerang flies  ");
                WL(9, "  out in a spiral,     ");
                WL(10, "  hurting nearby foes. ");
            }

            // Level 7 things (whirl)
            if (thisguy->get_level() >= 7) {
                WR(0, " Whirl    ");
                WR(1, "  The fighter whirls in");
                WR(2, "  a spiral, hurting or ");
                WR(3, "  stunning melee foes. ");
            }

            // Level 10 things (disarm)
            if (thisguy->get_level() >= 10) {
                WR(5, " Disarm   ");
                WR(6, "  Cause a melee foe to");
                WR(7, "  temporarily lose the");
                WR(8, "  strength of attacks.");
            }

            break;
        case FAMILY_BARBARIAN:
            buf << "Level " << thisguy->get_level() << " barbarian has:";
            mytext.write_xy(DETAIL_LM + 1, DETAIL_LD(0) + 1, buf.str(), 10, 1);
            mytext.write_xy(DETAIL_LM, DETAIL_LD(0), buf.str(), DARK_BLUE, 1);
            buf.clear();

            // Level 1 things (hurl boulder)
            WL(2, " Hurl Boulder     ");
            WL(3, "  Throw a massive stone ");
            WL(4, "  boulder at your       ");
            WL(5, "  enemies.              ");

            // Level 4 things (exploding boulder)
            if (thisguy->get_level() >= 4) {
                WL(7, " Exploding Boulder");
                WL(8, "  Hurl a boulder so hard");
                WL(9, "  that it explodes and  ");
                WL(10, "  hits foes all around. ");
            }

            break;
        case FAMILY_ELF:
            buf << "Level " << thisguy->get_level() << " elf has:";
            mytext.write_xy(DETAIL_LM + 1, DETAIL_LD(0) + 1, buf.str(), 10, 1);
            mytext.write_xy(DETAIL_LM, DETAIL_LD(0), buf.str(), DARK_BLUE, 1);
            buf.clear();

            // Level 1 things (rocks)
            WL(2, " Rocks/Forestwalk");
            WL(3, "  Rocks hurls a few rocks");
            WL(4, "  at the enemy. Forest-  ");
            WL(5, "  walk, dexterity-based, ");
            WL(6, "  lets you move in trees.");

            // Level 4 things (more rocks)
            if (thisguy->get_level() >= 4) {
                WL(7, " More Rocks      ");
                WL(8, "  Like #1, but these     ");
                WL(9, "  rocks bounce off walls ");
                WL(10, "  and other barricades.  ");
            }

            // Level 7 things (lots of rocks)
            if (thisguy->get_level() >= 7) {
                WR(0, " Lots of Rocks   ");
                WR(1, "  Like #2, but more      ");
                WR(2, "  rocks, with a longer   ");
                WR(3, "  thrown range.          ");
            }

            // Level 10 things (Mega rocks)
            if (thisguy->get_level() >= 10) {
                WR(5, " MegaRocks       ");
                WR(6, "  This giant handful of  ");
                WR(7, "  rocks bounces far away ");
                WR(8, "  and packs a big punch. ");
            }

            break;
        case FAMILY_ARCHER:
            buf << "Level " << thisguy->get_level() << " archer has:";
            mytext.write_xy(DETAIL_LM + 1, DETAIL_LD(0) + 1, buf.str(), 10, 1);
            mytext.write_xy(DETAIL_LM, DETAIL_LD(0), buf.str(), DARK_BLUE, 1);
            buf.clear();

            // Level 1 things (fire arrows)
            WL(2, " Fire Arrows   ");
            WL(3, "  An archer can spin in a");
            WL(4, "  circle, firing off a   ");
            WL(5, "  ring of flaming bolts. ");

            // Level 4 things (barrage)
            if (thisguy->get_level() >= 4) {
                WL(7, " Barrage       ");
                WL(8, "  Rather than a single   ");
                WL(9, "  bolt, the archer sends ");
                WL(10, "  3 deadly bolts ahead. ");
            }

            // Level 7 things (exploding bolt)
            if (thisguy->get_level() >= 7) {
                WR(0, " Exploding Bolt");
                WR(1, "  This fatal bolt will   ");
                WR(2, "  explode on contact,    ");
                WR(3, "  dealing death to all.  ");
            }

            // Level 10 things
            if (thisguy->get_level() >= 10) {
                WR(5, "               ");
                WR(6, "                         ");
                WR(7, "                         ");
                WR(8, "                         ");
            }

            break;
        case FAMILY_MAGE:
            buf << "Level " << thisguy->get_level() << " mage has:";
            mytext.write_xy(DETAIL_LM + 1, DETAIL_LD(0) + 1, buf.str(), 10, 1);
            mytext.write_xy(DETAIL_LM, DETAIL_LD(0), buf.str(), DARK_BLUE, 1);

            // Level 1 things (teleport)
            WL(2, " Teleport/Marker");
            WL(3, "  Any mage can teleport  ");
            WL(4, "  randomly away easily,  ");
            WL(5, "  leaving a marker for   ");
            WL(6, "  anchor requires 75 int.");

            // Level 4 things (warp space)
            if (thisguy->get_level() >= 4) {
                WL(7, " Warp Space     ");
                WL(8, "  Twist the fabric of    ");
                WL(9, "  space around you to    ");
                WL(10, "  deal death to enemies. ");
            }

            // Can we change to Archmage?
            if (thisguy->get_level() >= 6) {
                buf << "Level " << ((thisguy->get_level() - 6) / 2) + 1 << " archmage. This";
                myscreen->draw_dialog(158, 4, 315, 66, "Become Archmage");
                WR(-10, "Your mage is now of high ");
                WR(-9, "enough level to become a ");
                WR(-8, buf.str());
                buf.clear();
                WR(-7, "change CANNOT by undone! ");
                WR(-6, " Click here to change.   ");
            }

            // Level 7 things (freeze time)
            if (thisguy->get_level() >= 7) {
                WR(0, " Freeze Time    ");
                WR(1, "  Freeze time for all    ");
                WR(2, "  but your team and kill ");
                WR(3, "  enemies with ease.     ");
            }

            // Level 10 things (energy wave)
            if (thisguy->get_level() >= 10) {
                WR(4, " Energy Wave    ");
                WR(5, "  Send a growing ripple  ");
                WR(6, "  of energy through      ");
                WR(7, "  walls and foes.        ");
            }

            // Level 13 things (heart burst)
            if (thisguy->get_level() >= 13) {
                WR(8, " HeartBurst     ");
                WR(9, "  Burst your enemies     ");
                WR(10, "  into flame. More magic");
                WR(11, "  means a bigger effect.");
            }

            break;
        case FAMILY_ARCHMAGE:
            buf << "Level " << thisguy->get_level() << " archmage has:";
            mytext.write_xy(DETAIL_LM + 1, DETAIL_LD(0) + 1, buf.str(), 10, 1);
            mytext.write_xy(DETAIL_LM, DETAIL_LD(0), buf.str(), DARK_BLUE, 1);
            buf.clear();

            // Level 1 things (Teleport)
            WL(2, " Teleport/Marker        ");
            WL(3, "  Any mage can teleport  ");
            WL(4, "  randomly away easily.  ");
            WL(5, "  Leaving a marker for   ");
            WL(6, "  anchor requires 75 int.");

            // Level 4 things (heart burst)
            if (thisguy->get_level() >= 4) {
                WL(7, " HeartBurst/Lightning   ");
                WL(8, "  Burst your enemies     ");
                WL(9, "  into flames around you.");
                WL(10, "  ALT: Chain lightning   ");
                WL(11, "  bounces through foes.  ");
            }

            // Level 7 things (summon image)
            if (thisguy->get_level() >= 7) {
                WR(0, " Summon Image/Sum. Elem.");
                WR(1, "  Summon an illusionary  ");
                WR(2, "  ally to fight for you. ");
                WR(3, "  ALT: Summon a daemon,  ");
                WR(4, "  who uses your stamina. ");
            }

            // Level 10 things (mind control)
            if (thisguy->get_level() >= 10) {
                WR(5, " Mind Control           ");
                WR(6, "  Convert nearby foes to ");
                WR(7, "  your team, for a time. ");
            }

            break;
        case FAMILY_CLERIC:
            buf << "Level " << thisguy->get_level() << " cleric has:";
            mytext.write_xy(DETAIL_LM + 1, DETAIL_LD(0) + 1, buf.str(), 10, 1);
            mytext.write_xy(DETAIL_LM, DETAIL_LD(0), buf.str(), DARK_BLUE, 1);
            buf.clear();

            // Level 1 things (heal)
            WL(2, " Heal             ");
            WL(3, "  Heal all teammates who ");
            WL(4, "  are close to you, for  ");
            WL(5, "  as much as you have SP.");

            // Level 4 things (raise undead)
            if (thisguy->get_level() >= 4) {
                WL(7, " Raise/Turn Undead");
                WL(8, "  Raise the gore of any  ");
                WL(9, "  victim to a skeleton.  ");
                WL(10, "  Alternate (turning)   ");
                WL(11, "  requires 65 int.      ");
            }

            // Level 7 things (raise ghost)
            if (thisguy->get_level() >= 7) {
                WR(0, " Raise/Turn Ghost ");
                WR(1, "  A more powerful raise, ");
                WR(2, "  you can now get ghosts ");
                WR(3, "  to fly and wail.       ");
            }

            // Level 10 things (resurrection)
            if (thisguy->get_level() >= 10) {
                WR(5, " Resurrection     ");
                WR(6, "  The ultimate healing,  ");
                WR(7, "  this restores dead     ");
                WR(8, "  friends to life, or    ");
                WR(9, "  enemies to undead.     ");
                WR(10, "  Beware: This will use  ");
                WR(11, "  your own EXP to cast!  ");
            }

            break;
        case FAMILY_DRUID:
            buf << "Level " << thisguy->get_level() << " druid has:";
            mytext.write_xy(DETAIL_LM + 1, DETAIL_LD(0) + 1, buf.str(), 10, 1);
            mytext.write_xy(DETAIL_LM, DETAIL_LD(0), buf.str(), DARK_BLUE, 1);

            // Level 1 things (plant tree)
            WL(2, " Plant Tree          ");
            WL(3, "  These magical trees   ");
            WL(4, "  will resist the enemy,");
            WL(5, "  while allowing friends");
            WL(6, "  to pass.              ");

            // Level 4 things (summon faerie)
            if (thisguy->get_level() >= 4) {
                WL(7, " Summon Faerie       ");
                WL(8, "  This spell brings to  ");
                WL(9, "  you a small flying    ");
                WL(10, "  faerie to stun foes.  ");
            }

            // Level 7 things (circle of protection)
            if (thisguy->get_level() >= 7) {
                WR(0, " Circle of Protection");
                WR(1, "  Calls the winds to aid");
                WR(2, "  your nearby friends by");
                WR(3, "  circling them with a  ");
                WR(4, "  shield of moving air. ");
            }

            // Level 10 things (reveal)
            if (thisguy->get_level() >= 10) {
                WR(5, " Reveal              ");
                WR(6, "  Gives you a magical   ");
                WR(7, "  view to see treasure, ");
                WR(8, "  potions, outposts, and");
                WR(9, "  invisible enemies.    ");
            }

            break;
        case FAMILY_THIEF:
            buf << "Level " << thisguy->get_level() << " thief has:";
            mytext.write_xy(DETAIL_LM + 1, DETAIL_LD(0) + 1, buf.str(), 10, 1);
            mytext.write_xy(DETAIL_LM, DETAIL_LD(0), buf.str(), DARK_BLUE, 1);
            buf.clear();

            // Level 1 things (drop bomb)
            WL(2, " Drop Bomb        ");
            WL(3, "  Leave a burning bomb to");
            WL(4, "  explode and hurt the   ");
            WL(5, "  unwary, friend or foe! ");

            // Level 4 things (cloak of darkness)
            if (thisguy->get_level() >= 4) {
                WL(7, " Cloak of Darkness");
                WL(8, "  Cloak yourself in the  ");
                WL(9, "  shadows, slipping past ");
                WL(10, "  your enemies.         ");
            }

            // Level 7 things (taunt enemies)
            if (thisguy->get_level() >= 7) {
                WR(0, " Taunt Enemies    ");
                WR(1, "  Beckon your enemies    ");
                WR(2, "  to you with jeers, and ");
                WR(3, "  confuse their attack.  ");
            }

            // Level 10 things (poison cloud)
            if (thisguy->get_level() >= 10) {
                WR(5, " Poison Cloud     ");
                WR(6, "  Release a cloud of     ");
                WR(7, "  poisonous gas to roam  ");
                WR(8, "  at will and sicken     ");
                WR(9, "  your foes.             ");
            }

            break;
        case FAMILY_ORC:
            buf << "Level " << thisguy->get_level() << " orc has:";
            mytext.write_xy(DETAIL_LM + 1, DETAIL_LD(0) + 1, buf.str(), 10, 1);
            mytext.write_xy(DETAIL_LM, DETAIL_LD(0), buf.str(), DARK_BLUE, 1);
            buf.clear();

            // Level 1 things (howl)
            WL(2, " Howl         ");
            WL(3, "  Howl in rage, stunning ");
            WL(4, "  nearby enemies in their");
            WL(5, "  tracks.                ");

            // Level 4 things (devour corpse)
            if (thisguy->get_level() >= 4) {
                WL(7, " Devour Corpse");
                WL(8, "  Regain health by       ");
                WL(9, "  devouring the corspes  ");
                WL(10, "  of your foes.          ");
            }

            // Can we change to orc captain?
            if (thisguy->get_level() >= 6) {
                myscreen->draw_dialog(158, 4, 315, 66, "Become Orc Captain");
                WR(-10, "Your orc is now of high ");
                WR(-9, "enough level to become a");
                WR(-8, "Level 1 orc captain. You");
                WR(-7, "CANNOT undo this action!");
                WR(-6, "Click here to change.   ");
            }

            // Level 7 things
            if (thisguy->get_level() >= 7) {
                WR(0, "              ");
            }

            // Level 10 things
            if (thisguy->get_level() >= 10) {
                WR(5, "              ");
            }

            break;
        default:

            break;
        }

        picker_show_guy(0, 1, 80, 45);

        draw_buttons(buttons, num_buttons);
        draw_highlight_interior(buttons[highlighted_button]);
        myscreen->buffer_to_screen(0, 0, 320, 200);
    }

    // Back to edit menu
    return REDRAW;
}

Sint32 get_scen_num_from_filename(std::string const &name)
{
    std::string::const_iterator itr = name.begin();

    while (isalpha(*itr)) {
        ++itr;
    }

    if (itr == name.end()) {
        return -1;
    } else {
        return std::stoi(std::string(itr, name.end()));
    }
}

Sint32 do_pick_campaign(Sint32 arg1)
{
    CampaignResult result = pick_campaign(&myscreen->save_data);

    if (result.id.string().size() > 0) {
        // Load new campaign
        myscreen->save_data.current_campaign = result.id;
        myscreen->save_data.scen_num = load_campaign(result.id, myscreen->save_data.current_levels, result.first_level);
    }

    return REDRAW;
}

Sint32 do_set_scen_level(Sint32 arg1)
{
    Sint32 templevel = myscreen->save_data.scen_num;
    templevel = pick_level(myscreen->level_data.id);

    // Have some feedack if the level changed
    if (templevel != myscreen->level_data.id)
    {
        Sint32 old_id = myscreen->level_data.id;
        myscreen->level_data.id = templevel;

        if ((templevel < 0) || !myscreen->level_data.load()) {
            myscreen->clearbuffer();
            popup_dialog("Load Failed", "Invalid level file.");
            myscreen->level_data.id = old_id;

            if (!myscreen->level_data.load()) {
                myscreen->clearbuffer();
                popup_dialog("Big problem", "Also failed to reload current level...");
            }
        } else {
            // We're good
            myscreen->save_data.scen_num = templevel;
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Set level to %d\n", templevel);
        }
    }

    return REDRAW;
}

Sint32 set_difficulty()
{
    std::stringstream buf;

    current_difficulty = (current_difficulty + 1) % DIFFICULTY_SETTINGS;
    buf << "Difficulty: " << difficulty_names[current_difficulty];

    std::string message(buf.str());
    buf.clear();
    message.resize(80);

#ifndef DISABLE_MULTIPLAYER
    allbuttons[6]->label = message;

#else

    allbuttons[2]->label = message;
#endif

    return OK;
}

Sint32 change_teamnum(Sint32 arg)
{
    // Change the team number of the current guy
    Sint16 current_team;
    std::stringstream buf;

    // What is our current team number?
    if (!current_guy) {
        return 0;
    }

    current_team = current_guy->teamnum;

    // We can be from team 0 (default) to team 3...make sure
    // we don't exceed this range.
    current_team += arg;
    current_team %= 4;

    // Set our team number...
    current_guy->teamnum = current_team;

    // Update our button display
    buf << "Playing on Team " << current_team + 1;
    std::string message(buf.str());
    buf.clear();
    message.resize(80);

    allbuttons[18]->label = message;

    return OK;
}

Sint32 change_hire_teamnum(Sint32 arg)
{
    // Change the team number of the hiring menu...
    std::stringstream buf;

    current_team_num += arg;
    current_team_num %= 4;

    // Change our guy, if he exists...
    if (current_guy) {
        current_guy->teamnum = current_team_num;
    }

    // Update our button display
    buf << "Hiring for Team " << current_team_num + 1;
    std::string message(buf.str());
    buf.clear();
    message.resize(80);

    allbuttons[2]->label = message;

    return OK;
}

Sint32 change_allied()
{
    // Change our allied mode (on or off)
    std::string message;

    myscreen->save_data.allied_mode += 1;
    myscreen->save_data.allied_mode %= 2;

    if (myscreen->save_data.allied_mode) {
        message = "PVP: Ally";
    } else {
        message = "PVP: Enemy";
    }

    // Update our button display;
    allbuttons[7]->label = message;

    return OK;
}
