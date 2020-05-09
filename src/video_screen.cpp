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
// screen.cpp

/*
 * Changelog:
 *     buffers: 07/31/02:
 *         * Delete some redundant headers.
 *         * load_scenario now looks for all uppercase files in levels.001 if
 *           lowercase file fails
 *     buffers: 08/15/02:
 *         * load_scenario now check for uppercase file names in scen/ in case
 *           lowercase check fails
 */
#include "video_screen.hpp"

#include "gloader.hpp"
#include "gparser.hpp"
#include "guy.hpp"
#include "input.hpp"
#include "io.hpp"
#include "pal32.hpp"
#include "results_screen.hpp"
#include "smooth.hpp"
#include "util.hpp"
#include "view.hpp"
#include "view_sizes.hpp"
#include "walker.hpp"

#include <cstring>
#include <filesystem>
#include <sstream>
#include <string>

// Screen window boundaries
#define MAX_VIEWS 5
#define S_UP 0 // 12 // 0
#define S_LEFT 0 // 12 // 0
#define S_DOWN 200 // 188 // 200
#define S_RIGHT 320 // 228
#define S_WIDTH (S_RIGHT - S_LEFT)
#define S_HEIGHT (S_DOWN - S_UP)

#define MAX_SPREAD 10 // This controls find_near_foe

Sint16 load_version_2(SDL_RWops *infile, VideoScreen *master);
Sint16 load_version_3(SDL_RWops *infile, VideoScreen *master); // Version 3 scen
Sint16 load_version_4(SDL_RWops *infile, VideoScreen *master); // Version 4 scen: + names
Sint16 load_version_5(SDL_RWops *infile, VideoScreen *master); // Version 5 scen: + type
Sint16 load_version_6(SDL_RWops *infile, VideoScreen *master, Sint16 version=6); // Version 6 scen: + title

VideoScreen *myscreen;

/*
 **********************************************************
 * SCREEN -- Graphics routines
 *
 * This object is the video graphics object. All display
 * must pass through this object, and all onscreen objects
 * are found in this object.
 **********************************************************
 */
