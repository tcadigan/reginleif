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

// Use this for globally setting th graphics dir, etc.
// Uint8 pix_directory[80];

// ***********************
// Other graphics routines
// ***********************

// Have been moved to video

// **** Editor related functions? *****

// Have also been moved to video

PixieData read_pixie_file(std::string const &filename)
{
    /*
     * Create a file stream and read the image
     * File data in form:
     * <# of frames>  1 byte
     * <x size>       1 byte
     * <y size>       1 byte
     * <pixie data>   x * y * frames bytes
     */

    PixieData result;
    SDL_RWops *infile = nullptr;
    std::stringstream buf;

    // Zardus: Try to fine file using open_read_file
    infile = open_read_file(std::filesystem::path("pix/" + filename));
    if (infile == nullptr) {
        infile = open_read_file(std::filesystem::path(filename));
    }

    if (infile == nullptr) {
        buf << "Cannot open pixie file pix/" << filename << "!" << std::endl;
        Log("%s", buf.str().c_str());

        exit(5);
    }

    SDL_RWread(infile, &result.frames, 1, 1);
    SDL_RWread(infile, &result.w, 1, 1);
    SDL_RWread(infile, &result.h, 1, 1);

    size_t size = (result.w * result.h) * result.frames;
    result.data = new Uint8[size];

    // Now read the data in a big chunk
    SDL_RWread(infile, result.data, 1, size);

    SDL_RWclose(infile);

    return result;
} // end of image-reading routing

