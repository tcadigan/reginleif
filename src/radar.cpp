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
// radar.cpp

/*
 * Changelog
 *     buffers: 07/32/02:
 *         * Include cleanup
 */
#include "radar.hpp"

#include "colors.hpp"
#include "level_data.hpp"
#include "screen.hpp"
#include "util.hpp"
#include "walker.hpp"

// These are the dimensions of the radar viewport
#define RADAR_X 60
#define RADAR_Y 44

/*
 * ********************************************************
 * RADAR -- It's nothing like pixie, it just looks like it
 * ********************************************************
 *
 * Radar(Uint8, Sint16, Sint16, Screen) - Initializes the radar data (pix = char)
 * bool draw()
 * bool on_screen()
 */

/*
 * Radar -- This initializes the graphics data for the radar,
 * as well as its graphics x and y size. In addition, it informs
 * the radar of the screen object it is linked to.
 */
Radar::Radar(Sint16 whatnum)
{
    // What number viewscreen we are, to get control's position
    mynum = whatnum;
    bmp = nullptr;
    force_lower_position = false;
}

void Radar::start(LevelData const &data, Sint16 viewscreen_endx, Sint16 viewscreen_endy, Sint16 viewscreen_yloc)
{
    sizex = static_cast<Uint16>(data.grid.w);
    sizey = static_cast<Uint16>(data.grid.h);
    size = static_cast<Uint16>(static_cast<Uint16>(sizex) * static_cast<Uint16>(sizey));
    xview = RADAR_X;
    yview = RADAR_Y;
    radarx = 0;
    radary = 0;

    xview = std::min(xview, sizex);
    yview = std::min(yview, sizey);

#ifdef REDUCE_OVERSCAN
    // Used by level editor to place minimap
    if (force_lower_position) {
        // At bottom
        xloc = static_cast<Sint16>((viewscreen_endx - xview) - 4);
        yloc = static_cast<Sint16>((viewscreen_endy - yview) - 4);
    } else {
        // At top
        xloc = static_cast<Sint16>((viewscreen_endx - xview) - 4);
        yloc = static_cast<Sint16>(viewscreen_yloc + 4);
    }
    // At bottom
    xloc = static_cast<Sint16>((viewscreen_endx - xview) - 8);
    yloc = static_cast<Sint16>((viewscreen_endy - yview) - 8);
#else
    xloc = static_cast<Sint16>((viewscreen_endx - xview) - 4);
    yloc = static_cast<Sint16>((viewscreen_endy - yview) - 4);
#endif

    if (bmp != nullptr) {
        delete[] bmp;
    }

    bmp = new Uint8[size];
    update(data);
}

// Destruct the radar and its variables
Radar::~Radar()
{
    if (bmp) {
        delete[] bmp;
        bmp = nullptr;
    }
}