VideoScreen::VideoScreen(Sint16 howmany)
    : Video()
    , level_data(1)
{
    // Set the global here so objects we construct here can use it
    myscreen = this;
    Sint32 i;
    Sint32 j;
    Text &first_text = text_normal;
    Sint32 left = 66;

    timerstart = query_timer_control();
    framecount = 0;

    control_hp = 0;

    // Load the palette...
    load_and_set_palette(newpalette);

    // Load the pixie graphics data into memory
    draw_button(60, 50, 260, 110, 2, 1);
    // Header field
    draw_text_bar(64, 54, 256, 62);
    first_text.write_y(56, "Loading Gladiator...Please Wait", RED, 1);
    // Draw box for text
    draw_text_bar(64, 64, 256, 106);

    first_text.write_xy(left, 70, "Loading Graphics...", DARK_BLUE, 1);
    buffer_to_screen(0, 0, 320, 200);
    // FIXME: Loader used to be created here...but now it's in level_data.
    first_text.write_xy(left, 70, "Loading Graphics...Done", DARK_BLUE, 1);
    first_text.write_xy(left, 78, "Loading Gameplay Info...", DARK_BLUE, 1);
    buffer_to_screen(0, 0, 320, 200);

    palmode = 0;
    end = 0;
    // 'moderate' speed setting
    timer_wait = 6;

    redrawme = 1;
    // Color cycling on by default
    cyclemode = 1;

    enemy_freeze = 0;
    level_done = 0;
    retry = false;

    first_text.write_xy(left, 78, "Loading Gameplay Info...Done", DARK_BLUE, 1);
    first_text.write_xy(left, 78, "Initializing Display...", DARK_BLUE, 1);
    buffer_to_screen(0, 0, 320, 200);

    // Set up the viewscreen poshorters
    // # of viewscreens
    numviews = howmany;

    for (i = 0; i < MAX_VIEWS; ++i) {
        viewob[i] = nullptr;
    }

    initialize_views();

    first_text.write_xy(left, 86, "Initializing Display...Done", DARK_BLUE, 1);
    first_text.write_xy(left, 94, "Initializing Sound...", DARK_BLUE, 1);
    buffer_to_screen(0, 0, 320, 200);

    // Init the sound data
    soundp = new SoundObject();

    if (!cfg.is_on("sound", "sound")) {
        soundp->set_sound(1);
    }

    first_text.write_xy(left, 94, "Initializing Sound...Done", DARK_BLUE, 1);

    buffer_to_screen(0, 0, 320, 200);

    // Let's set the special names for all walkers...
    for (i = 0; i < NUM_FAMILIES; ++i) {
        for (j = 0; j < NUM_SPECIALS; ++j) {
            special_name[i][j] = "NONE";
            alternate_name[i][j] = "NONE";
        }
    }

    special_name[FAMILY_SOLDIER][1] = "CHARGE";
    special_name[FAMILY_SOLDIER][2] = "BOOMERANG";
    special_name[FAMILY_SOLDIER][3] = "WHIRLWIND";
    special_name[FAMILY_SOLDIER][4] = "DISARM";

    special_name[FAMILY_BARBARIAN][1] = "HURL BOULDER";
    special_name[FAMILY_BARBARIAN][2] = "EXPLODING BOULDER";

    special_name[FAMILY_ELF][1] = "ROCKS";
    special_name[FAMILY_ELF][2] = "BOUNCING ROCKS";
    special_name[FAMILY_ELF][3] = "LOTS OF ROCKS";
    special_name[FAMILY_ELF][4] = "MEGA ROCKS";

    special_name[FAMILY_ARCHER][1] = "FIRE ARROWS";
    special_name[FAMILY_ARCHER][2] = "BARRAGE";
    special_name[FAMILY_ARCHER][3] = "EXPLODING BOLT";

    special_name[FAMILY_MAGE][1] = "TELEPORT";
    alternate_name[FAMILY_MAGE][1] = "TELEPORT MARKER";
    special_name[FAMILY_MAGE][2] = "WARP SPACE";
    special_name[FAMILY_MAGE][3] = "FREEZE TIME";
    special_name[FAMILY_MAGE][4] = "ENERGY WAVE";
    special_name[FAMILY_MAGE][5] = "HEARTBURST";

    special_name[FAMILY_ARCHMAGE][1] = "TELEPORT";
    alternate_name[FAMILY_ARCHMAGE][1] = "TELEPORT_MARKER";
    special_name[FAMILY_ARCHMAGE][2] = "HEARTBURST";
    alternate_name[FAMILY_ARCHMAGE][2] = "CHAIN LIGHTNING";
    special_name[FAMILY_ARCHMAGE][3] = "SUMMON IMAGE";
    alternate_name[FAMILY_ARCHMAGE][3] = "SUMMON ELEMENTAL";
    special_name[FAMILY_ARCHMAGE][4] = "MIND CONTROL";
    alternate_name[FAMILY_ARCHMAGE][4] = "SUMMON ELEMENTAL";

    special_name[FAMILY_CLERIC][1] = "HEAL";
    alternate_name[FAMILY_CLERIC][1] = "MYSTIC MACE";
    special_name[FAMILY_CLERIC][2] = "RAISE UNDEAD";
    alternate_name[FAMILY_CLERIC][2] = "TURN UNDEAD";
    special_name[FAMILY_CLERIC][3] = "RAISE_GHOST";
    alternate_name[FAMILY_CLERIC][3] = "TURN UNDEAD";
    special_name[FAMILY_CLERIC][4] = "RESURRECT";

    special_name[FAMILY_DRUID][1] = "GROW TREE";
    special_name[FAMILY_DRUID][2] = "SUMMON FAERIE";
    special_name[FAMILY_DRUID][3] = "REVEAL";
    special_name[FAMILY_DRUID][4] = "PROTECTION";

    special_name[FAMILY_THIEF][1] = "DROP BOMB";
    special_name[FAMILY_THIEF][2] = "CLOAK";
    special_name[FAMILY_THIEF][3] = "TAUNT ENEMY";
    alternate_name[FAMILY_THIEF][3] = "CHARM OPPONENT";
    special_name[FAMILY_THIEF][4] = "POISON CLOUD";

    special_name[FAMILY_GHOST][1] = "SCARE";

    special_name[FAMILY_FIRE_ELEMENTAL][1] = "STARBURST";

    special_name[FAMILY_ORC][1] = "HOWL";
    special_name[FAMILY_ORC][2] = "EAT CORPSE";

    special_name[FAMILY_SMALL_SLIME][1] = "GROW";

    special_name[FAMILY_MEDIUM_SLIME][1] = "GROW";

    special_name[FAMILY_SLIME][1] = "SPLIT";

    special_name[FAMILY_SKELETON][1] = "TUNNEL";
}

VideoScreen::~VideoScreen()
{
    delete soundp;

    soundp = nullptr;
    // Make sure we've cleaned up
    cleanup(1);
}

void VideoScreen::initialize_views()
{
    // Even though it looks okay here, these positions and sizes are overridden
    // by ViewScreen::resize() later
    if (numviews == 1) {
        viewob[0] = new ViewScreen(S_LEFT, S_UP, S_WIDTH, S_HEIGHT, 0);
    } else if (numviews == 2) {
        viewob[0] = new ViewScreen(T_LEFT_ONE, T_UP_ONE, T_HALF_WIDTH, T_HEIGHT, 0);
        viewob[1] = new ViewScreen(T_LEFT_TWO, T_UP_TWO, T_HALF_WIDTH, T_HEIGHT, 1);
    } else if (numviews == 3) {
        viewob[0] = new ViewScreen(T_LEFT_ONE, T_UP_ONE, T_HALF_WIDTH, T_HALF_HEIGHT, 0);
        viewob[1] = new ViewScreen(T_LEFT_TWO, T_UP_TWO, T_HALF_WIDTH, T_HALF_HEIGHT, 1);
        viewob[2] = new ViewScreen(T_LEFT_THREE, T_UP_THREE, T_HALF_WIDTH, T_HALF_HEIGHT, 2);
    } else if (numviews == 4) {
        viewob[0] = new ViewScreen(T_LEFT_ONE, T_UP_ONE, T_HALF_WIDTH, T_HALF_HEIGHT, 0);
        viewob[1] = new ViewScreen(T_LEFT_TWO, T_UP_TWO, T_HALF_WIDTH, T_HALF_HEIGHT, 1);
        viewob[2] = new ViewScreen(T_LEFT_THREE, T_UP_THREE, T_HALF_WIDTH, T_HALF_HEIGHT, 2);
        viewob[3] = new ViewScreen(T_LEFT_FOUR, T_UP_FOUR, T_HALF_WIDTH, T_HALF_HEIGHT, 3);
    } else {
        Log("Error initializing screen views. numviews is %d\n", numviews);
    }
}

