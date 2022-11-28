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
#ifndef __PIXDEFS_HPP__
#define __PIXDEFS_HPP__

#include <SDL2/SDL.h>

//
// pixdefs.h
//
// Defines values for the loader, etc. to know where to put things in the array.
//

enum PixEnum : Uint8 {
    PIX_H_WALL1 = 0,
    PIX_GRASS1 = 1,
    PIX_WATER1 = 2,
    PIX_VOID1 = 3,
    PIX_WALL2 = 4,
    PIX_WALL3 = 5,
    PIX_FLOOR1 = 6,
    PIX_WALL4 = 7,
    PIX_WALL5 = 8,
    PIX_CARPET_LL = 9,
    PIX_CARPET_L = 10,
    PIX_CARPET_B = 11,
    PIX_CARPET_LR = 12,
    PIX_CARPET_UR = 13,
    PIX_CARPET_U = 14,
    PIX_CARPET_UL = 15,

// More grass...reorder when all use this defs file
    PIX_GRASS2 = 16,
    PIX_GRASS3 = 17,
    PIX_GRASS4 = 18,

    PIX_WATER2 = 19,
    PIX_WATER3 = 20,

    PIX_PAVEMENT1 = 21,

    PIX_WALLSIDE1 = 22,
    PIX_WALLSIDE_L = 23,
    PIX_WALLSIDE_R = 24,
    PIX_WALLSIDE_C = 25,

    PIX_WALL_LL = 26,

    PIX_PAVESTEPS1 = 27,

    PIX_BRAZIER1 = 28,

    PIX_WATERGRASS_LL = 29,
    PIX_WATERGRASS_LR = 30,
    PIX_WATERGRASS_UL = 31,
    PIX_WATERGRASS_UR = 32,

    PIX_CARPET_M = 33,
    PIX_CARPET_M2 = 34,

    PIX_PAVESTEPS2 = 35,
    PIX_PAVESTEPS2L = 36,
    PIX_PAVESTEPS2R = 37,

    PIX_WALLTOP_H = 38,

    PIX_TORCH1 = 39,
    PIX_TORCH2 = 40,
    PIX_TORCH3 = 41,

    PIX_CARPET_R = 42,

    PIX_FLOOR_PAVEL = 43,
    PIX_FLOOR_PAVER = 44,
    PIX_FLOOR_PAVEU = 45,
    PIX_FLOOR_PAVED = 46,

    PIX_GRASSWATER_LL = 47,
    PIX_GRASSWATER_LR = 48,
    PIX_GRASSWATER_UL = 49,
    PIX_GRASSWATER_UR = 50,

    PIX_PAVEMENT2 = 51,
    PIX_PAVEMENT3 = 52,

    PIX_COLUMN1 = 53,
    PIX_COLUMN2 = 54,

// These are trees
    PIX_TREE_B1 = 55,
    PIX_TREE_M1 = 56,
    PIX_TREE_T1 = 57,
    PIX_TREE_ML = 58, // Left of center

    PIX_DIRT_1 = 59,

    PIX_DIRTGRASS_UL1 = 60,
    PIX_DIRTGRASS_UR1 = 61,
    PIX_DIRTGRASS_LL1 = 62,
    PIX_DIRTGRASS_LR1 = 63,

    PIX_PATH_1 = 64,
    PIX_PATH_2 = 65,
    PIX_PATH_3 = 66,

    PIX_BOULDER_1 = 67,

    PIX_WATERGRASS_U = 68,
    PIX_WATERGRASS_L = 69,
    PIX_WATERGRASS_R = 70,
    PIX_WATERGRASS_D = 71,

    PIX_COBBLE_1 = 72,
    PIX_COBBLE_2 = 73,

    PIX_PATH_4 = 74,

    PIX_COBBLE_3 = 75,
    PIX_COBBLE_4 = 76,

    PIX_WALL_ARROW_GRASS = 77,
    PIX_WALL_ARROW_FLOOR = 78,

// Damaged tiles
    PIX_GRASS1_DAMAGED = 79,

    PIX_TREE_MR = 80, // Right of center
    PIX_TREE_MT = 81, // Thin

    PIX_GRASS_DARK_1 = 82,
    PIX_GRASS_DARK_LL = 83,
    PIX_GRASS_DARK_UR = 84,
    PIX_GRASS_RUBBLE = 85,
    PIX_GRASS_DARK_2 = 86,
    PIX_GRASS_DARK_3 = 87,
    PIX_GRASS_DARK_4 = 88,

    PIX_BOULDER_2 = 89,
    PIX_BOULDER_3 = 90,
    PIX_BOULDER_4 = 91,

    PIX_GRASS_DARK_B1 = 92, // Bottom "fuzzy" edges
    PIX_GRASS_DARK_B2 = 93,
    PIX_GRASS_DARK_BR = 94, // Bottom right fuzzy
    PIX_GRASS_DARK_R1 = 95, // Right fuzzy
    PIX_GRASS_DARK_R2 = 96,

    PIX_WALL_ARROW_GRASS_DARK = 97,

    PIX_DIRTGRASS_DARK_UL1 = 98,
    PIX_DIRTGRASS_DARK_UR1 = 99,
    PIX_DIRTGRASS_DARK_LL1 = 100,
    PIX_DIRTGRASS_DARK_LR1 = 101,

    PIX_WALLSIDE_CRACK_C1 = 102,

    PIX_DIRT_DARK_1 = 103,

    PIX_GRASS_LIGHT_1 = 104, // Lighter grass
    PIX_GRASS_LIGHT_TOP = 105,
    PIX_GRASS_LIGHT_RIGHT_TOP = 106,
    PIX_GRASS_LIGHT_RIGHT = 107,
    PIX_GRASS_LIGHT_RIGHT_BOTTOM = 108,
    PIX_GRASS_LIGHT_BOTTOM = 109,
    PIX_GRASS_LIGHT_LEFT_BOTTOM = 110,
    PIX_GRASS_LIGHT_LEFT = 111,
    PIX_GRASS_LIGHT_LEFT_TOP = 112,

// Cliff tiles
    PIX_CLIFF_BOTTOM = 113,
    PIX_CLIFF_TOP = 114,
    PIX_CLIFF_LEFT = 115,
    PIX_CLIFF_RIGHT = 116,
    PIX_CLIFF_BACK_1 = 117,
    PIX_CLIFF_BACK_2 = 118,
    PIX_CLIFF_BACK_L = 119,
    PIX_CLIFF_BACK_R = 120,
    PIX_CLIFF_TOP_L = 121,
    PIX_CLIFF_TOP_R = 122,

// Pete's graphics...
    PIX_JAGGED_GROUND_1 = 123,
    PIX_JAGGED_GROUND_2 = 124,
    PIX_JAGGED_GROUND_3 = 125,
    PIX_JAGGED_GROUND_4 = 126,

// The "small" carpet pieces
    PIX_CARPET_SMALL_HOR = 127,
    PIX_CARPET_SMALL_VER = 128,
    PIX_CARPET_SMALL_CUP = 129,
    PIX_CARPET_SMALL_CAP = 130,
    PIX_CARPET_SMALL_LEFT = 131,
    PIX_CARPET_SMALL_RIGHT = 132,
    PIX_CARPET_SMALL_TINY = 133,

// This should be the largest defined pix + 1
    PIX_MAX = 134 // Last currently = PIX_CARPET_SMALL_TINY
};

#endif
