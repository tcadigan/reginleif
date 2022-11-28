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
// graphlib.cpp

/*
 * Changelog
 *     buffers: 07/31/02: Include cleanup
 *     buffers: 08/15/02: Rewrote the file finding/loading code in read_pixie_file
 */
#include "graphlib.hpp"

#include <filesystem>
#include <string>
#include <sstream>

#include "io.hpp"
#include "pixdefs.hpp"
#include "util.hpp"

void load_map_data(PixieData *whereto)
{
    // Load the pixie graphics data into memory
    whereto[0] = PixieData(std::filesystem::path("16tile.pix"));
    whereto[PIX_GRASS1] = PixieData(std::filesystem::path("16grass1.pix"));
    whereto[PIX_WATER1] = PixieData(std::filesystem::path("16water1.pix"));
    whereto[3] = PixieData(std::filesystem::path("16space.pix"));
    whereto[4] = PixieData(std::filesystem::path("16wall2.pix"));
    whereto[5] = PixieData(std::filesystem::path("16wall3.pix"));
    whereto[6] = PixieData(std::filesystem::path("16floor.pix"));
    whereto[7] = PixieData(std::filesystem::path("16walllo.pix"));
    whereto[8] = PixieData(std::filesystem::path("16w2lo.pix"));

    whereto[9] = PixieData(std::filesystem::path("16carpll.pix"));
    whereto[10] = PixieData(std::filesystem::path("16carpb.pix"));
    whereto[11] = PixieData(std::filesystem::path("16carplr.pix"));
    whereto[12] = PixieData(std::filesystem::path("16carpur.pix"));
    whereto[13] = PixieData(std::filesystem::path("16carpu.pix"));
    whereto[14] = PixieData(std::filesystem::path("16carpul.pix"));
    whereto[PIX_CARPET_L] = PixieData(std::filesystem::path("16carpl.pix"));
    whereto[PIX_CARPET_M] = PixieData(std::filesystem::path("16carpm.pix"));
    whereto[PIX_CARPET_M2] = PixieData(std::filesystem::path("16carpm2.pix"));
    whereto[PIX_CARPET_R] = PixieData(std::filesystem::path("16carpr.pix"));

    whereto[PIX_CARPET_SMALL_HOR] = PixieData(std::filesystem::path("16cshor.pix"));
    whereto[PIX_CARPET_SMALL_VER] = PixieData(std::filesystem::path("16csver.pix"));
    whereto[PIX_CARPET_SMALL_CUP] = PixieData(std::filesystem::path("16cscup.pix"));
    whereto[PIX_CARPET_SMALL_CAP] = PixieData(std::filesystem::path("16cscap.pix"));
    whereto[PIX_CARPET_SMALL_LEFT] = PixieData(std::filesystem::path("16csleft.pix"));
    whereto[PIX_CARPET_SMALL_RIGHT] = PixieData(std::filesystem::path("16csright.pix"));
    whereto[PIX_CARPET_SMALL_TINY] = PixieData(std::filesystem::path("16cstiny.pix"));

    whereto[PIX_GRASS2] = PixieData(std::filesystem::path("16grass2.pix"));
    whereto[PIX_GRASS3] = PixieData(std::filesystem::path("16grass3.pix"));
    whereto[PIX_GRASS4] = PixieData(std::filesystem::path("16grass4.pix"));

    whereto[PIX_GRASS_DARK_1] = PixieData(std::filesystem::path("16grassd.pix"));
    whereto[PIX_GRASS_DARK_2] = PixieData(std::filesystem::path("16grd2.pix"));
    whereto[PIX_GRASS_DARK_3] = PixieData(std::filesystem::path("16grd3.pix"));
    whereto[PIX_GRASS_DARK_4] = PixieData(std::filesystem::path("16grd4.pix"));
    whereto[PIX_GRASS_DARK_LL] = PixieData(std::filesystem::path("16grassi.pix"));
    whereto[PIX_GRASS_DARK_UR] = PixieData(std::filesystem::path("16grassh.pix"));
    whereto[PIX_GRASS_RUBBLE] = PixieData(std::filesystem::path("16grassr.pix"));

    whereto[PIX_GRASS_DARK_B1] = PixieData(std::filesystem::path("16grdb1.pix"));
    whereto[PIX_GRASS_DARK_B2] = PixieData(std::filesystem::path("16grdb2.pix"));
    whereto[PIX_GRASS_DARK_R1] = PixieData(std::filesystem::path("16grdr1.pix"));
    whereto[PIX_GRASS_DARK_R2] = PixieData(std::filesystem::path("16grdr2.pix"));
    whereto[PIX_GRASS_DARK_BR] = PixieData(std::filesystem::path("16grdbr.pix"));

    whereto[PIX_GRASS_LIGHT_1] = PixieData(std::filesystem::path("16grl1.pix"));
    whereto[PIX_GRASS_LIGHT_TOP] = PixieData(std::filesystem::path("16grlt.pix"));
    whereto[PIX_GRASS_LIGHT_RIGHT_TOP] = PixieData(std::filesystem::path("16grlrt.pix"));
    whereto[PIX_GRASS_LIGHT_RIGHT] = PixieData(std::filesystem::path("16grlr.pix"));
    whereto[PIX_GRASS_LIGHT_RIGHT_BOTTOM] = PixieData(std::filesystem::path("16grlrb.pix"));
    whereto[PIX_GRASS_LIGHT_BOTTOM] = PixieData(std::filesystem::path("16grlb.pix"));
    whereto[PIX_GRASS_LIGHT_LEFT_BOTTOM] = PixieData(std::filesystem::path("16grllb.pix"));
    whereto[PIX_GRASS_LIGHT_LEFT] = PixieData(std::filesystem::path("16grll.pix"));
    whereto[PIX_GRASS_LIGHT_LEFT_TOP] = PixieData(std::filesystem::path("16grllt.pix"));

    whereto[PIX_WATER2] = PixieData(std::filesystem::path("16water2.pix"));
    whereto[PIX_WATER3] = PixieData(std::filesystem::path("16water3.pix"));

    whereto[PIX_WATERGRASS_LL] = PixieData(std::filesystem::path("16wgll.pix"));
    whereto[PIX_WATERGRASS_LR] = PixieData(std::filesystem::path("16wglr.pix"));
    whereto[PIX_WATERGRASS_UL] = PixieData(std::filesystem::path("16wgul.pix"));
    whereto[PIX_WATERGRASS_UR] = PixieData(std::filesystem::path("16wgur.pix"));
    whereto[PIX_WATERGRASS_U] = PixieData(std::filesystem::path("16wgu.pix"));
    whereto[PIX_WATERGRASS_D] = PixieData(std::filesystem::path("16wgd.pix"));
    whereto[PIX_WATERGRASS_L] = PixieData(std::filesystem::path("16wgl.pix"));
    whereto[PIX_WATERGRASS_R] = PixieData(std::filesystem::path("16wgr.pix"));

    whereto[PIX_GRASSWATER_LL] = PixieData(std::filesystem::path("16gwll.pix"));
    whereto[PIX_GRASSWATER_LR] = PixieData(std::filesystem::path("16gwlr.pix"));
    whereto[PIX_GRASSWATER_UL] = PixieData(std::filesystem::path("16gwul.pix"));
    whereto[PIX_GRASSWATER_UR] = PixieData(std::filesystem::path("16gwur.pix"));

    whereto[PIX_PAVEMENT1] = PixieData(std::filesystem::path("16pave1.pix"));
    whereto[PIX_PAVEMENT2] = PixieData(std::filesystem::path("16pave2.pix"));
    whereto[PIX_PAVEMENT3] = PixieData(std::filesystem::path("16pave3.pix"));

    whereto[PIX_PAVESTEPS1] = PixieData(std::filesystem::path("16pstep.pix"));
    whereto[PIX_PAVESTEPS2] = PixieData(std::filesystem::path("16pstest.pix"));
    whereto[PIX_PAVESTEPS2L] = PixieData(std::filesystem::path("16ptestl.pix"));
    whereto[PIX_PAVESTEPS2R] = PixieData(std::filesystem::path("16ptestr.pix"));

    whereto[PIX_WALLSIDE1] = PixieData(std::filesystem::path("16brick1.pix"));
    whereto[PIX_WALLSIDE_L] = PixieData(std::filesystem::path("16brickl.pix"));
    whereto[PIX_WALLSIDE_R] = PixieData(std::filesystem::path("16brickr.pix"));
    whereto[PIX_WALLSIDE_C] = PixieData(std::filesystem::path("16brickc.pix"));
    whereto[PIX_WALLSIDE_CRACK_C1] = PixieData(std::filesystem::path("16brick3.pix"));

    whereto[PIX_WALL_LL] = PixieData(std::filesystem::path("16wallll.pix"));

    whereto[PIX_BRAZIER1] = PixieData(std::filesystem::path("16braz1.pix"));

    whereto[PIX_WALLTOP_H] = PixieData(std::filesystem::path("16ttop.pix"));

    whereto[PIX_TORCH1] = PixieData(std::filesystem::path("16torch1.pix"));
    whereto[PIX_TORCH2] = PixieData(std::filesystem::path("16torch2.pix"));
    whereto[PIX_TORCH3] = PixieData(std::filesystem::path("16torch3.pix"));

    whereto[PIX_FLOOR_PAVEL] = PixieData(std::filesystem::path("16fpl.pix"));
    whereto[PIX_FLOOR_PAVER] = PixieData(std::filesystem::path("16fpr.pix"));
    whereto[PIX_FLOOR_PAVEU] = PixieData(std::filesystem::path("16fpu.pix"));
    whereto[PIX_FLOOR_PAVED] = PixieData(std::filesystem::path("16fpd.pix"));

    whereto[PIX_COLUMN1] = PixieData(std::filesystem::path("16clom0.pix"));
    whereto[PIX_COLUMN2] = PixieData(std::filesystem::path("16colm1.pix"));

    // Tree stuff
    whereto[PIX_TREE_B1] = PixieData(std::filesystem::path("16treeb1.pix"));
    whereto[PIX_TREE_M1] = PixieData(std::filesystem::path("16treem1.pix"));
    whereto[PIX_TREE_ML] = PixieData(std::filesystem::path("16treeml.pix"));
    whereto[PIX_TREE_MR] = PixieData(std::filesystem::path("16treemr.pix"));
    whereto[PIX_TREE_MT] = PixieData(std::filesystem::path("16treemt.pix"));
    whereto[PIX_TREE_T1] = PixieData(std::filesystem::path("16treet1.pix"));

    whereto[PIX_DIRT_1] = PixieData(std::filesystem::path("16dirt2.pix"));
    whereto[PIX_DIRT_DARK_1] = PixieData(std::filesystem::path("16dirtd1.pix"));

    whereto[PIX_DIRTGRASS_UL1] = PixieData(std::filesystem::path("16dgul1.pix"));
    whereto[PIX_DIRTGRASS_UR1] = PixieData(std::filesystem::path("16dgur1.pix"));
    whereto[PIX_DIRTGRASS_LL1] = PixieData(std::filesystem::path("16dgll1.pix"));
    whereto[PIX_DIRTGRASS_LR1] = PixieData(std::filesystem::path("16dglr1.pix"));

    whereto[PIX_DIRTGRASS_DARK_UL1] = PixieData(std::filesystem::path("16dguld.pix"));
    whereto[PIX_DIRTGRASS_DARK_UR1] = PixieData(std::filesystem::path("16dgurd.pix"));
    whereto[PIX_DIRTGRASS_DARK_LL1] = PixieData(std::filesystem::path("16dglld.pix"));
    whereto[PIX_DIRTGRASS_DARK_LR1] = PixieData(std::filesystem::path("16dglrd.pix"));

    whereto[PIX_PATH_1] = PixieData(std::filesystem::path("16path1.pix"));
    whereto[PIX_PATH_2] = PixieData(std::filesystem::path("16path2.pix"));
    whereto[PIX_PATH_3] = PixieData(std::filesystem::path("16path3.pix"));
    whereto[PIX_PATH_4] = PixieData(std::filesystem::path("16path4.pix"));

    whereto[PIX_BOULDER_1] = PixieData(std::filesystem::path("16stone1.pix"));
    whereto[PIX_BOULDER_2] = PixieData(std::filesystem::path("16stone2.pix"));
    whereto[PIX_BOULDER_3] = PixieData(std::filesystem::path("16stone3.pix"));
    whereto[PIX_BOULDER_4] = PixieData(std::filesystem::path("16stone4.pix"));

    whereto[PIX_COBBLE_1] = PixieData(std::filesystem::path("16cob1.pix"));
    whereto[PIX_COBBLE_2] = PixieData(std::filesystem::path("16cob2.pix"));
    whereto[PIX_COBBLE_3] = PixieData(std::filesystem::path("16cob3.pix"));
    whereto[PIX_COBBLE_4] = PixieData(std::filesystem::path("16cob4.pix"));

    whereto[PIX_WALL_ARROW_GRASS] = PixieData(std::filesystem::path("16wallog.pix"));
    whereto[PIX_WALL_ARROW_FLOOR] = PixieData(std::filesystem::path("16wallof.pix"));
    whereto[PIX_WALL_ARROW_GRASS_DARK] = PixieData(std::filesystem::path("16wallod.pix"));

    // Cliff tiles
    whereto[PIX_CLIFF_BOTTOM] = PixieData(std::filesystem::path("16cliff1.pix"));
    whereto[PIX_CLIFF_TOP] = PixieData(std::filesystem::path("16cliff2.pix"));
    whereto[PIX_CLIFF_LEFT] = PixieData(std::filesystem::path("16cliff3.pix"));
    whereto[PIX_CLIFF_RIGHT] = PixieData(std::filesystem::path("16cliff4.pix"));
    whereto[PIX_CLIFF_BACK_1] = PixieData(std::filesystem::path("16clifup.pix"));
    whereto[PIX_CLIFF_BACK_2] = PixieData(std::filesystem::path("16clifu2.pix"));
    whereto[PIX_CLIFF_BACK_L] = PixieData(std::filesystem::path("16cliful.pix"));
    whereto[PIX_CLIFF_BACK_R] = PixieData(std::filesystem::path("16clifur.pix"));
    whereto[PIX_CLIFF_TOP_L] = PixieData(std::filesystem::path("16clifdl.pix"));
    whereto[PIX_CLIFF_TOP_R] = PixieData(std::filesystem::path("16clifdr.pix"));

    // Damaged tiles...
    whereto[PIX_GRASS1_DAMAGED] = PixieData(std::filesystem::path("16grasd1.pix"));

    // Pete's graphics
    whereto[PIX_JAGGED_GROUND_1] = PixieData(std::filesystem::path("16jwg1.pix"));
    whereto[PIX_JAGGED_GROUND_2] = PixieData(std::filesystem::path("16jwg2.pix"));
    whereto[PIX_JAGGED_GROUND_3] = PixieData(std::filesystem::path("16jwg3.pix"));
    whereto[PIX_JAGGED_GROUND_4] = PixieData(std::filesystem::path("16jwg1.pix"));
}