void VideoScreen::cleanup(Sint16 howmany)
{
    Sint32 i;

    // # of viewscreens
    numviews = howmany;

    for (i = 0; i < MAX_VIEWS; ++i) {
        delete viewob[i];
        viewob[i] = nullptr;
    }
}

void VideoScreen::ready_for_battle(Sint16 howmany)
{
    // Set up the viewscreen poshorters
    // # of viewscreens
    numviews = howmany;

    // Clean stuff up
    cleanup(howmany);

    initialize_views();

    end = 0;
    retry = false;
    redrawme = 1;
    timerstart = query_timer_control();
    framecount = 0;
    enemy_freeze = 0;
    control_hp = 0;
    palmode = 0;
    redrawme = 1;
}

void VideoScreen::reset(Sint16 howmany)
{
    // Set up the viewscreen poshorters
    // # of viewscreens
    numviews = howmany;

    // Clean stuff up
    cleanup(howmany);

    if (numviews == 1) {
        viewob[0] = new ViewScreen(S_LEFT, S_UP, S_WIDTH, S_HEIGHT, 0);
    } else if (numviews == 2) {
        viewob[0] = new ViewScreen(T_LEFT_TWO, T_UP_TWO, T_WIDTH, T_HEIGHT, 0);
        viewob[1] = new ViewScreen(T_LEFT_ONE, T_UP_ONE, T_WIDTH, T_HEIGHT, 1);
    } else if (numviews == 3) {
        viewob[0] = new ViewScreen(T_LEFT_TWO, T_UP_TWO, T_WIDTH, T_HEIGHT, 0);
        viewob[1] = new ViewScreen(T_LEFT_ONE, T_UP_ONE, T_WIDTH, T_HEIGHT, 1);
        viewob[2] = new ViewScreen(112, 16, 100, 168, 2);
    } else if (numviews == 4) {
        viewob[0] = new ViewScreen(T_LEFT_TWO, T_UP_TWO, T_WIDTH, T_HEIGHT, 0);
        viewob[1] = new ViewScreen(T_LEFT_ONE, T_UP_ONE, T_WIDTH, T_HEIGHT, 1);
        viewob[2] = new ViewScreen(112, 16, 100, 168, 2);
        viewob[3] = new ViewScreen(112, 16, 100, 168, 3);
    }

    end = 0;
    redrawme = 1;
    save_data.reset();
    level_data.clear();
    timerstart = query_timer_control();
    framecount = 0;
    enemy_freeze = 0;
    control_hp = 0;
    palmode = 0;
    end = 0;
    redrawme = 1;
}