bool Radar::draw(LevelData const &data, Walker *control)
{
    Sint32 tempx;
    Sint32 tempy;
    Sint32 tempz;
    Uint8 tempcolor;
    Sint16 oborder;
    Sint16 obfamily;
    Sint16 obteam;
    Sint16 can_see = 0;
    Sint16 do_show = 0;
    Sint32 listtype = 0;

    radarx = 0;
    radary = 0;

    if (control != nullptr) {
        radarx = static_cast<Sint16>((control->xpos / GRID_SIZE) - (xview / 2));
        radary = static_cast<Sint16>((control->ypos / GRID_SIZE) - (yview / 2));

        if (control->view_all > 0) {
            can_see = 1;
        }

        obteam = control->team_num;
    } else {
        radarx = static_cast<Sint16>((data.topx / GRID_SIZE) - (xview / 2));
        radary = static_cast<Sint16>((data.topy / GRID_SIZE) - (yview / 2));
        obteam = 0;
    }

    radarx = std::max(std::min(radarx, static_cast<Sint16>(sizex - xview)), static_cast<Sint16>(0));
    radary = std::max(std::min(radary, static_cast<Sint16>(sizey - yview)), static_cast<Sint16>(0));

    Uint8 alpha = 255;

    if ((myscreen->numviews > 2) && ((myscreen->numviews != 3) || (mynum != 0))) {
        alpha = 127;
    }

    myscreen->putbuffer_alpha(xloc, yloc, sizex, sizey, xloc, yloc, xloc + xview, yloc + yview, &bmp[radarx + (radary * sizex)], alpha);

    // Now determine what objects are visible on the radar...
    while (listtype <= 1) {
        std::list<Walker *> const *ls;

        // Do oblist, standard
        if (listtype == 0) {
            ls = &data.oblist;
            ++listtype;
        } else if (listtype == 1) {
            // Do weapons
            ls = &data.weaplist;
            ++listtype;
        } else {
            continue;
        }

        for (auto const &ob : *ls) {
            oborder = ob->query_order();

            // Don't show, by default
            do_show = 0;

            if (((oborder == ORDER_LIVING)
                 || (oborder == ORDER_WEAPON)
                 || ((oborder == ORDER_TREASURE) && (ob->query_family() == FAMILY_LIFE_GEM))
                 || ((oborder == ORDER_TREASURE) && (ob->query_family() == FAMILY_EXIT))
                 || ((oborder == ORDER_GENERATOR) && can_see))
                && ((obteam == ob->team_num) || (ob->invisibility_left < 1) || can_see)
                && on_screen(static_cast<Sint16>((ob->xpos + 1) / GRID_SIZE), static_cast<Sint16>((ob->ypos + 1) / GRID_SIZE), radarx, radary)) {
                do_show = 1;
            }

            if (do_show) {
                tempx = xloc + (((ob->xpos + 1) / GRID_SIZE) - radarx);
                tempy = yloc + (((ob->ypos + 1) / GRID_SIZE) - radary);

                if (((tempx < xloc) || (tempx > (xloc + xview)))
                    || ((tempy < yloc) || (tempy > (yloc + yview)))) {
                    // Do nothing
                } else {
                    // This may need fixing
                    tempz = tempx + (tempy * 320);

                    if ((tempz > 64000) || (tempz < 0)) {
                        Log("bad radar, bad\n");

                        return true;
                    }

                    tempcolor = ob->query_team_color();

                    if (control == ob) {
                        tempcolor = static_cast<Uint8>(getRandomSint32(256));

                        if ((tempx >= ((xloc + xview) - 1)) && (tempy < (yloc + yview))) {
                            myscreen->pointb(tempx - 1, tempy, tempcolor, alpha);
                            myscreen->pointb(tempx, tempy, tempcolor, alpha);
                            myscreen->pointb(tempx - 1, tempy + 1, tempcolor, alpha);
                            myscreen->pointb(tempx, tempy + 1, tempcolor, alpha);
                        } else if (tempx >= ((xloc + xview) - 1)) {
                            myscreen->pointb(tempx, tempy, tempcolor, alpha);
                            myscreen->pointb(tempx - 1, tempy, tempcolor, alpha);
                            myscreen->pointb(tempx, tempy - 1, tempcolor, alpha);
                            myscreen->pointb(tempx - 1, tempy - 1, tempcolor, alpha);
                        } else if ((tempy >= ((yloc + yview) - 1)) && (tempx < (xloc + xview))) {
                            myscreen->pointb(tempx, tempy, tempcolor, alpha);
                            myscreen->pointb(tempx + 1, tempy, tempcolor, alpha);
                            myscreen->pointb(tempx, tempy - 1, tempcolor, alpha);
                            myscreen->pointb(tempx + 1, tempy - 1, tempcolor, alpha);
                        } else {
                            myscreen->pointb(tempx, tempy, tempcolor, alpha);
                            myscreen->pointb(tempx + 1, tempy, tempcolor, alpha);
                            myscreen->pointb(tempx, tempy + 1, tempcolor, alpha);
                            myscreen->pointb(tempx + 1, tempy + 1, tempcolor, alpha);
                        }
                    } else if (oborder == ORDER_LIVING) {
                        myscreen->pointb(tempx, tempy, tempcolor, alpha);
                    } else if (oborder == ORDER_GENERATOR) {
                        myscreen->pointb(tempx, tempy, static_cast<Sint16>(tempcolor + 1), alpha);
                    } else if (oborder == ORDER_TREASURE) {
                        // Currently life gems
                        myscreen->pointb(tempx, tempy, COLOR_FIRE, alpha);
                    } else {
                        myscreen->pointb(tempx, tempy, COLOR_WHITE, alpha);
                    }
                } // Draw the blob onto the radar
            }
        }
    } // Go back to new screen lists (weapons, etc.)

    for (auto const &ob : data.fxlist) {
        if (ob && !ob->dead) {
            oborder = ob->query_order();
            obfamily = ob->query_family();

            // Don't show, by default
            do_show = 0;

            if (oborder == ORDER_TREASURE) {
                if (can_see) {
                    switch (obfamily) {
                    case FAMILY_GOLD_BAR:
                        do_show = static_cast<Sint16>(YELLOW + getRandomSint32(5));

                        break;
                    case FAMILY_SILVER_BAR:
                        do_show = static_cast<Sint16>(GREY + getRandomSint32(5));

                        break;
                    case FAMILY_DRUMSTICK:
                        do_show = static_cast<Sint16>(COLOR_BROWN + getRandomSint32(2));

                        break;
                    case FAMILY_MAGIC_POTION:
                    case FAMILY_INVIS_POTION:
                    case FAMILY_INVULNERABLE_POTION:
                    case FAMILY_FLIGHT_POTION:
                        do_show = static_cast<Sint16>(COLOR_BLUE + getRandomSint32(5));

                        break;
                    default:
                        do_show = 0;

                        break;
                    }
                }

                if ((obfamily == FAMILY_EXIT) || (obfamily == FAMILY_TELEPORTER)) {
                    do_show = static_cast<Sint16>(LIGHT_BLUE + getRandomSint32(7));
                }
            }

            if (!on_screen(static_cast<Sint16>((ob->xpos + 1) / GRID_SIZE), static_cast<Sint16>((ob->ypos + 1) / GRID_SIZE), radarx, radary)) {
                do_show = 0;
            }

            if (do_show) {
                tempx = xloc + (((ob->xpos + 1) / GRID_SIZE) - radarx);
                tempy = yloc + (((ob->ypos + 1) / GRID_SIZE) - radary);

                if (((tempx < xloc) || (tempx > (xloc + xview)))
                    || ((tempy < yloc) || (tempy > (yloc + yview)))) {
                    // Do nothing
                } else {
                    // This may need fixing
                    tempz = tempx + (tempy * 320);

                    if ((tempz > 64000) || (tempz < 0)) {
                        Log("bad radar, bad\n");

                        return true;
                    }

                    myscreen->pointb(tempx, tempy, static_cast<Sint8>(do_show), alpha);
                } // Draw the blob onto the radar
            } // End of valid do_show
        } // Endo of if here->ob
    } // End of while (here)

    return true;
}