void load_map_data(PixieData *whereto)
{
    // Load the pixie graphics data into memory
    whereto[0] = read_pixie_file(std::string("16tile.pix"));
    whereto[PIX_GRASS1] = read_pixie_file(std::string("16grass1.pix"));
    whereto[PIX_WATER1] = read_pixie_file(std::string("16water1.pix"));
    whereto[3] = read_pixie_file(std::string("16space.pix"));
    whereto[4] = read_pixie_file(std::string("16wall2.pix"));
    whereto[5] = read_pixie_file(std::string("16wall3.pix"));
    whereto[6] = read_pixie_file(std::string("16floor.pix"));
    whereto[7] = read_pixie_file(std::string("16walllo.pix"));
    whereto[8] = read_pixie_file(std::string("16w2lo.pix"));

    whereto[9] = read_pixie_file(std::string("16carpll.pix"));
    whereto[10] = read_pixie_file(std::string("16carpb.pix"));
    whereto[11] = read_pixie_file(std::string("16carplr.pix"));
    whereto[12] = read_pixie_file(std::string("16carpur.pix"));
    whereto[13] = read_pixie_file(std::string("16carpu.pix"));
    whereto[14] = read_pixie_file(std::string("16carpul.pix"));
    whereto[PIX_CARPET_L] = read_pixie_file(std::string("16carpl.pix"));
    whereto[PIX_CARPET_M] = read_pixie_file(std::string("16carpm.pix"));
    whereto[PIX_CARPET_M2] = read_pixie_file(std::string("16carpm2.pix"));
    whereto[PIX_CARPET_R] = read_pixie_file(std::string("16carpr.pix"));

    whereto[PIX_CARPET_SMALL_HOR] = read_pixie_file(std::string("16cshor.pix"));
    whereto[PIX_CARPET_SMALL_VER] = read_pixie_file(std::string("16csver.pix"));
    whereto[PIX_CARPET_SMALL_CUP] = read_pixie_file(std::string("16cscup.pix"));
    whereto[PIX_CARPET_SMALL_CAP] = read_pixie_file(std::string("16cscap.pix"));
    whereto[PIX_CARPET_SMALL_LEFT] = read_pixie_file(std::string("16csleft.pix"));
    whereto[PIX_CARPET_SMALL_RIGHT] = read_pixie_file(std::string("16csright.pix"));
    whereto[PIX_CARPET_SMALL_TINY] = read_pixie_file(std::string("16cstiny.pix"));

    whereto[PIX_GRASS2] = read_pixie_file(std::string("16grass2.pix"));
    whereto[PIX_GRASS3] = read_pixie_file(std::string("16grass3.pix"));
    whereto[PIX_GRASS4] = read_pixie_file(std::string("16grass4.pix"));

    whereto[PIX_GRASS_DARK_1] = read_pixie_file(std::string("16grassd.pix"));
    whereto[PIX_GRASS_DARK_2] = read_pixie_file(std::string("16grd2.pix"));
    whereto[PIX_GRASS_DARK_3] = read_pixie_file(std::string("16grd3.pix"));
    whereto[PIX_GRASS_DARK_4] = read_pixie_file(std::string("16grd4.pix"));
    whereto[PIX_GRASS_DARK_LL] = read_pixie_file(std::string("16grassi.pix"));
    whereto[PIX_GRASS_DARK_UR] = read_pixie_file(std::string("16grassh.pix"));
    whereto[PIX_GRASS_RUBBLE] = read_pixie_file(std::string("16grassr.pix"));

    whereto[PIX_GRASS_DARK_B1] = read_pixie_file(std::string("16grdb1.pix"));
    whereto[PIX_GRASS_DARK_B2] = read_pixie_file(std::string("16grdb2.pix"));
    whereto[PIX_GRASS_DARK_R1] = read_pixie_file(std::string("16grdr1.pix"));
    whereto[PIX_GRASS_DARK_R2] = read_pixie_file(std::string("16grdr2.pix"));
    whereto[PIX_GRASS_DARK_BR] = read_pixie_file(std::string("16grdbr.pix"));

    whereto[PIX_GRASS_LIGHT_1] = read_pixie_file(std::string("16grl1.pix"));
    whereto[PIX_GRASS_LIGHT_TOP] = read_pixie_file(std::string("16grlt.pix"));
    whereto[PIX_GRASS_LIGHT_RIGHT_TOP] = read_pixie_file(std::string("16grlrt.pix"));
    whereto[PIX_GRASS_LIGHT_RIGHT] = read_pixie_file(std::string("16grlr.pix"));
    whereto[PIX_GRASS_LIGHT_RIGHT_BOTTOM] = read_pixie_file(std::string("16grlrb.pix"));
    whereto[PIX_GRASS_LIGHT_BOTTOM] = read_pixie_file(std::string("16grlb.pix"));
    whereto[PIX_GRASS_LIGHT_LEFT_BOTTOM] = read_pixie_file(std::string("16grllb.pix"));
    whereto[PIX_GRASS_LIGHT_LEFT] = read_pixie_file(std::string("16grll.pix"));
    whereto[PIX_GRASS_LIGHT_LEFT_TOP] = read_pixie_file(std::string("16grllt.pix"));

    whereto[PIX_WATER2] = read_pixie_file(std::string("16water2.pix"));
    whereto[PIX_WATER3] = read_pixie_file(std::string("16water3.pix"));

    whereto[PIX_WATERGRASS_LL] = read_pixie_file(std::string("16wgll.pix"));
    whereto[PIX_WATERGRASS_LR] = read_pixie_file(std::string("16wglr.pix"));
    whereto[PIX_WATERGRASS_UL] = read_pixie_file(std::string("16wgul.pix"));
    whereto[PIX_WATERGRASS_UR] = read_pixie_file(std::string("16wgur.pix"));
    whereto[PIX_WATERGRASS_U] = read_pixie_file(std::string("16wgu.pix"));
    whereto[PIX_WATERGRASS_D] = read_pixie_file(std::string("16wgd.pix"));
    whereto[PIX_WATERGRASS_L] = read_pixie_file(std::string("16wgl.pix"));
    whereto[PIX_WATERGRASS_R] = read_pixie_file(std::string("16wgr.pix"));

    whereto[PIX_GRASSWATER_LL] = read_pixie_file(std::string("16gwll.pix"));
    whereto[PIX_GRASSWATER_LR] = read_pixie_file(std::string("16gwlr.pix"));
    whereto[PIX_GRASSWATER_UL] = read_pixie_file(std::string("16gwul.pix"));
    whereto[PIX_GRASSWATER_UR] = read_pixie_file(std::string("16gwur.pix"));

    whereto[PIX_PAVEMENT1] = read_pixie_file(std::string("16pave1.pix"));
    whereto[PIX_PAVEMENT2] = read_pixie_file(std::string("16pave2.pix"));
    whereto[PIX_PAVEMENT3] = read_pixie_file(std::string("16pave3.pix"));

    whereto[PIX_PAVESTEPS1] = read_pixie_file(std::string("16pstep.pix"));
    whereto[PIX_PAVESTEPS2] = read_pixie_file(std::string("16pstest.pix"));
    whereto[PIX_PAVESTEPS2L] = read_pixie_file(std::string("16ptestl.pix"));
    whereto[PIX_PAVESTEPS2R] = read_pixie_file(std::string("16ptestr.pix"));

    whereto[PIX_WALLSIDE1] = read_pixie_file(std::string("16brick1.pix"));
    whereto[PIX_WALLSIDE_L] = read_pixie_file(std::string("16brickl.pix"));
    whereto[PIX_WALLSIDE_R] = read_pixie_file(std::string("16brickr.pix"));
    whereto[PIX_WALLSIDE_C] = read_pixie_file(std::string("16brickc.pix"));
    whereto[PIX_WALLSIDE_CRACK_C1] = read_pixie_file(std::string("16brick3.pix"));

    whereto[PIX_WALL_LL] = read_pixie_file(std::string("16wallll.pix"));

    whereto[PIX_BRAZIER1] = read_pixie_file(std::string("16braz1.pix"));

    whereto[PIX_WALLTOP_H] = read_pixie_file(std::string("16ttop.pix"));

    whereto[PIX_TORCH1] = read_pixie_file(std::string("16torch1.pix"));
    whereto[PIX_TORCH2] = read_pixie_file(std::string("16torch2.pix"));
    whereto[PIX_TORCH3] = read_pixie_file(std::string("16torch3.pix"));

    whereto[PIX_FLOOR_PAVEL] = read_pixie_file(std::string("16fpl.pix"));
    whereto[PIX_FLOOR_PAVER] = read_pixie_file(std::string("16fpr.pix"));
    whereto[PIX_FLOOR_PAVEU] = read_pixie_file(std::string("16fpu.pix"));
    whereto[PIX_FLOOR_PAVED] = read_pixie_file(std::string("16fpd.pix"));

    whereto[PIX_COLUMN1] = read_pixie_file(std::string("16clom0.pix"));
    whereto[PIX_COLUMN2] = read_pixie_file(std::string("16colm1.pix"));

    // Tree stuff
    whereto[PIX_TREE_B1] = read_pixie_file(std::string("16treeb1.pix"));
    whereto[PIX_TREE_M1] = read_pixie_file(std::string("16treem1.pix"));
    whereto[PIX_TREE_ML] = read_pixie_file(std::string("16treeml.pix"));
    whereto[PIX_TREE_MR] = read_pixie_file(std::string("16treemr.pix"));
    whereto[PIX_TREE_MT] = read_pixie_file(std::string("16treemt.pix"));
    whereto[PIX_TREE_T1] = read_pixie_file(std::string("16treet1.pix"));

    whereto[PIX_DIRT_1] = read_pixie_file(std::string("16dirt2.pix"));
    whereto[PIX_DIRT_DARK_1] = read_pixie_file(std::string("16dirtd1.pix"));

    whereto[PIX_DIRTGRASS_UL1] = read_pixie_file(std::string("16dgul1.pix"));
    whereto[PIX_DIRTGRASS_UR1] = read_pixie_file(std::string("16dgur1.pix"));
    whereto[PIX_DIRTGRASS_LL1] = read_pixie_file(std::string("16dgll1.pix"));
    whereto[PIX_DIRTGRASS_LR1] = read_pixie_file(std::string("16dglr1.pix"));

    whereto[PIX_DIRTGRASS_DARK_UL1] = read_pixie_file(std::string("16dguld.pix"));
    whereto[PIX_DIRTGRASS_DARK_UR1] = read_pixie_file(std::string("16dgurd.pix"));
    whereto[PIX_DIRTGRASS_DARK_LL1] = read_pixie_file(std::string("16dglld.pix"));
    whereto[PIX_DIRTGRASS_DARK_LR1] = read_pixie_file(std::string("16dglrd.pix"));

    whereto[PIX_PATH_1] = read_pixie_file(std::string("16path1.pix"));
    whereto[PIX_PATH_2] = read_pixie_file(std::string("16path2.pix"));
    whereto[PIX_PATH_3] = read_pixie_file(std::string("16path3.pix"));
    whereto[PIX_PATH_4] = read_pixie_file(std::string("16path4.pix"));

    whereto[PIX_BOULDER_1] = read_pixie_file(std::string("16stone1.pix"));
    whereto[PIX_BOULDER_2] = read_pixie_file(std::string("16stone2.pix"));
    whereto[PIX_BOULDER_3] = read_pixie_file(std::string("16stone3.pix"));
    whereto[PIX_BOULDER_4] = read_pixie_file(std::string("16stone4.pix"));

    whereto[PIX_COBBLE_1] = read_pixie_file(std::string("16cob1.pix"));
    whereto[PIX_COBBLE_2] = read_pixie_file(std::string("16cob2.pix"));
    whereto[PIX_COBBLE_3] = read_pixie_file(std::string("16cob3.pix"));
    whereto[PIX_COBBLE_4] = read_pixie_file(std::string("16cob4.pix"));

    whereto[PIX_WALL_ARROW_GRASS] = read_pixie_file(std::string("16wallog.pix"));
    whereto[PIX_WALL_ARROW_FLOOR] = read_pixie_file(std::string("16wallof.pix"));
    whereto[PIX_WALL_ARROW_GRASS_DARK] = read_pixie_file(std::string("16wallod.pix"));

    // Cliff tiles
    whereto[PIX_CLIFF_BOTTOM] = read_pixie_file(std::string("16cliff1.pix"));
    whereto[PIX_CLIFF_TOP] = read_pixie_file(std::string("16cliff2.pix"));
    whereto[PIX_CLIFF_LEFT] = read_pixie_file(std::string("16cliff3.pix"));
    whereto[PIX_CLIFF_RIGHT] = read_pixie_file(std::string("16cliff4.pix"));
    whereto[PIX_CLIFF_BACK_1] = read_pixie_file(std::string("16clifup.pix"));
    whereto[PIX_CLIFF_BACK_2] = read_pixie_file(std::string("16clifu2.pix"));
    whereto[PIX_CLIFF_BACK_L] = read_pixie_file(std::string("16cliful.pix"));
    whereto[PIX_CLIFF_BACK_R] = read_pixie_file(std::string("16clifur.pix"));
    whereto[PIX_CLIFF_TOP_L] = read_pixie_file(std::string("16clifdl.pix"));
    whereto[PIX_CLIFF_TOP_R] = read_pixie_file(std::string("16clifdr.pix"));

    // Damaged tiles...
    whereto[PIX_GRASS1_DAMAGED] = read_pixie_file(std::string("16grasd1.pix"));

    // Pete's graphics
    whereto[PIX_JAGGED_GROUND_1] = read_pixie_file(std::string("16jwg1.pix"));
    whereto[PIX_JAGGED_GROUND_2] = read_pixie_file(std::string("16jwg2.pix"));
    whereto[PIX_JAGGED_GROUND_3] = read_pixie_file(std::string("16jwg3.pix"));
    whereto[PIX_JAGGED_GROUND_4] = read_pixie_file(std::string("16jwg1.pix"));
}