bool VideoScreen::query_grid_passable(float x, float y, Walker *ob)
{
    Sint32 i;
    Sint32 j;
    Sint32 xtrax = 1;
    Sint32 xtray = 1;
    Sint32 xtarg; // The for loop target
    Sint32 ytarg; // The for loop target
    Sint32 dist;
    Sint32 xover = x + ob->sizex;
    Sint32 yover = y + ob->sizey;

    if ((x < 0) || (y < 0) || (xover >= level_data.pixmaxx) || (yover >= level_data.pixmaxy)) {
        return 0;
    }

    // Are we ethereal?
    if (ob->stats.query_bit_flags(BIT_ETHEREAL)) {
        // Moved up to avoid unneeded calculation
        return 1;
    }

    // Zardus: PORT: Does the grid exist?
    if (!level_data.grid.valid()) {
        return 0;
    }

    // Check if our butt hangs over into next grid square
    if (xover % GRID_SIZE == 0) {
        // This should be the rare case
        xtrax = 0;
    }

    if (yover % GRID_SIZE == 0) {
        // This should be the rare case
        xtray = 0;
    }

    // Check grid squares by simulated grid coords.
    xtarg = (xover / GRID_SIZE) + xtrax;
    ytarg = (yover / GRID_SIZE) + xtray;

    for (i = (x / GRID_SIZE); i < xtarg; ++i) {
        for (j = (y / GRID_SIZE); j < ytarg; ++j) {
            // Check if item in background grid
            switch (level_data.grid.data[i + (level_data.grid.w * j)]) {
            case PIX_GRASS1: // Grass is pass...
            case PIX_GRASS2:
            case PIX_GRASS3:
            case PIX_GRASS4:
            case PIX_GRASS_DARK_1:
            case PIX_GRASS_DARK_2:
            case PIX_GRASS_DARK_3:
            case PIX_GRASS_DARK_4:
            case PIX_GRASS_DARK_LL:
            case PIX_GRASS_DARK_UR:
            case PIX_GRASS_DARK_B1: // Shadowed edges
            case PIX_GRASS_DARK_B2:
            case PIX_GRASS_DARK_BR:
            case PIX_GRASS_DARK_R1:
            case PIX_GRASS_DARK_R2:
            case PIX_GRASS_RUBBLE:
            case PIX_GRASS1_DAMAGED:
            case PIX_GRASS_LIGHT_1: // Lighter grass
            case PIX_GRASS_LIGHT_TOP:
            case PIX_GRASS_LIGHT_RIGHT_TOP:
            case PIX_GRASS_LIGHT_RIGHT:
            case PIX_GRASS_LIGHT_RIGHT_BOTTOM:
            case PIX_GRASS_LIGHT_BOTTOM:
            case PIX_GRASS_LIGHT_LEFT_BOTTOM:
            case PIX_GRASS_LIGHT_LEFT:
            case PIX_GRASS_LIGHT_LEFT_TOP:
            case PIX_GRASSWATER_LL: // Mostly gras
            case PIX_GRASSWATER_LR:
            case PIX_GRASSWATER_UL:
            case PIX_GRASSWATER_UR:
            case PIX_PAVEMENT1: // Floor ok
            case PIX_PAVEMENT2:
            case PIX_PAVEMENT3:
            case PIX_COBBLE_1: // Cobblestone
            case PIX_COBBLE_2:
            case PIX_COBBLE_3:
            case PIX_COBBLE_4:
            case PIX_FLOOR_PAVEL: // Wood/Tile ok
            case PIX_FLOOR_PAVER:
            case PIX_FLOOR_PAVEU:
            case PIX_FLOOR_PAVED:
            case PIX_PAVESTEPS1: // Steps
            case PIX_PAVESTEPS2:
            case PIX_PAVESTEPS2L:
            case PIX_PAVESTEPS2R:
            case PIX_FLOOR1:
            case PIX_CARPET_LL: // Carpet ok
            case PIX_CARPET_B:
            case PIX_CARPET_LR:
            case PIX_CARPET_UR:
            case PIX_CARPET_U:
            case PIX_CARPET_UL:
            case PIX_CARPET_L:
            case PIX_CARPET_M:
            case PIX_CARPET_M2:
            case PIX_CARPET_R:
            case PIX_CARPET_SMALL_HOR:
            case PIX_CARPET_SMALL_VER:
            case PIX_CARPET_SMALL_CUP:
            case PIX_CARPET_SMALL_CAP:
            case PIX_CARPET_SMALL_LEFT:
            case PIX_CARPET_SMALL_RIGHT:
            case PIX_CARPET_SMALL_TINY:
            case PIX_DIRT_1: // Dirt paths
            case PIX_DIRTGRASS_UL1:
            case PIX_DIRTGRASS_UR1:
            case PIX_DIRTGRASS_LL1:
            case PIX_DIRTGRASS_LR1:
            case PIX_DIRT_DARK_1: // Shadowed dirt/grass
            case PIX_DIRTGRASS_DARK_UL1:
            case PIX_DIRTGRASS_DARK_UR1:
            case PIX_DIRTGRASS_DARK_LL1:
            case PIX_DIRTGRASS_DARK_LR1:
            case PIX_PATH_1:
            case PIX_PATH_2:
            case PIX_PATH_3:
            case PIX_PATH_4:

                break;
            case PIX_TREE_M1: // Trees are usually bad, but we can fly over them
            case PIX_TREE_ML:
            case PIX_TREE_MR:
            case PIX_TREE_MT:
            case PIX_TREE_T1:
                if (ob->stats.query_bit_flags(BIT_FORESTWALK)) {
                    break;
                } else if (ob->stats.query_bit_flags(BIT_FLYING) || ob->flight_left) {
                    break;
                }

                return 0;
            case PIX_TREE_B1: // Tree bottoms
                if ((ob->query_order() == ORDER_WEAPON) || ob->stats.query_bit_flags(BIT_FORESTWALK)) {
                    break;
                } else if (ob->stats.query_bit_flags(BIT_FLYING) || ob->flight_left) {
                    break;
                }

                return 0;
            case PIX_H_WALL1: // Walls bad, but we can "ethereal" through them by default
            case PIX_WALL2:
            case PIX_WALL3:
            case PIX_WALL_LL:
            case PIX_WALLTOP_H:

                return 0;
            case PIX_WALL4: // Arrow slits
            case PIX_WALL5:
            case PIX_WALL_ARROW_GRASS:
            case PIX_WALL_ARROW_FLOOR:
            case PIX_WALL_ARROW_GRASS_DARK:
                if (ob->query_order() == ORDER_LIVING) {
                    return 0;
                }

                if (abs(ob->xpos - ob->owner->xpos) > abs(ob->ypos - ob->owner->ypos)) {
                    dist = abs(ob->xpos - ob->owner->xpos);
                } else {
                    dist = abs(ob->ypos - ob->owner->ypos);
                }

                dist -= (GRID_SIZE / 2);

                if (dist < GRID_SIZE) {
                    dist += GRID_SIZE;
                }

                if (getRandomSint32(dist / GRID_SIZE)) {
                    return 0;
                }
                // Drop through
            case PIX_WATER1: // Water
            case PIX_WATER2:
            case PIX_WATER3:
            case PIX_WATERGRASS_LL:
            case PIX_WATERGRASS_LR:
            case PIX_WATERGRASS_UL:
            case PIX_WATERGRASS_UR:
            case PIX_WATERGRASS_U:
            case PIX_WATERGRASS_L:
            case PIX_WATERGRASS_R:
            case PIX_WATERGRASS_D:
            case PIX_WALLSIDE_L: // Vertical walls
            case PIX_WALLSIDE1:
            case PIX_WALLSIDE_R:
            case PIX_WALLSIDE_C:
            case PIX_WALLSIDE_CRACK_C1:
            case PIX_TORCH1:
            case PIX_TORCH2:
            case PIX_TORCH3:
            case PIX_BRAZIER1: // Brazier
            case PIX_COLUMN1: // Columns
            case PIX_COLUMN2:
            case PIX_BOULDER_1: // Rocks
            case PIX_BOULDER_2:
            case PIX_BOULDER_3:
            case PIX_BOULDER_4:
                if (ob->query_order() == ORDER_WEAPON) {
                    break;
                } else if (ob->stats.query_bit_flags(BIT_FLYING) || ob->flight_left) {
                    break;
                }

                return 0;
            default:

                return 0;
            }
        }
    }

    return 1;
}