/*
 * bool Radar::refresh()
 * {
 *     // The first two avalues are screwy... I don't know why
 *     myscreen->buffer_to_screen(xloc, yloc, xview, yview);
 *
 *     return true;
 * }
 *
 * In theory the above function will NOT be required
 */

bool Radar::on_screen(Sint16 whatx, Sint16 whaty, Sint16 hor, Sint16 ver)
{
    // Return 0 if off radar.
    // These measurements are grid coords, not pixels
    if ((whatx < hor) || (whatx >= (hor + xview)) || (whaty < ver) || (whaty >= (ver + yview))) {
        return false;
    } else {
        return true;
    }
}

// This function re-initializes the radar map data. Do not call it often, as it
// is very slow...
void Radar::update(LevelData const &data)
{
    Sint16 temp;

    for (Sint16 i = 0; i < sizex; ++i) {
        for (Sint16 j = 0; j < sizey; ++j) {
            // Check if item in background grid
            switch (static_cast<Uint8>(data.grid.data[i + (sizex * j)])) {
            case PIX_GRASS1: // Grass is green
            case PIX_GRASS_DARK_1:
            case PIX_GRASS_DARK_B1:
            case PIX_GRASS_DARK_BR:
                temp = COLOR_GREEN + 3;

                break;
            case PIX_GRASS2:
            case PIX_GRASS_DARK_2:
            case PIX_GRASS_DARK_B2:
            case PIX_WALL_ARROW_GRASS:
                temp = COLOR_GREEN + 4;

                break;
            case PIX_GRASS3:
            case PIX_GRASS_DARK_3:
            case PIX_GRASS_DARK_R1:
            case PIX_WALL_ARROW_GRASS_DARK:
                temp = COLOR_GREEN + 5;

                break;
            case PIX_GRASS4:
            case PIX_GRASS_DARK_4:
            case PIX_GRASS_DARK_R2:
                temp = COLOR_GREEN + 5;

                break;
            case PIX_GRASS_DARK_LL:
            case PIX_GRASS_DARK_UR:
            case PIX_GRASS_RUBBLE:
            case PIX_GRASS_LIGHT_1: // Lighter grass
            case PIX_GRASS_LIGHT_TOP:
            case PIX_GRASS_LIGHT_RIGHT_TOP:
            case PIX_GRASS_LIGHT_RIGHT:
            case PIX_GRASS_LIGHT_RIGHT_BOTTOM:
            case PIX_GRASS_LIGHT_BOTTOM:
            case PIX_GRASS_LIGHT_LEFT_BOTTOM:
            case PIX_GRASS_LIGHT_LEFT:
            case PIX_GRASS_LIGHT_LEFT_TOP:
                temp = static_cast<Sint16>((COLOR_GREEN + getRandomSint32(3)) + 3);

                break;
            case PIX_TREE_M1: // Trees are green
            case PIX_TREE_ML:
            case PIX_TREE_T1:
            case PIX_TREE_MR:
            case PIX_TREE_MT:
                temp = static_cast<Sint16>(COLOR_TREES + getRandomSint32(3));

                break;
            case PIX_TREE_B1: // Trunks are brown
                temp = COLOR_BROWN + 6;

                break;
            case PIX_PAVEMENT1: // Pavement dark grey
            case PIX_PAVEMENT2:
            case PIX_PAVEMENT3:
            case PIX_PAVESTEPS1:
            case PIX_PAVESTEPS2:
            case PIX_PAVESTEPS2L:
            case PIX_PAVESTEPS2R:
            case PIX_COBBLE_1:
            case PIX_COBBLE_2:
            case PIX_COBBLE_3:
            case PIX_COBBLE_4:
                temp = 17;

                break;
            case PIX_FLOOR_PAVEL: // Wood is brown
            case PIX_FLOOR_PAVER:
            case PIX_FLOOR_PAVEU:
            case PIX_FLOOR_PAVED:
            case PIX_FLOOR1:
            case PIX_WALL_ARROW_FLOOR:
                temp = COLOR_BROWN + 4;

                break;
            case PIX_DIRT_1: // Path is brown
            case PIX_DIRTGRASS_UL1:
            case PIX_DIRTGRASS_UR1:
            case PIX_DIRTGRASS_LL1:
            case PIX_DIRTGRASS_LR1:
            case PIX_DIRT_DARK_1:
            case PIX_DIRTGRASS_DARK_UL1:
            case PIX_DIRTGRASS_DARK_UR1:
            case PIX_DIRTGRASS_DARK_LL1:
            case PIX_DIRTGRASS_DARK_LR1:
                temp = COLOR_BROWN + 5;

                break;
            case PIX_JAGGED_GROUND_1:
            case PIX_JAGGED_GROUND_2:
            case PIX_JAGGED_GROUND_3:
            case PIX_JAGGED_GROUND_4:
                temp = COLOR_BROWN + 5;

                break;
            case PIX_CLIFF_BOTTOM: // Slightly darker
            case PIX_CLIFF_TOP:
            case PIX_CLIFF_LEFT:
            case PIX_CLIFF_RIGHT:
            case PIX_CLIFF_BACK_1:
            case PIX_CLIFF_BACK_2:
            case PIX_CLIFF_BACK_L:
            case PIX_CLIFF_BACK_R:
            case PIX_CLIFF_TOP_L:
            case PIX_CLIFF_TOP_R:
                temp = COLOR_BROWN + 6;

                break;
            case PIX_CARPET_LL: // Carpet is purple
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
                temp = COLOR_PURPLE + 4;

                break;
            case PIX_H_WALL1: // Walls are light grey
            case PIX_WALL2:
            case PIX_WALL3:
            case PIX_WALL_LL:
            case PIX_WALLTOP_H:
            case PIX_WALL4:
            case PIX_WALL5:
            case PIX_BOULDER_1:
            case PIX_BOULDER_2:
            case PIX_BOULDER_3:
            case PIX_BOULDER_4:
            case PIX_PATH_1: // Sparser cobblestone/grass
            case PIX_PATH_2:
            case PIX_PATH_3:
            case PIX_PATH_4:
                temp = 24;

                break;
            case PIX_WATER1: // Water is dark blue
            case PIX_WATER2:
            case PIX_WATER3:
            case PIX_WATERGRASS_LL:
            case PIX_WATERGRASS_LR:
            case PIX_WATERGRASS_UL:
            case PIX_WATERGRASS_UR:
            case PIX_GRASSWATER_LL:
            case PIX_GRASSWATER_LR:
            case PIX_GRASSWATER_UL:
            case PIX_GRASSWATER_UR:
                temp = COLOR_BLUE + 2;

                break;
            case PIX_WALLSIDE_L: // White, maybe?
            case PIX_WALLSIDE1:
            case PIX_WALLSIDE_R:
            case PIX_WALLSIDE_C:
            case PIX_WALLSIDE_CRACK_C1:
                temp = COLOR_WHITE - 1;

                break;
            case PIX_TORCH1:
            case PIX_TORCH2:
            case PIX_TORCH3:
            case PIX_BRAZIER1:
                temp = COLOR_FIRE;

                break;
            default:
                temp = 0;
            }

            bmp[i + (sizex * j)] = static_cast<Uint8>(temp);
        }
    }
}
