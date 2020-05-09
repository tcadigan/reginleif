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

extern Sint32 backgrounds[];
extern Sint32 maxrows;
extern bool redraw; // Need to redraw?
extern bool levelchanged; // Has level changed?
extern bool campaignchanged; // Has campaign changed?
extern Sint32 rowsdown;

extern std::vector<ObjectType> object_pane;

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