bool VideoScreen::query_object_passable(float x, float y, Walker *ob)
{
    if (ob->dead) {
        return true;
    }

    return level_data.myobmap->query_list(ob, x, y);
}

bool VideoScreen::query_passable(float x, float y, Walker *ob)
{
    return (query_grid_passable(x, y, ob) && query_object_passable(x, y, ob));
}

void VideoScreen::clear()
{
    Uint16 i;

     clearbuffer();

     for (i = 0; i < numviews; ++i) {
         viewob[i]->clear();
     }
}

// REDRAW -- This function moves through the data on the grid (map) finding
//           which grid squares are on screen. For each on screen, it pashorts
//           the appropriate graphics pixie onto the screen by calling the
//           function DRAW in PIXIE.
bool VideoScreen::redraw()
{
    Sint16 i;

    for (i = 0; i < numviews; ++i) {
        viewob[i]->redraw();
    }

    return true;
}

// REFRESH -- Refreshes the viewscreens
void VideoScreen::refresh()
{
    Sint16 i;

    for (i = 0; i < numviews; ++i) {
        viewob[i]->refresh();
    }
}

// *******************
// Useful stuff again
// *******************

bool VideoScreen::input(SDL_Event const &event)
{
    Sint16 i;

    for (i = 0; i < numviews; ++i) {
        viewob[i]->input(event);
    }

    return true;
}

bool VideoScreen::continuous_input()
{
    Sint16 i;

    for (i = 0; i < numviews; ++i) {
        viewob[i]->continuous_input();
    }

    return true;
}

bool VideoScreen::act()
{
    // Have we printed message yet?
    Sint32 printed_time = 0;

    level_done = 2; // Unless we find valid foes while looping

    if (enemy_freeze) {
        --enemy_freeze;
    }

    if (enemy_freeze == 1) {
        set_palette(ourpalette);
    }

    for (auto const & ob : level_data.oblist) {
        // Normal functionality
        if (!enemy_freeze) {
            if (ob && !ob->dead) {
                // Zardus: While acting, in_act is set
                ob->in_act = 1;
                ob->act();
                ob->in_act = 0;

                if (ob && !ob->dead) {
                    if (!ob->is_friendly_to_team(save_data.my_team) && (ob->query_order() == ORDER_LIVING)) {
                        level_done = 0;
                    }

                    // Testing...Trying to FORCE foes :)
                    if ((ob->foe == nullptr) && (ob->leader == nullptr)) {
                        ob->foe = myscreen->find_far_foe(ob);
                    }
                }
            }
        } else {
            // Enemy livings are frozen
            if ((enemy_freeze % 10 == 0) && !printed_time) {
                std::stringstream buf("TIME LEFT: ");
                buf << enemy_freeze;
                viewob[0]->set_display_text(buf.str(), 10);
                printed_time = 1;
            }

            if (ob && !ob->dead
                && (((ob->query_order() != ORDER_LIVING)
                        && (ob->query_order() != ORDER_GENERATOR))
                    || (ob->team_num == 0))) {
                ob->act();

                if (ob && !ob->dead) {
                    if (!ob->is_friendly_to_team(save_data.my_team) && (ob->query_order() == ORDER_LIVING)) {
                        level_done = 0;
                    }
                }
            }
        }
    }

    // Let the weapons act...
    for (auto const & ob : level_data.weaplist) {
        if (ob && !ob->dead) {
            ob->act();

            if (ob && !ob->dead) {
                if (!ob->is_friendly_to_team(save_data.my_team) && (ob->query_order() == ORDER_LIVING)) {
                    level_done = 0;
                }
            }
        }
    } // End of weapons acting

    // Quickly check the background for exits, etc.
    for (auto const & ob : level_data.fxlist) {
        if (ob && !ob->dead) {
            if ((ob->query_order() == ORDER_TREASURE)
                && (ob->query_family() == FAMILY_EXIT)
                && (level_done != 0)) {
                // 0 => foes, 1 => no foes but exit, 2 => no foes or exit
                level_done = 1;
            }
        }
    }

    if (level_done == 2) {
        // No exit and no enemies: Go to next sequential level
        return endgame(0, level_data.id + 1);
    }

    if (end) {
        return true;
    }

    // Make sure we're all pointing to legal targets
    for (auto & ob : level_data.oblist) {
        if (ob->foe && ob->foe->dead) {
            ob->foe = nullptr;
        }

        if (ob->leader && ob->leader->dead) {
            ob->leader = nullptr;
        }

        if (ob->owner && ob->owner->dead) {
            ob->owner = nullptr;
        }

        if (ob->collide_ob && ob->collide_ob->dead) {
            ob->collide_ob = nullptr;
        }
    }

    for (auto & ob : level_data.weaplist) {
        if (ob->foe && ob->foe->dead) {
            ob->foe = nullptr;
        }

        if (ob->leader && ob->leader->dead) {
            ob->leader = nullptr;
        }

        if (ob->owner && ob->owner->dead) {
            ob->owner = nullptr;
        }

        if (ob->collide_ob && ob->collide_ob->dead) {
            ob->collide_ob = nullptr;
        }
    }

    // Remove dead objects
    auto obj = level_data.oblist.begin();

    while (obj != level_data.oblist.end()) {
        Walker *ob = *obj;

        if (ob && ob->dead && (ob->myguy == nullptr)) {
            // Delete the dead thing safely

            // Is it a player?
            if (ob->user != -1) {
                // Remove it from its viewscreen
                for (Sint32 i = 0; i < numviews; ++i) {
                    if (ob == viewob[i]->control) {
                        viewob[i]->control = nullptr;
                    }
                }
            }

            // Save dead guys to be delete later. Delete everything else right
            // now. This is so the "owner" of weapons remains valid.
            level_data.dead_list.push_back(ob);

            // level_data.remove_ob();

            // Remove from the list directory here so we can preserve our iterator
            if (ob->query_order() == ORDER_LIVING) {
                --level_data.numobs;
            }

            obj = level_data.oblist.erase(obj);
        } else {
            ++obj;
        }
    }

    auto fx = level_data.fxlist.begin();

    while (fx != level_data.fxlist.end()) {
        Walker *ob = *fx;

        if (ob && ob->dead) {
            delete ob;
            fx = level_data.fxlist.erase(fx);
        } else {
            ++fx;
        }
    }

    auto weap = level_data.weaplist.begin();

    while (weap != level_data.weaplist.end()) {
        Walker *ob = *weap;

        if (ob && ob->dead) {
            delete ob;
            weap = level_data.weaplist.erase(weap);
        } else {
            ++weap;
        }
    }

    return true;
}

