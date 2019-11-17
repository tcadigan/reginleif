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
#ifndef __LEVEL_EDITOR_FWD_HPP__
#define __LEVEL_EDITOR_FWD_HPP__

#include "base.hpp"
#include "object_type.hpp"
#include "pixdefs.hpp"

#include <SDL2/SDL.h>

#include <vector>

enum EditModeEnum : Uint8 {
    TERRAIN,
    OBJECT,
    SELECT
};

Sint32 redraw = 1; // Need to redraw?
Sint32 levelchanged = 0; // Has level changed?
Sint32 campaignchanged = 0; // Has campaign changed?
Sint32 rowsdown = 0;

Sint32 backgrounds[] = {
    PIX_GRASS1, PIX_GRASS2, PIX_GRASS_DARK_1, PIX_GRASS_DARK_2,
    // PIX_GRASS_DARK_B1, PIX_GRASS_DARK_BR, PIX_GRASS_DARK_R1, PIX_GRASS_DARK_R2,
    PIX_BOULDER_1, PIX_GRASS_DARK_LL, PIX_GRASS_DARK_UR, PIX_GRASS_RUBBLE,
    PIX_GRASS_LIGHT_LEFT_TOP, PIX_GRASS_LIGHT_1,
    PIX_GRASS_LIGHT_RIGHT_TOP, PIX_WATER1,
    PIX_WATERGRASS_U, PIX_WATERGRASS_D,
    PIX_WATERGRASS_L, PIX_WATERGRASS_R,
    PIX_DIRTGRASS_UR1, PIX_DIRT_1, PIX_DIRT_1, PIX_DIRTGRASS_LL1,
    PIX_DIRTGRASS_LR1, PIX_DIRT_DARK_1, PIX_DIRT_DARK_1, PIX_DIRTGRASS_UL1,
    PIX_DIRTGRASS_DARK_UR1, PIX_DIRTGRASS_DARK_LL1,
    PIX_DIRTGRASS_DARK_LR1, PIX_DIRTGRASS_DARK_UL1,
    PIX_JAGGED_GROUND_1, PIX_JAGGED_GROUND_2,
    PIX_JAGGED_GROUND_3, PIX_JAGGED_GROUND_4,
    PIX_PATH_1, PIX_PATH_2, PIX_PATH_3, PIX_PATH_4,
    PIX_COBBLE_1, PIX_COBBLE_2, PIX_COBBLE_3, PIX_COBBLE_4,
    // PIX_WALL2, PIX_WALL3, PIX_WALL4, PIX_WALL5,
    PIX_WALL4, PIX_WALL_ARROW_GRASS,
    PIX_WALL_ARROW_FLOOR, PIX_WALL_ARROW_GRASS_DARK,
    PIX_WALL2, PIX_WALL3, PIX_H_WALL1, PIX_WALL_LL,
    PIX_WALLSIDE_L, PIX_WALLSIDE_C, PIX_WALLSIDE_R, PIX_WALLSIDE1,
    PIX_WALLSIDE_CRACK_C1, PIX_WALLSIDE_CRACK_C1,
    PIX_TORCH1, PIX_VOID1,
    // PIX_VOID1, PIX_FLOOR1, PIX_VOID1, PIX_VOID1,
    PIX_CARPET_SMALL_TINY, PIX_CARPET_M2, PIX_PAVEMENT1, PIX_FLOOR1,
    // PIX_PAVEMENT1, PIX_PAVEMENT2, PIX_PAVEMENT3, PIX_PAVEMENT3,
    PIX_FLOOR_PAVEL, PIX_FLOOR_PAVEU, PIX_FLOOR_PAVED, PIX_FLOOR_PAVED,
    PIX_WALL_LL,
    PIX_WALLTOP_H,
    PIX_PAVESTEPS1,
    PIX_BRAZIER1,
    PIX_PAVESTEPS2L, PIX_PAVESTEPS2, PIX_PAVESTEPS2R, PIX_PAVESTEPS1,
    // PIX_TORCH1, PIX_TORCH2, PIX_TORCH3, PIX_TORCH3,
    PIX_COLUMN1, PIX_COLUMN2, PIX_COLUMN2, PIX_COLUMN2,
    PIX_TREE_T1, PIX_TREE_T1, PIX_TREE_T1, PIX_TREE_T1,
    PIX_TREE_ML, PIX_TREE_M1, PIX_TREE_MT, PIX_TREE_MR,
    PIX_TREE_B1, PIX_TREE_B1, PIX_TREE_B1, PIX_TREE_B1,
    PIX_CLIFF_BACK_L, PIX_CLIFF_BACK_1, PIX_CLIFF_BACK_2, PIX_CLIFF_BACK_R,
    PIX_CLIFF_LEFT, PIX_CLIFF_BOTTOM, PIX_CLIFF_TOP, PIX_CLIFF_RIGHT,
    PIX_CLIFF_LEFT, PIX_CLIFF_TOP_L, PIX_CLIFF_TOP_R, PIX_CLIFF_RIGHT
};

Sint32 maxrows = (sizeof(backgrounds) / 4) / 4;

Sint32 mouse_last_x = 0;
Sint32 mouse_last_y = 0;

std::vector<ObjectType> object_pane;

#define S_UP 1
#define S_RIGHT 245

#define PIX_TOP (S_UP + 79)
#define PIX_DOWN 4
#define PIX_BOTTOM (PIX_TOP + (PIX_DOWN * GRID_SIZE))

#endif
