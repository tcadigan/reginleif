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
#ifndef __LEVEL_EDITOR_DATA_HPP__
#define __LEVEL_EDITOR_DATA_HPP__

#include "campaign_data.hpp"
#include "campaign_result.hpp"
#include "editor_object_brush.hpp"
#include "editor_terrain_brush.hpp"
#include "level_data.hpp"
#include "object_type.hpp"
#include "rectf.hpp"
#include "selection_info.hpp"
#include "simple_button.hpp"
#include "video_screen.hpp"
#include "walker.hpp"

#include <SDL2/SDL.h>

#include <string>
#include <vector>

enum EditModeEnum : Uint8 {
    TERRAIN,
    OBJECT,
    SELECT
};

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

Sint32 redraw = 1; // Need to redraw?
Sint32 levelchanged = 0; // Has level changed?
Sint32 campaignchanged = 0; // Has campaign changed?
Sint32 rowsdown = 0;

Sint32 mouse_last_x = 0;
Sint32 mouse_last_y = 0;

std::vector<ObjectType> object_pane;

#define S_RIGHT 245
#define S_UP 1

#define PIX_TOP (S_UP + 79)
#define PIX_DOWN 4
#define PIX_BOTTOM (PIX_TOP + (PIX_DOWN * GRID_SIZE))

class LevelEditorData
{
public:
    CampaignData campaign;
    LevelData *level;
    EditModeEnum mode;
    EditorTerrainBrush terrain_brush;
    EditorObjectBrush object_brush;
    std::vector<SelectionInfo> selection;
    bool rect_selecting;
    Rectf selection_rect;
    bool dragging;
    Uint16 menu_button_height;
    std::set<SimpleButton *> menu_buttons;
    // The active menu buttons
    std::list<std::pair<SimpleButton *, std::set<SimpleButton *>>> current_menu;
    // The mode-specific buttons
    std::set<SimpleButton *> mode_buttons;
    std::set<SimpleButton *> pan_buttons;

    // Menu buttons
    // File menu
    SimpleButton fileButton;
    SimpleButton fileCampaignButton;
    SimpleButton fileLevelButton;
    SimpleButton fileQuitButton;

    // File > Campaign submenu
    SimpleButton fileCampaignImportButton;
    SimpleButton fileCampaignShareButton;
    SimpleButton fileCampaignNewButton;
    SimpleButton fileCampaignLoadButton;
    SimpleButton fileCampaignSaveButton;
    SimpleButton fileCampaignSaveAsButton;

    // File -> Level submenu
    SimpleButton fileLevelNewButton;
    SimpleButton fileLevelLoadButton;
    SimpleButton fileLevelSaveButton;
    SimpleButton fileLevelSaveAsButton;

    // Campaign menu
    SimpleButton campaignButton;
    SimpleButton campaignInfoButton;
    SimpleButton campaignProfileButton;
    SimpleButton campaignDetailsButton;
    SimpleButton campaignValidateButton;

    // Campaign > Profile submenu
    SimpleButton campaignProfileTitleButton;
    SimpleButton campaignProfileDescriptionButton;
    SimpleButton campaignProfileIconButton;
    SimpleButton campaignProfileAuthorsButton;
    SimpleButton campaignProfileContributorsButton;

    // Campaign > Details submenu
    SimpleButton campaignDetailsVersionButton;
    SimpleButton campaignDetailsSuggestedPowerButton;
    SimpleButton campaignDetailsFirstLevelButton;

    // Level menu
    SimpleButton levelButton;
    SimpleButton levelInfoButton;
    SimpleButton levelProfileButton;
    SimpleButton levelDetailsButton;
    SimpleButton levelGoalsButton;
    SimpleButton levelResmoothButton;
    SimpleButton levelDeleteTerrainButton;
    SimpleButton levelDeleteObjectsButton;

    // Level > Profile submenu
    SimpleButton levelProfileTitleButton;
    SimpleButton levelProfileDescriptionButton;

    // Level > Details submenu
    SimpleButton levelDetailsMapSizeButton;
    SimpleButton levelDetailsParValueButton;
    SimpleButton levelDetailsTimeLimitButton;

    // Level > Goals submenu
    SimpleButton levelGoalsEnemiesButton;
    SimpleButton levelGoalsGeneratorsButton;
    SimpleButton levelGoalsNPCsButton;

    // Edit menu
    SimpleButton modeButton;
    SimpleButton modeTerrainButton;
    SimpleButton modeObjectButton;
    SimpleButton modeSelectButton;

    // On-screen buttons
    SimpleButton pickerButton;
    SimpleButton gridSnapButton;
    SimpleButton terrainSmoothButton;
    SimpleButton setNameButton;
    SimpleButton prevTeamButton;
    SimpleButton nextTeamButton;
    SimpleButton prevLevelButton;
    SimpleButton nextLevelButton;
    SimpleButton prevClassButton;
    SimpleButton nextClassButton;
    SimpleButton facingButton;
    SimpleButton deleteButton;
    SimpleButton panUpButton;
    SimpleButton panDownButton;
    SimpleButton panLeftButton;
    SimpleButton panRightButton;
    SimpleButton panUpRightButton;
    SimpleButton panUpLeftButton;
    SimpleButton panDownRightButton;
    SimpleButton panDownLeftButton;

    LevelEditorData();
    ~LevelEditorData();

    bool loadCampaign(std::filesystem::path const &id);
    bool reloadCampaign();

    bool loadLevel(Sint32 id);
    bool reloadLevel();

    bool saveCampaignAs(std::filesystem::path const &id);
    bool saveCampaign();

    bool saveLevelAs(Sint32 id);
    bool saveLevel();

    void draw(VideoScreen *myscreen);
    Sint32 display_panel(VideoScreen *myscreen);

    bool mouse_on_menus(Sint32 mx, Sint32 my);
    void update_menu_buttons();
    void reset_mode_buttons();
    void activate_mode_button(SimpleButton *button);

    void clear_terrain();
    void resmooth_terrain();
    void mouse_down(Sint32 mx, Sint32 my);
    void mouse_motion(Sint32 mx, Sint32 my, Sint32 dx, Sint32 dy);
    void mouse_up(Sint32 mx, Sint32 my, Sint32 old_mx, Sint32 old_my, bool &done);
    void pick_by_mouse(Sint32 mx, Sint32 my);

    bool is_in_grid(Sint32 x, Sint32 y);
    Uint8 get_terrain(Sint32 x, Sint32 y);
    void set_terrain(Sint32 x, Sint32 y, Uint8 terrain);
    Walker *get_object(Sint32 x, Sint32 y);
};

#endif