bool VideoScreen::endgame(Sint16 ending)
{
    return endgame(ending, -1);
}

bool VideoScreen::endgame(Sint16 ending, Sint16 nextlevel)
{
    if (end) {
        return true;
    }

    std::map<Sint32, Guy *> before;
    std::map<Sint32, Walker *> after;

    // Get guys from before battle
    for (Sint32 i = 0; i < save_data.team_size; ++i) {
        if (save_data.team_list[i] != nullptr) {
            before.insert(std::make_pair(save_data.team_list[i]->id, save_data.team_list[i]));
        }
    }

    // Get guys from the battle
    for (auto const & ob : level_data.oblist) {
        if (ob && ob->myguy) {
            after.insert(std::make_pair(ob->myguy->id, ob));
        }
    }

    // Let's show the results!
    retry = results_screen(ending, nextlevel, before, after);

    if (retry) {
        // Retry without updating the roster and saving the game
        end = 1;

        return true;
    }

    // 1 = lose, for some reason
    if (ending == 1) {
        // Generic defeat
        if (nextlevel == -1) {
            end = 1;
        } else {
            // we're withdrawing to another level
            end = 1;
        }
    } else if (ending == SCEN_TYPE_SAVE_ALL) {
        // Failed to save a guy
        end = 1;
    } else if (ending == 0) {
        // We won
        Uint32 bonuscash[4] = { 0, 0, 0, 0 };
        Uint32 allbonuscash = 0;

        // Update all the money!
        for (Sint32 i = 0; i < 4; ++i) {
            save_data.m_totalscore[i] += save_data.m_score[i];
            save_data.m_totalcash[i] += (save_data.m_score[i] * 2);
        }

        for (Sint32 i = 0; i < 4; ++i) {
            bonuscash[i] = get_time_bonus(i);
            save_data.m_totalcash[i] += bonuscash[i];
            allbonuscash += bonuscash[i];
        }

        // Already won, no bonus
        if (save_data.is_level_completed(save_data.scen_num)) {
            for (Sint32 i = 0; i < 4; ++i) {
                bonuscash[i] = 0;
            }

            allbonuscash = 0;
        }

        // Beat that level
        // This scenario is completed...
        save_data.add_level_completed(save_data.current_campaign, save_data.scen_num);

        if (nextlevel != -1) {
            // Fake jumping to next level...
            save_data.scen_num = nextlevel;
        }

        // Grab our team out of the level
        save_data.update_guys(level_data.oblist);

        // Autosave because we won
        save_data.save("save0");

        end = 1;
    }

    return true;
}

Walker *VideoScreen::find_near_foe(Walker *ob)
{
    Sint16 targx;
    Sint16 targy;
    Sint16 spread = 1;
    Sint16 xchange = 0;
    Sint16 loop = 0;
    Sint16 resolution = level_data.myobmap->obmapres;

    if (!ob) {
        Log("No ob in find near foe.\n");

        return nullptr;
    }

    targx = ob->xpos;
    targy = ob->ypos;
    spread = 1;

    while (spread < MAX_SPREAD) {
        for (loop = 0; loop < spread; ++loop) {
            if (xchange % 2 == 0) {
                // changex is 0 or a multiple of 2
                targx += resolution;

                if (targx <= 0) {
                    // Left edge of screen
                    return find_far_foe(ob);
                }

                if (targx >= level_data.pixmaxx) {
                    // Right edge of screen
                    return find_far_foe(ob);
                }
            } else {
                // changex is odd
                targy += resolution;

                if (targy <= 0) {
                    // Top of screen
                    return find_far_foe(ob);
                }

                if (targy >= level_data.pixmaxy) {
                    // Bottom of screen
                    return find_far_foe(ob);
                }
            }

            std::list<Walker *> &ls = level_data.myobmap->obmap_get_list(targx, targy);

            // Go through the list we received
            for (auto const & w : ls) {
                if (!w->dead && (ob->is_friendly(w) == 0) && (getRandomSint32(w->invisibility_left / 20) == 0)) {
                    if ((w->query_order() == ORDER_LIVING) || (w->query_order() == ORDER_GENERATOR)) {
                        // Done separately since they are logically more signficant
                        // This should be a valid foe
                        return w;
                    }
                }
            } // End inner while
        } // End for

        // Change whether we do x or y in each loop
        ++xchange;

        if (xchange % 2 == 0) {
            // Reverse directrion around the search every other for
            // Increase the search width every other for
            ++spread;
        }
    } // End while

    // Failure
    return find_far_foe(ob);
}

Walker *VideoScreen::find_far_foe(Walker *ob)
{
    Sint32 distance;
    Sint32 tempdistance;
    Walker *endfoe;

    if (!ob) {
        Log("No ob in find far foe.\n");

        return nullptr;
    }

    // Set our "default" foe to nullptr
    endfoe = nullptr;
    distance = 10000;
    ob->stats.last_distance = 10000;

    for (auto const & foe : level_data.oblist) {
        if ((foe == nullptr) || foe->dead) {
            continue;
        }

        // Check for valid objects...
        if (ob->is_friendly(foe) == 0) {
            if (((foe->query_order() == ORDER_LIVING) || (foe->query_order() == ORDER_GENERATOR)) && (getRandomSint32(foe->invisibility_left / 20) == 0)) {
                tempdistance = ob->distance_to_ob(foe);

                if (tempdistance < distance) {
                    distance = tempdistance;
                    endfoe = foe;
                }
            }
        }
    }

    return endfoe;
}

std::string VideoScreen::get_scen_title(std::string const &filename, VideoScreen *master)
{
    SDL_RWops *infile = nullptr;
    char temptext[10] = "XXX";
    std::string tempfile(filename);
    tempfile.append(".fss");
    Uint8 versionnumber = 0;
    char buffer[30];

    // Zardus: First get the file from scen/
    infile = open_read_file(std::filesystem::path("scen/" + tempfile));
    if (infile == nullptr) {
        return "none";
    }

    // Are we a scenario file?
    SDL_RWread(infile, temptext, 3, 1);

    if (strcmp(temptext, "FSS")) {
        return "none";
    }

    // Check the version number
    SDL_RWread(infile, &versionnumber, 1, 1);

    if (versionnumber < 6) {
        return "none";
    }

    // Discard the grid name...
    SDL_RWread(infile, buffer, 8, 1);

    // Return the title, 30 bytes
    SDL_RWread(infile, buffer, 30, 1);

    if (infile) {
        SDL_RWclose(infile);
    }

    return std::string(buffer);
}

// Look for the first non-dead instance of a given walker...
Walker *VideoScreen::first_of(Uint8 whatorder, Uint8 whatfamily, Sint32 team_num)
{
    for (auto const & ob : level_data.oblist) {
        if (ob && !ob->dead) {
            if ((ob->query_order() == whatorder) && (ob->query_family() == whatfamily)) {
                if ((team_num == -1) || (team_num == ob->team_num)) {
                    return ob;
                }
            }
        }
    }

    return nullptr;
}

void VideoScreen::draw_panels(Sint16 howmany)
{
    Sint16 i;

    // Force a memory clear...
    clearbuffer();

    for (i = 0; i < numviews; ++i) {
        if ((viewob[i]->prefs[PREF_VIEW] != PREF_VIEW_FULL) && (numviews != 4)) {
            draw_button(viewob[i]->xloc - 4, viewob[i]->yloc - 3, viewob[i]->endx + 3, viewob[i]->endy + 3, 3, 1);
            draw_box(viewob[i]->xloc - 1, viewob[i]->yloc - 1, viewob[i]->endx, viewob[i]->endy, 0, 0, 1);
        }
    }

    // Repaint the screen area...
    redraw();
    buffer_to_screen(0, 0, 320, 200);
}

// This can be slow, so don't call it much
Walker *VideoScreen::find_nearest_blood(Walker *who)
{
    Sint32 distance;
    Sint32 newdistance;
    Walker *returnob = nullptr;

    if (!who) {
        return nullptr;
    }

    distance = 800;

    for (auto const & w : level_data.fxlist) {
        if (w && (w->query_order() == ORDER_TREASURE) && (w->query_family() == FAMILY_STAIN) && !w->dead) {
            newdistance = static_cast<Uint32>(who->distance_to_ob_center(w));

            if (newdistance < distance) {
                distance = newdistance;
                returnob = w;
            }
        }
    }

    return returnob;
}

std::list<Walker *> VideoScreen::find_in_range(std::list<Walker *> &somelist, Sint32 range, Sint16 *howmany, Walker *ob)
{
    std::list<Walker *> result;

    *howmany = 0;

    if (!ob) {
        return result;
    }

    for (auto const & w : somelist) {
        if (w && !w->dead) {
            if (ob->distance_to_ob(w) <= range) {
                result.push_back(w);
                ++(*howmany);
            }
        }
    }

    return result;
}

Walker *VideoScreen::find_nearest_player(Walker *ob)
{
    Walker *returnob = nullptr;
    Uint32 distance = 32000;
    Uint32 tempdistance;

    if (!ob) {
        return nullptr;
    }

    for (auto const & w : level_data.oblist) {
        if (w && (w->user != -1)) {
            tempdistance = ob->distance_to_ob(w);

            if (tempdistance < distance) {
                distance = tempdistance;
                returnob = w;
            }
        }
    }

    return returnob;
}

std::list<Walker *> VideoScreen::find_foes_in_range(std::list<Walker *> &somelist, Sint32 range, Sint16 *howmany, Walker *ob)
{
    std::list<Walker *> result;
    *howmany = 0;

    if (!ob) {
        return result;
    }

    for (auto const & w : somelist) {
        if (w && !w->dead && ((w->query_order() == ORDER_LIVING) || (w->query_order() == ORDER_GENERATOR)) && (ob->is_friendly(w) == 0)) {
            if (ob->distance_to_ob(w) <= range) {
                result.push_back(w);
                ++(*howmany);
            }
        }
    }

    return result;
}

std::list<Walker *> VideoScreen::find_friends_in_range(std::list<Walker *> & somelist, Sint32 range, Sint16 *howmany, Walker *ob)
{
    std::list<Walker *> result;
    *howmany = 0;

    if (!ob) {
        return result;
    }

    for (auto const & w : somelist) {
        if (w && !w->dead && (w->query_order() == ORDER_LIVING) && ob->is_friendly(w)) {
            if (ob->distance_to_ob(w) <= range) {
                result.push_back(w);
                ++(*howmany);
            }
        }
    }

    return result;
}

std::list<Walker *> VideoScreen::find_foe_weapons_in_range(std::list<Walker *> & somelist, Sint32 range, Sint16 *howmany, Walker *ob)
{
    std::list<Walker *> result;
    *howmany = 0;

    if (!ob) {
        return result;
    }

    for (auto const & w : somelist) {
        if (w && !w->dead && (w->query_order() == ORDER_WEAPON) && ob->is_friendly(w)) {
            if (ob->distance_to_ob(w) <= range) {
                result.push_back(w);
                ++(*howmany);
            }
        }
    }

    return result;
}

// Uses pixel coordinates
// Damage the specified tile
Uint8 VideoScreen::damage_tile(Sint16 xloc, Sint16 yloc)
{
    Sint16 xover;
    Sint16 yover;
    Sint16 gridloc;

    xover = xloc / GRID_SIZE;
    yover = yloc / GRID_SIZE;

    if ((xover < 0) || (yover < 0)) {
        return 0;
    }

    if ((xover >= level_data.grid.w) || (yover >= level_data.grid.h)) {
        return 0;
    }

    gridloc = (yover * level_data.grid.w) + xover;

    switch (level_data.grid.data[gridloc]) {
    case PIX_GRASS1: // Grass
    case PIX_GRASS2:
    case PIX_GRASS3:
    case PIX_GRASS4:
        level_data.grid.data[gridloc] = PIX_GRASS1_DAMAGED;

        break;
    default:

        break;
    }

    return level_data.grid.data[gridloc];
}

void VideoScreen::do_notify(std::string const &message, Walker *who)
{
    Sint16 i;
    bool sent = false;

    for (i = 0; i < numviews; ++i) {
        if (who && (viewob[i]->control == who)) {
            viewob[i]->set_display_text(message, STANDARD_TEXT_TIME);
            sent = true;
        }
    }

    if (!sent) {
        for (i = 0; i < numviews; ++i) {
            viewob[i]->set_display_text(message, STANDARD_TEXT_TIME);
        }
    }
}

void VideoScreen::report_mem()
{

    struct meminfo {
        Uint32 LargeestblockAvail;
        Uint32 MaxUnlockedPage;
        Uint32 LargestLockablePage;
        Uint32 LinAddrSpace;
        Uint32 NumFreePagesAvail;
        Uint32 NumPhysicalPagesFree;
        Uint32 TotalPhysicalPages;
        Uint32 FreeLinAddrSpace;
        Uint32 SizeOfPageFile;
        Uint32 Reserved[3];
    } Memory;

    Memory.FreeLinAddrSpace = 0;
    std::stringstream memreport;

    memreport << "Free Linear address: " << Memory.FreeLinAddrSpace << " pages";

    viewob[0]->set_display_text(memreport.str(), 25);
}
