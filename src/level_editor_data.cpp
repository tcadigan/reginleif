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
#include "level_editor_data.hpp"

#include "gloader.hpp"
#include "gparser.hpp"
#include "input.hpp"
#include "io.hpp"
#include "level_editor.hpp"
#include "level_picker.hpp"
#include "picker.hpp"
#include "rect.hpp"
#include "screen.hpp"
#include "util.hpp"
#include "view.hpp"
#include "walker.hpp"

#include <algorithm>
#include <sstream>
#include <string>

#define NUM_BACKGROUNDS PIX_MAX
#define PIX_OVER 4

#define L_D(x) ((S_UP + 7) + (8 * x))

#define DEFAULT_EDITOR_MENU_BUTTON_HEIGHT 20

#ifdef REDUCE_OVERSCAN
#define OVERSCAN_PADDING 6
#else
#define OVERSCAN_PADDING 0
#endif


// buffers: PORT: changed start_time to start_time_s to avoid conflict with input.cpp
Sint32 start_time_s; // For timer ops

LevelEditorData::LevelEditorData()
    : campaign(new CampaignData("org.openglad.gladiator"))
    , level(new LevelData(1))
    , mode(EditModeEnum::TERRAIN)
    , rect_selecting(false)
    , dragging(false)
    , menu_button_height(DEFAULT_EDITOR_MENU_BUTTON_HEIGHT)
    , fileButton("File", OVERSCAN_PADDING, 0, 30, menu_button_height)
    , fileCampaignButton("Campaign >", OVERSCAN_PADDING, fileButton.area.y + fileButton.area.h,
                         65, menu_button_height, true)
    , fileLevelButton("Level >", OVERSCAN_PADDING,
                      fileCampaignButton.area.y + fileCampaignButton.area.h,
                      65, menu_button_height, true, true)
    , fileQuitButton("Exit", OVERSCAN_PADDING, fileLevelButton.area.y + fileLevelButton.area.h,
                     65, menu_button_height, true, true)
    , fileCampaignImportButton("Import...",
                               fileCampaignButton.area.x + fileCampaignButton.area.w,
                               fileCampaignButton.area.y, 65, menu_button_height, true)
    , fileCampaignShareButton("Share...", fileCampaignImportButton.area.x,
                              fileCampaignImportButton.area.y + fileCampaignImportButton.area.h,
                              65, menu_button_height, true, true)
    // , fileCampaignNewButton("New", fileCampaignImportButton.area.x,
    //                         fileCampaignShareButton.area.y + fileCampaignShareButton.area.h,
    //                         65, menu_button_height, true, true)
    , fileCampaignNewButton("New",
                            fileCampaignButton.area.x + fileCampaignButton.area.w,
                            fileCampaignButton.area.y, 65, menu_button_height, true)
    , fileCampaignLoadButton("Load...", fileCampaignImportButton.area.x,
                             fileCampaignNewButton.area.y + fileCampaignNewButton.area.h,
                             65, menu_button_height, true, true)
    , fileCampaignSaveButton("Save", fileCampaignImportButton.area.x,
                             fileCampaignLoadButton.area.y + fileCampaignLoadButton.area.h,
                             65, menu_button_height, true, true)
    , fileCampaignSaveAsButton("Save As...", fileCampaignImportButton.area.x,
                               fileCampaignSaveButton.area.y + fileCampaignSaveButton.area.h,
                               65, menu_button_height, true, true)
    , fileLevelNewButton("New", fileLevelButton.area.x + fileLevelButton.area.w,
                         fileLevelButton.area.y, 65, menu_button_height, true)
    , fileLevelLoadButton("Load...", fileLevelNewButton.area.x,
                          fileLevelNewButton.area.y + fileLevelNewButton.area.h,
                          65, menu_button_height, true, true)
    , fileLevelSaveButton("Save", fileLevelNewButton.area.x,
                          fileLevelLoadButton.area.y + fileLevelLoadButton.area.h,
                          65, menu_button_height, true, true)
    , fileLevelSaveAsButton("Save As...", fileLevelNewButton.area.x,
                            fileLevelSaveButton.area.y + fileLevelSaveButton.area.h,
                            65, menu_button_height, true, true)
    , campaignButton("Campaign", fileButton.area.x + fileButton.area.w, 0, 55, menu_button_height)
    , campaignInfoButton("Info...", campaignButton.area.x,
                         campaignButton.area.y + campaignButton.area.h,
                         59, menu_button_height, true, true)
    , campaignProfileButton("Profile >" , campaignButton.area.x,
                            campaignInfoButton.area.y + campaignInfoButton.area.h,
                            59, menu_button_height, true, true)
    , campaignDetailsButton("Details >", campaignButton.area.x,
                            campaignProfileButton.area.y + campaignProfileButton.area.h,
                            59, menu_button_height, true, true)
    , campaignValidateButton("Validate", campaignButton.area.x,
                             campaignDetailsButton.area.y + campaignDetailsButton.area.h,
                             59, menu_button_height, true, true)
    , campaignProfileTitleButton("Title...",
                                 campaignProfileButton.area.x + campaignProfileButton.area.w,
                                 campaignProfileButton.area.y, 95, menu_button_height, true)
    , campaignProfileDescriptionButton("Description...", campaignProfileTitleButton.area.x,
                                       campaignProfileTitleButton.area.y + campaignProfileTitleButton.area.h,
                                       95, menu_button_height, true, true)
    , campaignProfileIconButton("Icon...", campaignProfileTitleButton.area.x,
                                campaignProfileDescriptionButton.area.y + campaignProfileDescriptionButton.area.h,
                                95, menu_button_height, true, true)
    // , campaignProfileAuthorsButton("Authors...", campaignProfileTitleButton.area.x,
    //                                campaignProfileIconButton.area.y + campaignProfileIconButton.area.h,
    //                                95, menu_button_height, true, true)
    , campaignProfileAuthorsButton("Authors...", campaignProfileTitleButton.area.x,
                                   campaignProfileDescriptionButton.area.y + campaignProfileDescriptionButton.area.h,
                                   95, menu_button_height, true, true)
    , campaignProfileContributorsButton("Contributors...", campaignProfileTitleButton.area.x,
                                        campaignProfileAuthorsButton.area.y + campaignProfileAuthorsButton.area.h,
                                        95, menu_button_height, true, true)
    , campaignDetailsVersionButton("Version...",
                                   campaignDetailsButton.area.x + campaignDetailsButton.area.w,
                                   campaignDetailsButton.area.y, 113, menu_button_height, true)
    , campaignDetailsSuggestedPowerButton("Suggested power...",
                                          campaignDetailsVersionButton.area.x,
                                          campaignDetailsVersionButton.area.y + campaignDetailsVersionButton.area.h,
                                          113, menu_button_height, true, true)
    , campaignDetailsFirstLevelButton("First level...", campaignDetailsVersionButton.area.x,
                                      campaignDetailsSuggestedPowerButton.area.y + campaignDetailsSuggestedPowerButton.area.h,
                                      113, menu_button_height, true, true)
    , levelButton("Level", campaignButton.area.x + campaignButton.area.w,
                  0, 40, menu_button_height)
    , levelInfoButton("Info...", levelButton.area.x, levelButton.area.y + levelButton.area.h,
                      110, menu_button_height, true)
    , levelProfileButton("Profile >", levelButton.area.x,
                      levelInfoButton.area.y + levelInfoButton.area.h,
                      110, menu_button_height, true, true)
    , levelDetailsButton("Details >", levelButton.area.x,
                         levelProfileButton.area.y + levelProfileButton.area.h,
                         110, menu_button_height, true, true)
    , levelGoalsButton("Goals >", levelButton.area.x,
                       levelDetailsButton.area.y + levelDetailsButton.area.h,
                       110, menu_button_height, true, true)
    , levelResmoothButton("Resmooth terrain", levelButton.area.x,
                          levelGoalsButton.area.y + levelGoalsButton.area.h,
                          110, menu_button_height, true, true)
    , levelDeleteTerrainButton("Clear all terrain", levelButton.area.x,
                               levelResmoothButton.area.y + levelResmoothButton.area.h,
                               110, menu_button_height, true, true)
    , levelDeleteObjectsButton("Clear all objects", levelButton.area.x,
                               levelDeleteTerrainButton.area.y + levelDeleteTerrainButton.area.h,
                               110, menu_button_height, true, true)
    , levelProfileTitleButton("Title...", levelProfileButton.area.x + levelProfileButton.area.w,
                              levelProfileButton.area.y, 95, menu_button_height, true)
    , levelProfileDescriptionButton("Description...", levelProfileTitleButton.area.x,
                                    levelProfileTitleButton.area.y + levelProfileTitleButton.area.h,
                                    95, menu_button_height, true, true)
    , levelDetailsMapSizeButton("Map size...",
                                levelDetailsButton.area.x + levelDetailsButton.area.w,
                                levelDetailsButton.area.y, 95, menu_button_height, true)
    , levelDetailsParValueButton("Par value...", levelDetailsMapSizeButton.area.x,
                                 levelDetailsMapSizeButton.area.y + levelDetailsMapSizeButton.area.h,
                                 95, menu_button_height, true, true)
    , levelDetailsTimeLimitButton("Time limit...", levelDetailsParValueButton.area.x,
                                  levelDetailsParValueButton.area.y + levelDetailsParValueButton.area.h,
                                  95, menu_button_height, true, true)
    , levelGoalsEnemiesButton("Defeat enemies: On",
                              (levelGoalsButton.area.x + levelGoalsButton.area.w) - (2 * OVERSCAN_PADDING),
                              levelGoalsButton.area.y, 125, menu_button_height, true)
    , levelGoalsGeneratorsButton("Beat generators: Off", levelGoalsEnemiesButton.area.x,
                                 levelGoalsEnemiesButton.area.y + levelGoalsEnemiesButton.area.h,
                                 125, menu_button_height, true, true)
    , levelGoalsNPCsButton("Protect NPCs: Off", levelGoalsEnemiesButton.area.x,
                           levelGoalsEnemiesButton.area.y + levelGoalsEnemiesButton.area.h,
                           125, menu_button_height, true, true)
    , modeButton("Edit (Terrain)", levelButton.area.x + levelButton.area.w,
                 0, 90, menu_button_height)
    , modeTerrainButton("Terrain Mode", modeButton.area.x, modeButton.area.y + modeButton.area.h,
                        75, menu_button_height, true, true)
    , modeObjectButton("Object Mode", modeButton.area.x,
                       modeTerrainButton.area.y + modeTerrainButton.area.h,
                       75, menu_button_height, true, true)
    , modeSelectButton("Select Mode", modeButton.area.x,
                       modeObjectButton.area.y + modeObjectButton.area.h,
                       75,  menu_button_height, true, true)
    , pickerButton("Pick", OVERSCAN_PADDING, 20, 27, 15)
    , gridSnapButton("Snap", (pickerButton.area.x + pickerButton.area.w) + 2, 20, 27, 15)
    , terrainSmoothButton("Smooth", (pickerButton.area.x + pickerButton.area.w) + 2,
                          20, 39, 15) // Same place as gridSnapButton
    , setNameButton("Set name", OVERSCAN_PADDING,
                    (10 + gridSnapButton.area.y) + gridSnapButton.area.h, 52, 15)
    , prevTeamButton("< Team", OVERSCAN_PADDING, setNameButton.area.y + setNameButton.area.h,
                     40, 15)
    , nextTeamButton("Team >", prevTeamButton.area.x + prevTeamButton.area.w,
                     prevTeamButton.area.y, 40, 15)
    , prevLevelButton("< Lvl", OVERSCAN_PADDING, prevTeamButton.area.y + prevTeamButton.area.h,
                      40, 15)
    , nextLevelButton("Lvl >", prevLevelButton.area.x + prevLevelButton.area.w,
                      prevLevelButton.area.y, 40, 15)
    , prevClassButton("< Class", OVERSCAN_PADDING,
                      prevLevelButton.area.y + prevLevelButton.area.h, 48, 15)
    , nextClassButton("Class >", prevClassButton.area.x + prevClassButton.area.w,
                      prevClassButton.area.y, 48, 15)
    , facingButton("Facing >", OVERSCAN_PADDING, prevClassButton.area.y + prevClassButton.area.h,
                   52, 15)
    , deleteButton("Delete", OVERSCAN_PADDING, (10 + facingButton.area.y) + facingButton.area.h,
                   40, 15)
    , panUpButton("U", OVERSCAN_PADDING + 18, 200 - 51, 15, 15)
    , panDownButton("D", OVERSCAN_PADDING + 18, 200 - 21, 15, 15)
    , panLeftButton("L", OVERSCAN_PADDING + 3, 200 - 36, 15, 15)
    , panRightButton("R", OVERSCAN_PADDING + 33, 200 - 36, 15, 15)
    , panUpRightButton("", OVERSCAN_PADDING + 33, 200 - 51, 15, 15)
    , panUpLeftButton("", OVERSCAN_PADDING + 3, 200 - 51, 15, 15)
    , panDownRightButton("", OVERSCAN_PADDING + 33, 200 - 21, 15, 15)
    , panDownLeftButton("", OVERSCAN_PADDING + 3, 200 - 21, 15, 15)
{
    // Top menu
    menu_buttons.insert(&fileButton);
    menu_buttons.insert(&campaignButton);
    menu_buttons.insert(&levelButton);
    menu_buttons.insert(&modeButton);

    gridSnapButton.set_colors_enabled();
    terrainSmoothButton.set_colors_enabled();

    myscreen->viewob[0]->myradar->force_lower_position = true;
}

LevelEditorData::~LevelEditorData()
{
    delete campaign;
    delete level;
}

bool LevelEditorData::loadCampaign(std::string const &id)
{
    campaign->id = id;

    return campaign->load();
}

bool LevelEditorData::reloadCampaign()
{
    return campaign->load();
}

bool LevelEditorData::loadLevel(Sint32 id)
{
    level->id = id;
    bool result = level->load();
    update_menu_buttons();

    return result;
}

bool LevelEditorData::reloadLevel()
{
    bool result = level->load();
    update_menu_buttons();

    return result;
}

bool LevelEditorData::saveCampaignAs(std::string const &id)
{
    bool result = campaign->save_as(id);

    // Remount for consistency in PhysFS
    if (!remount_campaign_package()) {
        Log("Failed to remount campaign after saving it.\n");

        return false;
    }

    return result;
}

bool LevelEditorData::saveCampaign()
{
    bool result = campaign->save();

    // Remount for consistency in PhysFS
    if (!remount_campaign_package()) {
        Log("Failed to remount campaign after saving it.\n");

        return false;
    }

    return result;
}

bool LevelEditorData::saveLevelAs(Sint32 id)
{
    level->id = id;

    std::stringstream buf("scen");

    buf << id;

    std::string scen(buf.str());
    buf.clear();
    scen.resize(20);

    level->grid_file = scen;

    std::string old_campaign(get_mounted_campaign());
    unpack_campaign(old_campaign);
    bool result = level->save();

    if (result) {
        result = repack_campaign(old_campaign);
    }

    cleanup_unpacked_campaign();

    // Remount for consistency in PhyFS
    remount_campaign_package();

    return result;
}

// Wouldn't spatial partitioning be nice? Too bad!
bool LevelEditorData::mouse_on_menus(Sint32 mx, Sint32 my)
{
    for (auto const &e : menu_buttons) {
        if (e->contains(mx, my)) {
            return true;
        }
    }

    for (auto const &e : mode_buttons) {
        if (e->contains(mx, my)) {
            return true;
        }
    }

    // Count anything in the area of the pan buttons
    if (!pan_buttons.empty()
        && Rect(panLeftButton.area.x,
                panUpButton.area.y,
                (panRightButton.area.x + panRightButton.area.w) - panLeftButton.area.x,
                (panDownButton.area.y + panDownButton.area.h) - panUpButton.area.y).contains(mx, my)) {
        return true;
    }

    for (auto const &e : current_menu) {
        for (auto const &f : e.second) {
            if (f->contains(mx, my)) {
                return true;
            }
        }
    }

    return false;
}

void LevelEditorData::update_menu_buttons()
{
    levelGoalsEnemiesButton.label = "Defeat enemies: ";

    if (level->type & LevelData::TYPE_CAN_EXIT_WHENEVER) {
        levelGoalsEnemiesButton.label += "Off";
    } else {
        levelGoalsEnemiesButton.label += "On";
    }

    levelGoalsGeneratorsButton.label = "Beat generators: ";

    if (level->type & LevelData::TYPE_MUST_DESTROY_GENERATORS) {
        levelGoalsGeneratorsButton.label += "On";
    } else {
        levelGoalsGeneratorsButton.label += "Off";
    }

    levelGoalsNPCsButton.label = "Protect NPCs: ";

    if (level->type & LevelData::TYPE_MUST_PROTECT_NAMED_NPCS) {
        levelGoalsNPCsButton.label += "On";
    } else {
        levelGoalsNPCsButton.label += "Off";
    }
}

void LevelEditorData::reset_mode_buttons()
{
    mode_buttons.clear();

    switch (mode) {
    case TERRAIN:
        mode_buttons.insert(&pickerButton);
        mode_buttons.insert(&terrainSmoothButton);

        if (terrain_brush.picking) {
            pickerButton.set_colors_active();
        } else {
            pickerButton.set_colors_normal();
        }

        break;
    case OBJECT:
        mode_buttons.insert(&pickerButton);
        mode_buttons.insert(&gridSnapButton);
        mode_buttons.insert(&prevTeamButton);
        mode_buttons.insert(&nextTeamButton);

        if (object_brush.picking) {
            pickerButton.set_colors_active();
        } else {
            pickerButton.set_colors_normal();
        }

        break;
    case SELECT:
        mode_buttons.insert(&gridSnapButton);

        if ((selection.size() == 1) && (selection.front().order == ORDER_LIVING)) {
            mode_buttons.insert(&setNameButton);
        }

        if (!selection.empty()) {
            mode_buttons.insert(&prevTeamButton);
            mode_buttons.insert(&nextTeamButton);
            mode_buttons.insert(&prevLevelButton);
            mode_buttons.insert(&nextLevelButton);
            mode_buttons.insert(&prevClassButton);
            mode_buttons.insert(&nextClassButton);
            mode_buttons.insert(&facingButton);
            mode_buttons.insert(&deleteButton);
        }

        break;
    }
}

void LevelEditorData::activate_mode_button(SimpleButton *button)
{
    if (button == &pickerButton) {
        if (mode == TERRAIN) {
            terrain_brush.picking = !terrain_brush.picking;

            if (terrain_brush.picking) {
                pickerButton.set_colors_active();
            } else {
                pickerButton.set_colors_normal();
            }
        } else if (mode == OBJECT) {
            object_brush.picking = !object_brush.picking;

            if (object_brush.picking) {
                pickerButton.set_colors_active();
            } else {
                pickerButton.set_colors_normal();
            }
        }
    } else if (button == &gridSnapButton) {
        object_brush.snap_to_grid = !object_brush.snap_to_grid;

        if (object_brush.snap_to_grid) {
            gridSnapButton.set_colors_enabled();
        } else {
            gridSnapButton.set_colors_normal();
        }
    } else if (button == &terrainSmoothButton) {
        terrain_brush.use_smoothing = !terrain_brush.use_smoothing;

        if (terrain_brush.use_smoothing) {
            terrainSmoothButton.set_colors_enabled();
        } else {
            terrainSmoothButton.set_colors_normal();
        }
    } else if (button == &setNameButton) {
        if ((selection.size() == 1) && (selection.front().order == ORDER_LIVING)) {
            Walker *obj = selection.front().get_object(level);

            if (obj != nullptr) {
                std::string name(obj->stats.name);

                if (prompt_for_string("Rename", name)) {
                    name.resize(11);
                    obj->stats.name = name;
                    selection.front().name = obj->stats.name;
                    levelchanged = 1;
                }
            }
        }
    } else if (button == &prevTeamButton) {
        if (mode == SELECT) {
            for (auto &&e : selection) {
                Walker *obj = e.get_object(level);

                if (obj != nullptr) {
                    if (obj->team_num > 0) {
                        --obj->team_num;
                    } else {
                        obj->team_num = MAX_TEAM;
                    }

                    levelchanged = 1;
                }
            }
        } else if (mode == OBJECT) {
            if (object_brush.team > 0) {
                --object_brush.team;
            } else {
                object_brush.team = MAX_TEAM;
            }
        }
    } else if (button == &nextTeamButton) {
        if (mode == SELECT) {
            for (auto &&e : selection) {
                Walker *obj = e.get_object(level);

                if (obj != nullptr) {
                    if (obj->team_num < MAX_TEAM) {
                        ++obj->team_num;
                    } else {
                        obj->team_num = 0;
                    }

                    levelchanged = 1;
                }
            }
        } else if (mode == OBJECT) {
            if (object_brush.team < MAX_TEAM) {
                ++object_brush.team;
            } else {
                object_brush.team = 0;
            }
        }
    } else if (button == &prevLevelButton) {
        for (auto &&e : selection) {
            Walker *obj = e.get_object(level);

            if (obj != nullptr) {
                if (obj->stats.level > 1) {
                    --obj->stats.level;
                    e.level = obj->stats.level;
                    levelchanged = 1;
                }
            }
        }
    } else if (button == &nextLevelButton) {
        for (auto &&e : selection) {
            Walker *obj = e.get_object(level);

            if (obj != nullptr) {
                ++obj->stats.level;
                e.level = obj->stats.level;
                levelchanged = 1;
            }
        }
    } else if (button == &prevClassButton) {
        for (auto &&e : selection) {
            Walker *obj = e.get_object(level);

            if ((obj != nullptr) && (obj->query_order() == ORDER_LIVING)) {
                if (e.family > 0) {
                    --e.family;
                } else {
                    e.family = NUM_FAMILIES - 1;
                }

                set_walker(obj, e.order, e.family);
                obj->ani_type = ANI_WALK;
                obj->transform_to(e.order, e.family);
                obj->set_frame(obj->ani[obj->curdir][0]);
                obj->setxy(e.x, e.y);
                e.set(obj);

                levelchanged = 1;
            }
        }
    } else if (button == &nextClassButton) {
        for (auto &&e : selection) {
            Walker *obj = e.get_object(level);

            if ((obj != nullptr) && (obj->query_order() == ORDER_LIVING)) {
                if ((e.family + 1) < NUM_FAMILIES) {
                    ++e.family;
                } else {
                    e.family = 0;
                }

                set_walker(obj, e.order, e.family);
                obj->ani_type = ANI_WALK;
                obj->transform_to(e.order, e.family);
                obj->set_frame(obj->ani[obj->curdir][0]);
                obj->setxy(e.x, e.y);
                e.set(obj);

                levelchanged = 1;
            }
        }
    } else if (button == &facingButton) {
        for (auto &&e : selection) {
            Walker *obj = e.get_object(level);

            if (obj != nullptr) {
                if (obj->curdir < FACE_UP_LEFT) {
                    ++obj->curdir;
                } else {
                    obj->curdir = FACE_UP;
                }

                obj->set_frame(obj->ani[obj->curdir][0]);

                levelchanged = 1;
            }
        }
    } else if (button == &deleteButton) {
        for (auto &&e : selection) {
            Walker *obj = e.get_object(level);

            if (obj != nullptr) {
                level->remove_ob(obj);
                delete obj;
                levelchanged = 1;
            }
        }

        selection.clear();
    }
}

bool LevelEditorData::saveLevel()
{
    std::stringstream buf("scen");

    buf << level->id;

    std::string str(buf.str());
    buf.clear();
    str.resize(20);

    level->grid_file = str;

    std::string old_campaign(get_mounted_campaign());
    unpack_campaign(old_campaign);

    bool result = level->save();

    if (result) {
        result = repack_campaign(get_mounted_campaign());
    }

    cleanup_unpacked_campaign();

    // Remount for consistency in PhysFs
    remount_campaign_package();

    return result;
}

void LevelEditorData::draw(VideoScreen *myscreen)
{
    myscreen->clearbuffer();
    myscreen->redraw();

    if (rect_selecting) {
        Rectf r((selection_rect.x - level->topx) + myscreen->viewob[0]->xloc,
                (selection_rect.y - level->topy) + myscreen->viewob[0]->yloc,
                selection_rect.w, selection_rect.h);

        if (r.w < 0.0f) {
            r.x += r.w;
            r.w = -r.w;
        }

        if (r.h < 0.0f) {
            r.y += r.h;
            r.h = -r.h;
        }

        myscreen->draw_box(r.x, r.y, r.x + r.w, r.y + r.h, ORANGE_START, 0, 1);
        redraw = 1;
    }

    display_panel(myscreen);
}

Sint32 LevelEditorData::display_panel(VideoScreen *myscreen)
{
    Text& scentext = myscreen->text_normal;
    // Draw selection indicators
    if ((mode == SELECT) && !selection.empty()) {
        for (auto const &e : selection) {
            // Daw cursor
            Sint32 mx;
            Sint32 my;
            mx = e.x - level->topx;
            my = e.y - level->topy;

            // Draw target tile
            Sint32 worldx = mx + level->topx;
            Sint32 worldy = my + level->topy;
            Sint32 screenx = worldx - level->topx;
            Sint32 screeny = worldy - level->topy;

            if (dragging) {
                myscreen->draw_box(screenx, screeny, screenx + e.w, screeny + e.h, ORANGE_START, 0, 1);
            } else {
                myscreen->draw_box(screenx, screeny, screenx + e.w, screeny + e.h, YELLOW, 0, 1);
            }
        }
    }

    // Draw minimap
    myscreen->viewob[0]->myradar->draw(*level, nullptr);

    // Draw mode-specific buttons
    for (auto const &e : mode_buttons) {
        e->draw(myscreen);
    }

    if (!pan_buttons.empty()) {
        Rect r(panLeftButton.area.x,
               panUpButton.area.y,
               panRightButton.area.w + panRightButton.area.x,
               (panDownButton.area.h + panDownButton.area.h) - panUpButton.area.y);

        myscreen->fastbox(r.x, r.y, r.w, r.h, 13);

        for (auto const &e : pan_buttons) {
            e->draw(myscreen);
        }
    }

    std::string message;
    // For loops
    Sint32 i;
    Sint32 j;
    // Sint32 static family =-1;
    // Sint32 hitpoints = -1;
    // Sint32 score = -1;
    // Sint32 act = -1;
    Sint32 numobs = myscreen->level_data.numobs;
    Sint32 lm = 245;
    Sint32 curline = 0;
    Sint32 whichback;

    std::string blood_string("REMAINS");

    if (cfg.is_on("effects", "gore")) {
        blood_string = "BLOOD";
    }

    std::string treasures[NUM_FAMILIES] = {
        blood_string,
        "DRUMSTICK",
        "GOLD",
        "SILVER",
        "MAGIC",
        "INVIS",
        "INVULN",
        "FLIGHT",
        "EXIT",
        "TELEPORTER",
        "LIFE GEM",
        "KEY",
        "SPEED",
        "CC"
    };

    std::string weapons[NUM_FAMILIES] = {
        "KNIFE",
        "ROCK",
        "ARROW",
        "FIREBALL",
        "TREE",
        "METEOR",
        "SPRINKLE",
        "BONE",
        blood_string,
        "BLOB",
        "FIRE ARROW",
        "LIGHTNING",
        "GLOW",
        "WAVE 1",
        "WAVE 2",
        "WAVE 3",
        "PROTECTION",
        "HAMMER",
        "DOOR"
    };

    std::string livings[NUM_FAMILIES] = {
        "SOLDIER",
        "ELF",
        "ARCHER",
        "MAGE",
        "SKELETON",
        "CLERIC",
        "ELEMENTAL",
        "FAERIE",
        "L SLIME",
        "S SLIME",
        "M SLIME",
        "THIEF",
        "GHOST",
        "DRUID",
        "ORC",
        "ORC CAPTAIN",
        "BARBARIAN",
        "ARCHMAGE",
        "GOLEM",
        "G SKELETON",
        "TOWER1"
    };

    std::stringstream buf;

    // Info box for select mode
    if ((mode == SELECT) && !selection.empty()) {
        // Draw the info box background
        myscreen->draw_button(lm - 4, L_D(-1) + 4, 315, L_D(7) - 2, 1, 1);

        if (!selection.empty()) {
            scentext.write_xy(lm, L_D(curline), "SELECTED:", RED, 1);
            ++curline;
        }

        Sint32 i = 0;

        for (auto const &e : selection) {
            bool showing_name = false;

            // Too many names to show?
            if (((i + 1) == 6) && (selection.size() > 6)) {
                buf << "+" << selection.size() - 5 << " more";
                std::string text(buf.str());
                buf.clear();
                text.resize(20);
                scentext.write_xy(lm, L_D(curline), text, DARK_BLUE, 1);
                ++curline;
                // No more;
                break;
            } else if (!e.name.empty() && (e.order == ORDER_LIVING)) {
                // Show name
                scentext.write_xy(lm, L_D(curline), ("\"" + e.name + "\"").c_str(), DARK_BLUE, 1);
                ++curline;
                showing_name = true;
            } else if (selection.empty()) {
                // Skip name line for guy with no name;
                ++curline;
            }

            if ((selection.size() == 1) || !showing_name) {
                // Show family namemessage[0] = '\0';

                if (e.order == ORDER_LIVING) {
                    message.append(livings[e.family]);
                } else if (e.order == ORDER_GENERATOR) {
                    // Who are we?
                    switch (e.family) {
                    case FAMILY_TENT:
                        message.append("TENT");

                        break;
                    case FAMILY_TOWER:
                        message.append("MAGE_TOWER");

                        break;
                    case FAMILY_BONES:
                        message.append("BONEPILE");

                        break;
                    case FAMILY_TREEHOUSE:
                        message.append("TREEHOUSE");

                        break;
                    default:
                        message.append("GENERATOR");

                        break;
                    }
                } else if (e.order == ORDER_SPECIAL) {
                    message.append("START_TILE");
                } else if (e.order == ORDER_TREASURE) {
                    message.append(treasures[e.family]);
                } else if (e.order == ORDER_WEAPON) {
                    message.append(weapons[e.family]);
                } else {
                    message.append("UNKNOWN");
                }

                message.resize(50);
                scentext.write_xy(lm, L_D(curline), message, DARK_BLUE, 1);
                ++curline;
            }

            ++i;

            // Only show extended info for a single selection
            if (!selection.empty()) {
                continue;
            }

            // More info for a single selection
            // Level display
            message.clear();

            switch (e.order) {
            case ORDER_LIVING:
            case ORDER_GENERATOR:
                buf << "LEVEL: " << e.level;

                break;
            case ORDER_TREASURE:
                if ((e.family == FAMILY_GOLD_BAR) || (e.family == FAMILY_SILVER_BAR)) {
                    buf << "VALUE: " << e.level;
                } else if (e.family == FAMILY_KEY) {
                    buf << "DOOR ID: " << e.level;
                } else if (e.family == FAMILY_TELEPORTER) {
                    buf << "GROUP: " << e.level;
                } else if (e.family == FAMILY_EXIT) {
                    buf << "EXIT TO: " << e.level;
                } else if (e.family != FAMILY_STAIN) {
                    buf << "POWER: " << e.level;
                }

                break;
            case ORDER_WEAPON:
                if (e.family == FAMILY_DOOR) {
                    buf << "DOOR ID: " << e.level;
                } else {
                    buf << "POWER: " << e.level;
                }

                break;
            default:

                break;
            }

            message = buf.str();
            buf.clear();

            if (!message.empty()) {
                message.resize(50);
                scentext.write_xy(lm, L_D(curline), message, DARK_BLUE, 1);
                ++curline;
            }
        }
    }

    if (mode == OBJECT) {
        // Draw the bounding box
        myscreen->draw_button(lm - 4, L_D(-1) + 4, 315, L_D(7) - 2, 1, 1);

        // Get team number...
        message.clear();

        if (object_brush.order == ORDER_LIVING) {
            message.append(livings[object_brush.family]);
        } else if (object_brush.order == ORDER_GENERATOR) {
            // Who are we?
            switch (object_brush.family) {
            case FAMILY_TENT:
                message.append("TENT");

                break;
            case FAMILY_TOWER:
                message.append("MAGE TOWER");

                break;
            case FAMILY_BONES:
                message.append("BONEPILE");

                break;
            case FAMILY_TREEHOUSE:
                message.append("TREEHOUSE");

                break;
            default:
                message.append("GENERATOR");

                break;
            }
        } else if (object_brush.order == ORDER_SPECIAL) {
            message.append("START_TILE");
        } else if (object_brush.order == ORDER_TREASURE) {
            message.append(treasures[object_brush.family]);
        } else if (object_brush.order == ORDER_WEAPON) {
            message.append(weapons[object_brush.family]);
        } else {
            message.append("UNKNOWN");
        }

        message.resize(50);
        scentext.write_xy(lm, L_D(curline), message, DARK_BLUE, 1);
        ++curline;

        // Level display
        message.clear();

        switch (object_brush.order) {
        case ORDER_LIVING:
        case ORDER_GENERATOR:
            buf << "LEVEL: " << object_brush.level;

            break;
        case ORDER_TREASURE:
            if ((object_brush.family == FAMILY_GOLD_BAR) || (object_brush.family == FAMILY_SILVER_BAR)) {
                buf << "VALUE: " << object_brush.level;
            } else if (object_brush.family == FAMILY_KEY) {
                buf << "DOOR ID: " << object_brush.level;
            } else if (object_brush.family == FAMILY_TELEPORTER) {
                buf << "GROUP: " << object_brush.level;
            } else if (object_brush.family == FAMILY_EXIT) {
                buf << "EXIT TO: " << object_brush.level;
            } else if (object_brush.family != FAMILY_STAIN) {
                buf << "POWER: " << object_brush.level;
            }

            break;
        case ORDER_WEAPON:
            if (object_brush.family == FAMILY_DOOR) {
                buf << "DOOR ID: " << object_brush.level;
            } else {
                buf << "POWER: " << object_brush.level;
            }

            break;
        default:

            break;
        }

        message = buf.str();
        buf.clear();

        if (!message.empty()) {
            message.resize(50);
            scentext.write_xy(lm, L_D(curline), message, DARK_BLUE, 1);
            ++curline;
        }

        numobs = myscreen->level_data.numobs;
        // myscreen->fastbox(lm, L_d(curline), 55, 7, 27, 1);
        buf << "OB: " << numobs;
        message = buf.str();
        buf.clear();
        message.resize(50);
        scentext.write_xy(lm, L_D(curline), message, DARK_BLUE, 1);
        ++curline;
    }

    if (mode == TERRAIN) {
        // Show the current brush
        myscreen->putbuffer(lm + 25, (PIX_TOP - 16) - 1, GRID_SIZE, GRID_SIZE,
                            0, 0, 320, 200,
                            myscreen->level_data.pixdata[terrain_brush.terrain].data);

        // Border
        myscreen->draw_box(lm + 25, (PIX_TOP - 16) -1, (lm + 25) + GRID_SIZE,
                           ((PIX_TOP - 16) - 1) + GRID_SIZE, RED, 0, 1);

        // Show the background grid
        for (i = 0; i < PIX_OVER; ++i) {
            for (j = 0; j < 4; ++j) {
                whichback = (i + ((j + rowsdown) * 4)) % (sizeof(backgrounds) / 4);
                myscreen->putbuffer(S_RIGHT + (i * GRID_SIZE), PIX_TOP + (j * GRID_SIZE),
                                    GRID_SIZE, GRID_SIZE, 0, 0, 320, 200,
                                    myscreen->level_data.pixdata[backgrounds[whichback]].data);
            }
        }

        myscreen->draw_box(S_RIGHT, PIX_TOP, S_RIGHT + (4 * GRID_SIZE),
                           PIX_TOP + (4 * GRID_SIZE), 0, 0, 1);

        // Draw cursor
        Sint32 mx;
        Sint32 my;
        MouseState& mymouse = query_mouse_no_poll();
        mx = mymouse.x;
        my = mymouse.y;
        bool over_radar = false;

        if ((mx > ((myscreen->viewob[0]->endx - myscreen->viewob[0]->myradar->xview) - 4))
            && (my > ((myscreen->viewob[0]->endy - myscreen->viewob[0]->myradar->yview) - 4))
            && (mx < (myscreen->viewob[0]->endx - 4))
            && (my < (myscreen->viewob[0]->endy - 4))) {
            over_radar = true;
        }

        if (!over_radar
            && !Rect(S_RIGHT, PIX_TOP, 4 * GRID_SIZE, 4 * GRID_SIZE).contains(mx, my)
            && !mouse_on_menus(mx, my)) {
            // Draw target tile
            Sint32 worldx = mx + level->topx;
            Sint32 worldy = my + level->topy;
            Sint32 gridx = worldx - (worldx % GRID_SIZE);
            Sint32 gridy = worldy - (worldy % GRID_SIZE);
            Sint32 screenx = gridx - level->topx;
            Sint32 screeny = gridy - level->topy;
            myscreen->draw_box(screenx, screeny, screenx + GRID_SIZE, screeny + GRID_SIZE, YELLOW, 0, 1);
        }
    } else if (mode == OBJECT) {
        // Draw current brush
        // Background
        myscreen->draw_box(lm + 25,
                           (PIX_TOP - 16) - 1,
                           (lm + 25) + GRID_SIZE,
                           ((PIX_TOP - 16) - 1) + GRID_SIZE,
                           PURE_BLACK, 1, 1);

        // guy
        Walker *newob = level->add_ob(ORDER_LIVING, FAMILY_ELF);
        newob->setxy((lm + 25) + level->topx, ((PIX_TOP - 16) - 1) + level->topy);
        newob->set_data(level->myloader->graphics[PIX(object_brush.order, object_brush.family)]);
        set_walker(newob, object_brush.order, object_brush.family);
        newob->team_num = object_brush.team;
        newob->draw_tile(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                         myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                         myscreen->viewob[0]->endx, myscreen->viewob[0]->endy,
                         myscreen->viewob[0]->control);

        // Border
        myscreen->draw_box(lm + 25, (PIX_TOP - 16) - 1, (lm + 25) + GRID_SIZE,
                           ((PIX_TOP - 16) - 1) + GRID_SIZE, RED, 0, 1);

        myscreen->draw_box(S_RIGHT, PIX_TOP, S_RIGHT + (4 * GRID_SIZE),
                           PIX_TOP + (4 * GRID_SIZE), PURE_BLACK, 1, 1);

        myscreen->draw_box(S_RIGHT, PIX_TOP, S_RIGHT + (4 * GRID_SIZE),
                           PIX_TOP + (4 * GRID_SIZE), WHITE, 0, 1);

        for (i = 0; i < PIX_OVER; ++i) {
            for (j = 0; j < 4; ++j) {
                Sint32 index = (i + ((j + rowsdown) * PIX_OVER)) % object_pane.size();

                if (index < static_cast<Sint32>(object_pane.size())) {
                    newob->setxy((S_RIGHT + (i * GRID_SIZE)) + level->topx,
                                 (PIX_TOP + (j * GRID_SIZE)) + level->topy);
                    newob->set_data(level->myloader->graphics[PIX(object_pane[index].order,
                                                                  object_pane[index].family)]);
                    set_walker(newob, object_pane[index].order, object_pane[index].family);
                    newob->team_num = object_brush.team;
                    newob->draw_tile(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                                     myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                                     myscreen->viewob[0]->endx, myscreen->viewob[0]->endy,
                                     myscreen->viewob[0]->control);
                }
            }
        }

        // Draw cursor
        Sint32 mx;
        Sint32 my;
        MouseState &mymouse = query_mouse_no_poll();
        mx = mymouse.x;
        my = mymouse.y;
        bool over_radar = false;

        if((mx > ((myscreen->viewob[0]->endx - myscreen->viewob[0]->myradar->xview) - 4))
           && (my > ((myscreen->viewob[0]->endy - myscreen->viewob[0]->myradar->yview) - 4))
           && (mx < (myscreen->viewob[0]->endx - 4))
           && (my < (myscreen->viewob[0]->endy - 4))) {
            over_radar = true;
        }

        bool over_info = Rect(lm - 4, L_D(-1) + 4, 315 - (lm - 4), (L_D(7) - 2) - L_D(-1)).contains(mx, my);

        if (!over_radar
            && !over_info
            && !Rect(S_RIGHT, PIX_TOP, 4 * GRID_SIZE, 4 * GRID_SIZE).contains(mx, my)
            && !mouse_on_menus(mx, my)) {
            // Prepare object sprite
            newob->setxy(mx + level->topx, my + level->topy);
            newob->set_data(level->myloader->graphics[PIX(object_brush.order, object_brush.family)]);
            set_walker(newob, object_brush.order, object_brush.family);
            newob->team_num = object_brush.team;

            // Get size rounded up to nearest GRID_SIZE
            Sint32 w = newob->sizex;
            Sint32 h = newob->sizey;

            if ((w % GRID_SIZE) == 0) {
                w += (GRID_SIZE - GRID_SIZE);
            } else {
                w += (GRID_SIZE - (w % GRID_SIZE));
            }

            if ((h % GRID_SIZE) == 0) {
                h += (GRID_SIZE - GRID_SIZE);
            } else {
                h += (GRID_SIZE - (h % GRID_SIZE));
            }

            // Draw target tile
            if (object_brush.snap_to_grid) {
                Sint32 worldx = mx + level->topx;
                Sint32 worldy = my + level->topy;
                Sint32 gridx = worldx - (worldx % GRID_SIZE);
                Sint32 gridy = worldy - (worldy % GRID_SIZE);
                Sint32 screenx = gridx - level->topx;
                Sint32 screeny = gridy - level->topy;
                myscreen->draw_box(screenx, screeny, screenx + w, screeny + h, YELLOW, 0, 1);
            }

            // Draw current brush near cursor
            newob->draw(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                        myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                        myscreen->viewob[0]->endx, myscreen->viewob[0]->endy,
                        myscreen->viewob[0]->control);
        }

        level->remove_ob(newob);
        delete newob;
    }

    // Draw top menu
    for (auto const &e : menu_buttons) {
        e->draw(myscreen);
    }

    // Draw submenus
    for (auto const &e : current_menu) {
        for (auto const &f : e.second) {
            f->draw(myscreen);
        }
    }

    myscreen->buffer_to_screen(0, 0, 320, 200);

    return 1;
}

void LevelEditorData::clear_terrain()
{
    Sint32 w = level->grid.w;
    Sint32 h = level->grid.h;

    memset(level->grid.data, 1, w * h);
    resmooth_terrain();
}

void LevelEditorData::resmooth_terrain()
{
    level->mysmoother.smooth();
    myscreen->viewob[0]->myradar->update(*level);
}

void LevelEditorData::mouse_down(Sint32 mx, Sint32 my)
{
    dragging = false;
}

void LevelEditorData::mouse_motion(Sint32 mx, Sint32 my, Sint32 dx, Sint32 dy)
{
    MouseState &mymouse = query_mouse_no_poll();

    if (mymouse.left) {
        if ((mode == SELECT) && !mouse_on_menus(mouse_last_x, mouse_last_y)) {
            Sint32 worldx = (mx + level->topx) - myscreen->viewob[0]->xloc; // - S_LEFT
            Sint32 worldy = (my + level->topy) - myscreen->viewob[0]->yloc; // - S_UP

            Walker *under_cursor = nullptr;

            if (!dragging && !rect_selecting) {
                // Did we start dragging a selected object?
                under_cursor = get_object(worldx, worldy);

                Walker *got_one = nullptr;

                for (auto const &e : selection) {
                    if (e.target == under_cursor) {
                        got_one = under_cursor;

                        break;
                    }
                }

                under_cursor = got_one;
            }

            if ((dragging || (under_cursor != nullptr)) && !selection.empty()) {
                // Drag the selected objects
                dragging = true;

                for (auto &&e : selection) {
                    Walker *w = e.get_object(level);

                    if (w != nullptr) {
                        w->setxy(w->xpos + dx, w->ypos + dy);

                        // Update selection position
                        e.x = w->xpos;
                        e.y = w->ypos;
                    }
                }
            }

            if (!dragging) {
                // Select with a rectangle
                float worldx = (mx + level->topx) - myscreen->viewob[0]->xloc;
                float worldy = (my + level->topy) - myscreen->viewob[0]->yloc;

                if (!rect_selecting) {
                    selection_rect.x = worldx;
                    selection_rect.y = worldy;
                    selection_rect.w = 1;
                    selection_rect.h = 1;
                    rect_selecting = true;
                }

                selection_rect.w = worldx - selection_rect.x;
                selection_rect.h = worldy - selection_rect.y;
            }
        }
    }
}

void LevelEditorData::mouse_up(Sint32 mx, Sint32 my, Sint32 old_mx, Sint32 old_my, bool &done)
{
    if (dragging) {
        dragging = false;

        return;
    }

    bool mouse_on_menu = mouse_on_menus(mx, my);
    bool old_mouse_on_menu = mouse_on_menus(old_mx, old_my);
    bool on_menu = mouse_on_menu && old_mouse_on_menu;
    bool off_menu = !mouse_on_menu && !old_mouse_on_menu;

    // Clicking on menu items
    if (on_menu) {
        // FILE
        if (activate_sub_menu_button(mx, my, current_menu, fileButton, true)) {
            std::set<SimpleButton *> s = {
                &fileCampaignButton,
                &fileLevelButton,
                &fileQuitButton
            };
            current_menu.push_back(std::make_pair(&fileButton, s));
        } else if (activate_sub_menu_button(mx, my, current_menu, fileCampaignButton)) {
            // Campaign >
            std::set<SimpleButton *> s = {
                // &fileCampaignImportButton,
                // &fileCampaignShareButton,
                &fileCampaignNewButton,
                &fileCampaignLoadButton,
                &fileCampaignSaveButton,
                &fileCampaignSaveAsButton
            };
            current_menu.push_back(std::make_pair(&fileCampaignButton, s));
        } else if (activate_menu_choice(mx, my, *this, fileCampaignImportButton)) {
            bool cancel = false;

            if (levelchanged) {
                cancel = !yes_or_no_prompt("Import", "Discard unsaved level changes?", false);
            }

            if (campaignchanged) {
                cancel = !yes_or_no_prompt("Import", "Discard unsaved campaign changed?", false);
            }

            if (!cancel) {
                popup_dialog("Import Campaign", "Not yet implemented.");
                importCampaignPicker();
            }
        } else if (activate_menu_choice(mx, my, *this, fileCampaignShareButton)) {
            bool cancel = false;

            if (levelchanged) {
                if (yes_or_no_prompt("Share", "Save level first?", false)) {
                    if (saveLevel()) {
                        timed_dialog("Level saved.");
                        redraw = 1;
                        levelchanged = 0;
                    } else {
                        timed_dialog("Save failed.");
                        redraw = 1;

                        cancel = true;
                    }
                }
            }

            if (campaignchanged) {
                if (yes_or_no_prompt("Share", "Save campaign first?", false)) {
                    if (saveCampaign()) {
                        timed_dialog("Campaign saved.");
                        redraw = 1;
                        campaignchanged = 0;
                    } else {
                        timed_dialog("Save failed.");
                        redraw = 1;

                        cancel = true;
                    }
                }
            }

            if (!cancel) {
                popup_dialog("Share Campaign", "Not yet implemented.");
                shareCampaign(myscreen);
            }
        } else if (activate_menu_choice(mx, my, *this, fileCampaignNewButton)) {
            // Confirm if unsaved
            bool cancel = false;

            if (levelchanged) {
                cancel = !yes_or_no_prompt("New Campaign", "Discard unsaved changes?", false);
            }

            if (!cancel) {
                // Ask for campaign ID
                std::string campaign("com.example.new_campaign");

                if (prompt_for_string("New Campaign", campaign)) {
                    // TODO: Check if campaign already exists and prompt the user to overwrite
                    if (does_campaign_exist(campaign) && !yes_or_no_prompt("Overwrite?", "Overwrite existing campaign with that ID?", false)) {
                        cancel = true;
                    }

                    if (!cancel) {
                        if (create_new_campaign(campaign)) {
                            // Load campaign data for the editor
                            if (loadCampaign(campaign)) {
                                // Mount new campaign
                                unmount_campaign_package(get_mounted_campaign());
                                mount_campaign_package(campaign);

                                // Load first scenario
                                std::list<Sint32> levels = list_levels();

                                if (!levels.empty()) {
                                    loadLevel(levels.front());
                                    // Update minimap
                                    myscreen->viewob[0]->myradar->start(
                                        myscreen->level_data,
                                        myscreen->viewob[0]->endx,
                                        myscreen->viewob[0]->endy,
                                        myscreen->viewob[0]->yloc);
                                    timed_dialog("Campaign created.");
                                    campaignchanged = 0;
                                    levelchanged = 0;
                                } else {
                                    timed_dialog("Campaign has no scenarios!");
                                    redraw = 1;
                                }
                            } else {
                                timed_dialog("Failed to load new campaign.");
                                redraw = 1;
                            }
                        } else {
                            timed_dialog("Failed to create new campaign.");
                            redraw = 1;
                        }
                    }
                }
            }
        } else if (activate_menu_choice(mx, my, *this, fileCampaignLoadButton)) {
            // Pick a campaign, then load it and load the first level
            redraw = 1;
            bool cancel = false;

            if (campaignchanged) {
                cancel = !yes_or_no_prompt("Load Campaign", "Discard unsaved changes?", false);
            }

            if (!cancel) {
                CampaignResult result = pick_campaign(nullptr, true);

                if (!result.id.empty()) {
                    if (loadCampaign(result.id)) {
                        unmount_campaign_package(get_mounted_campaign());
                        mount_campaign_package(result.id);
                        campaignchanged = 0;
                    } else {
                        timed_dialog("Failed to load campaign.");
                        cancel = true;
                    }

                    if (!cancel) {
                        myscreen->clearbuffer();

                        // Prompt to load starting level. If we don't, then the
                        // user can transfer levels between campaigns here.
                        bool load_first_level = yes_or_no_prompt("Load Campaign", "Load first level?", false);

                        if (load_first_level && levelchanged) {
                            load_first_level = yes_or_no_prompt("Load Level", "Discard unsaved changes?", false);
                        }

                        if (load_first_level) {
                            // Load first scenario
                            if (loadLevel(result.first_level)) {
                                // Update minimap
                                myscreen->viewob[0]->myradar->start(myscreen->level_data,
                                                                    myscreen->viewob[0]->endx,
                                                                    myscreen->viewob[0]->endy,
                                                                    myscreen->viewob[0]->yloc);
                                timed_dialog("Campaign loaded.");
                                levelchanged = 0;
                            } else {
                                timed_dialog("Failed to load first level.");
                            }
                        } else {
                            timed_dialog("Campaign loaded.");
                        }
                    }
                }
            }
        } else if (activate_menu_choice(mx, my, *this, fileCampaignSaveButton)) {
            if (saveCampaign()) {
                timed_dialog("Campaign saved.");
                campaignchanged = 0;
                redraw = 1;
            } else {
                timed_dialog("Failed to save campaign.");
                redraw = 1;
            }
        } else if (activate_menu_choice(mx, my, *this, fileCampaignSaveAsButton)) {
            CampaignResult result = pick_campaign(nullptr, true);

            if (!result.id.empty()) {
                std::list<std::string> campaigns = list_campaigns();

                if ((std::find(campaigns.begin(), campaigns.end(), result.id) == campaigns.end())
                    || yes_or_no_prompt("Overwrite", "Overwrite existing campaign?", false)) {
                    if (saveCampaignAs(result.id)) {
                        timed_dialog("Campaign saved.");
                        campaignchanged = 0;
                        redraw = 1;
                    } else {
                        timed_dialog("Failed to save campaign.");
                        redraw = 1;
                    }
                }
            }
        } else if (activate_sub_menu_button(mx, my, current_menu, fileLevelButton)) {
            // Level >
            std::set<SimpleButton *> s = {
                &fileLevelNewButton,
                &fileLevelLoadButton,
                &fileLevelSaveButton,
                &fileLevelSaveAsButton
            };
            current_menu.push_back(std::make_pair(&fileLevelButton, s));
        } else if (activate_menu_choice(mx, my, *this, fileLevelNewButton)) {
            // Confirm if unsaved
            bool cancel = false;

            if (levelchanged) {
                cancel = !yes_or_no_prompt("Load Level", "Discard unsaved changes?", false);
            }

            if (!cancel) {
                // New level
                level->clear();
                level->create_new_grid();
                myscreen->viewob[0]->myradar->start(myscreen->level_data,
                                                    myscreen->viewob[0]->endx,
                                                    myscreen->viewob[0]->endy,
                                                    myscreen->viewob[0]->yloc);
                levelchanged = 1;
                redraw = 1;
            }
        } else if (activate_menu_choice(mx, my, *this, fileLevelLoadButton)) {
            // Confirm if unsaved
            bool cancel = false;

            if (levelchanged) {
                cancel = !yes_or_no_prompt("Load Level", "Discard unsaved changes?", false);
            }

            if (!cancel) {
                // Browse for the level to load
                Sint32 id = pick_level(level->id, true);
                // Don't bother loading the level if it is the same, unchanged level
                if ((id >= 0) && (levelchanged || (id != level->id))) {
                    if (loadLevel(id)) {
                        timed_dialog("Level loaded.");
                        levelchanged = 0;
                        redraw = 1;
                    } else {
                        timed_dialog("Failed to load level.");
                        redraw = 1;
                    }

                    myscreen->viewob[0]->myradar->start(myscreen->level_data,
                                                        myscreen->viewob[0]->endx,
                                                        myscreen->viewob[0]->endy,
                                                        myscreen->viewob[0]->yloc);
                    redraw = 1;
                }
            }
        } else if (activate_menu_choice(mx, my, *this, fileLevelSaveButton)) {
            if (saveLevel()) {
                timed_dialog("Level saved.");
                redraw = 1;
                levelchanged = 0;
            } else {
                timed_dialog("Save failed.");
                redraw = 1;
            }
        } else if (activate_menu_choice(mx, my , *this, fileLevelSaveAsButton)) {
            Sint32 id = pick_level(level->id, true);

            if ((id >= 0) && (id != level->id)) {
                std::list<Sint32> levels = list_levels();

                if ((std::find(levels.begin(), levels.end(), id) == levels.end())
                    || yes_or_no_prompt("Overwrites", "Overwrite existing level?", false)) {
                    if (saveLevelAs(id)) {
                        timed_dialog("Level saved.");
                        redraw = 1;
                        levelchanged = 0;
                    } else {
                        timed_dialog("Save failed.");
                        redraw = 1;
                    }
                }
            }
        } else if (activate_menu_choice(mx, my, *this, fileQuitButton)) {
            if ((!levelchanged && !campaignchanged) || yes_or_no_prompt("Exit", "Quit without saving?", false)) {
                done = true;
            }
        } else if (activate_sub_menu_button(mx, my, current_menu, campaignButton, true)) {
            // CAMPAIGN
            std::set<SimpleButton *>s = {
                &campaignInfoButton,
                &campaignProfileButton,
                &campaignDetailsButton,
                &campaignValidateButton
            };
            current_menu.push_back(std::make_pair(&campaignButton, s));
        } else if (activate_menu_choice(mx, my, *this, campaignInfoButton)) {
            std::stringstream buf;

            if (campaignchanged) {
                buf << "(unsaved)";
            }

            buf << std::endl
                << "ID: " << campaign->id << std::endl
                << "Title: " << campaign->title << std::endl
                << "Version: " << campaign->version << std::endl
                << "Authors: " << campaign->authors << std::endl
                << "Contributors: " << campaign->contributors << std::endl
                << "Sugg. Power: " << campaign->suggested_power << std::endl
                << "First level: " << campaign->first_level;

            std::string str(buf.str());
            buf.clear();
            str.resize(512);

            popup_dialog("Campaign Info", str.c_str());
        } else if (activate_sub_menu_button(mx, my, current_menu, campaignProfileButton)) {
            // Profile >
            std::set<SimpleButton *> s = {
                &campaignProfileTitleButton,
                &campaignProfileDescriptionButton,
                // &campaignProfileIconButton,
                &campaignProfileAuthorsButton,
                &campaignProfileContributorsButton
            };
            current_menu.push_back(std::make_pair(&campaignProfileButton, s));
        } else if (activate_menu_choice(mx, my, *this, campaignProfileTitleButton)) {
            std::string title(campaign->title);

            if (prompt_for_string("Campaign Title", title)) {
                campaign->title = title;
                campaignchanged = 1;
            }
        } else if (activate_menu_choice(mx, my, *this, campaignProfileDescriptionButton)) {
            std::list<std::string> desc = campaign->description;

            if (prompt_for_string_block("Campaign Description", desc)) {
                campaign->description = desc;
                campaignchanged = 1;
            }

            redraw = 1;
        } else if (activate_menu_choice(mx, my, *this, campaignProfileIconButton)) {
            popup_dialog("Edit Icon", "Not yet implemented.");
        } else if(activate_menu_choice(mx, my, *this, campaignProfileAuthorsButton)) {
            std::string authors(campaign->authors);

            if (prompt_for_string("Campaign Authors", authors)) {
                campaign->authors = authors;
                campaignchanged = 1;
            }
        } else if (activate_menu_choice(mx, my, *this, campaignProfileContributorsButton)) {
            std::string contributors(campaign->contributors);

            if (prompt_for_string("Campaign Contributors", contributors)) {
                campaign->contributors = contributors;
                campaignchanged = 1;
            }
        } else if (activate_sub_menu_button(mx, my, current_menu, campaignDetailsButton)) {
            // Details >
            std::set<SimpleButton *> s = {
                &campaignDetailsVersionButton,
                &campaignDetailsSuggestedPowerButton,
                &campaignDetailsFirstLevelButton
            };
            current_menu.push_back(std::make_pair(&campaignDetailsButton, s));
        } else if (activate_menu_choice(mx, my, *this, campaignDetailsVersionButton)) {
            std::string version(campaign->version);

            if (prompt_for_string("Campaign Version", version)) {
                campaign->version = version;
                campaignchanged = 1;
            }
        } else if (activate_menu_choice(mx, my, *this, campaignDetailsSuggestedPowerButton)) {
            std::stringstream buf;

            buf << campaign->suggested_power;

            std::string power(buf.str());
            buf.clear();
            power.resize(20);

            if (prompt_for_string("Suggested Power", power)) {
                campaign->suggested_power = std::stoi(power);
                campaignchanged = 1;
            }
        } else if (activate_menu_choice(mx, my, *this, campaignDetailsFirstLevelButton)) {
            std::stringstream buf;

            buf << campaign->first_level;

            std::string level(buf.str());
            buf.clear();
            level.resize(20);

            if (prompt_for_string("First Level", level)) {
                campaign->first_level = std::stoi(level);
                campaignchanged = 1;
            }
        } else if (activate_menu_choice(mx, my, *this, campaignValidateButton)) {
            std::list<Sint32> levels = list_levels();
            std::set<Sint32> connected;
            std::list<std::string> problems;

            // Are the levels all connected to the first level?
            Sint32 current_level = campaign->first_level;
            get_connected_level_exits(current_level, levels, connected, problems);

            for (auto const &e : levels) {
                if (connected.find(e) == connected.end()) {
                    std::stringstream buf("Level ");

                    buf << e << " is not connected.";

                    std::string problem(buf.str());
                    buf.clear();
                    problem.resize(40);
                    problems.push_back(problem);
                }
            }

            // Get ready to show the user the problems
            std::stringstream buf;

            if (problems.empty()) {
                buf << "No problems!";
            } else {
                // Only show the first 6 problems and "More problems..."
                if (problems.size() > 6) {
                    Sint32 num_over = problems.size() - 6;

                    while (problems.size() > 6) {
                        problems.pop_back();
                    }

                    std::stringstream buf2;

                    buf2 << num_over << " more problems...";

                    std::string problem(buf2.str());
                    buf2.clear();
                    problem.resize(40);

                    problems.push_back(problem);
                }

                // Put all the problems together for the printer
                for (auto const &e : problems) {
                    buf << e << std::endl;
                }
            }

            std::string issues(buf.str());
            buf.clear();
            issues.resize(512);

            // Show the user the problems
            popup_dialog("Validate Campaign", issues);
        } else if (activate_sub_menu_button(mx, my, current_menu, levelButton, true)) {
            // LEVEL
            std::set<SimpleButton *> s = {
                &levelInfoButton,
                &levelProfileButton,
                &levelDetailsButton,
                &levelGoalsButton,
                &levelResmoothButton,
                &levelDeleteTerrainButton,
                &levelDeleteObjectsButton
            };
            current_menu.push_back(std::make_pair(&levelButton, s));
        } else if (activate_menu_choice(mx, my, *this, levelInfoButton)) {
            std::stringstream buf;

            if (levelchanged) {
                buf << "(unsaved)";
            }

            buf << std::endl
                << "ID number: " << level->id << std::endl
                << "Title: " << level->title << std::endl
                << "Size: " << level->grid.w << "x" << level->grid.h;

            std::string info(buf.str());
            buf.clear();
            info.resize(512);

            popup_dialog("Level Info", info);
        } else if (activate_sub_menu_button(mx, my, current_menu, levelProfileButton)) {
            // Profile >
            std::set<SimpleButton *> s = {
                &levelProfileTitleButton,
                &levelProfileDescriptionButton
            };
            current_menu.push_back(std::make_pair(&levelProfileButton, s));
        } else if (activate_menu_choice(mx, my, *this, levelProfileTitleButton)) {
            std::string title(level->title);

            if (prompt_for_string("Level Title", title)) {
                level->title = title;
                levelchanged = 1;
            }
        } else if (activate_menu_choice(mx, my, *this, levelProfileDescriptionButton)) {
            std::list<std::string> desc = level->description;

            if (prompt_for_string_block("Level Description", desc)) {
                level->description = desc;
                levelchanged = 1;
            }

            redraw = 1;
        } else if (activate_sub_menu_button(mx, my, current_menu, levelDetailsButton)) {
            // Details >
            std::set<SimpleButton *> s = {
                &levelDetailsMapSizeButton,
                &levelDetailsParValueButton,
                &levelDetailsTimeLimitButton
            };
            current_menu.push_back(std::make_pair(&levelDetailsButton, s));
        } else if (activate_menu_choice(mx, my, *this, levelDetailsMapSizeButton)) {
            // Using two prompts sequentially

            std::stringstream buf;

            buf << level->grid.w;

            std::string width(buf.str());
            buf.clear();
            width.resize(20);

            buf << level->grid.h;

            std::string height(buf.str());
            buf.clear();
            height.resize(20);

            if (prompt_for_string("Map Width", width)) {
                Sint32 w = std::stoi(width);
                Sint32 h;

                if (prompt_for_string("Map Height", height)) {
                    h = std::stoi(height);

                    // Validate here so we can tell the user
                    // size is limited to one byte in the file format
                    if ((w < 3) || (h < 3) || (w > 255) || (h > 255)) {
                        buf << "Can't resize grid to " << w << "x" << h << std::endl;

                        if (w < 3) {
                            buf << "Width is too small." << std::endl;
                        }

                        if (h < 3) {
                            buf << "Height is too small." << std::endl;
                        }

                        if (w > 255) {
                            buf << "Width is too big (max 255)." << std::endl;
                        }

                        if (h > 255) {
                            buf << "Height is too big (max 255)." << std::endl;
                        }

                        std::string msg(buf.str());
                        buf.clear();
                        msg.resize(200);

                        popup_dialog("Resize Map", msg);
                    } else {
                        if (((w >= level->grid.w) && (h >= level->grid.h))
                            || !are_objects_outside_area(level, 0, 0, w, h)
                            || yes_or_no_prompt("Resize Map", "Delete objects outside of map?", false)) {
                            // Now change it
                            level->resize_grid(w, h);

                            // Reset the minimap
                            myscreen->viewob[0]->myradar->start(myscreen->level_data,
                                                                myscreen->viewob[0]->endx,
                                                                myscreen->viewob[0]->endy,
                                                                myscreen->viewob[0]->yloc);

                            draw(myscreen);
                            myscreen->refresh();

                            buf << "Resized map to " << level->grid.w << "x" << level->grid.h;

                            std::string msg(buf.str());
                            buf.clear();
                            msg.resize(30);

                            timed_dialog(msg);
                            redraw = 1;
                            levelchanged = 1;
                        } else {
                            timed_dialog("Resize canceled.");
                            redraw = 1;
                        }
                    }
                } else {
                    timed_dialog("Resize cancled.");
                    redraw = 1;
                }
            } else {
                timed_dialog("Resize canceled.");
                redraw = 1;
            }
        } else if (activate_sub_menu_button(mx, my, current_menu, levelGoalsButton)) {
            // Goals >
            std::set<SimpleButton *> s = {
                &levelGoalsEnemiesButton,
                &levelGoalsGeneratorsButton,
                &levelGoalsNPCsButton
            };
            current_menu.push_back(std::make_pair(&levelGoalsButton, s));
        } else if (activate_menu_toggle_choice(mx, my, *this, levelGoalsEnemiesButton)) {
            level->type ^= LevelData::TYPE_CAN_EXIT_WHENEVER;
            update_menu_buttons();
        } else if (activate_menu_toggle_choice(mx, my, *this, levelGoalsGeneratorsButton)) {
            level->type ^= LevelData::TYPE_MUST_DESTROY_GENERATORS;
            update_menu_buttons();
        } else if (activate_menu_toggle_choice(mx, my, *this, levelGoalsNPCsButton)) {
            level->type ^= LevelData::TYPE_MUST_PROTECT_NAMED_NPCS;
            update_menu_buttons();
        } else if (activate_menu_choice(mx, my, *this, levelDetailsParValueButton)) {
            std::stringstream buf;

            buf << level->par_value;

            std::string par(buf.str());
            buf.clear();
            par.resize(20);

            if (prompt_for_string("Par Value (num)", par)) {
                Sint32 v = std::stoi(par);

                if (v > 0) {
                    level->par_value = v;
                    levelchanged = 1;
                }
            }
        } else if (activate_menu_choice(mx, my, *this, levelDetailsTimeLimitButton)) {
            std::stringstream buf;

            buf << level->time_bonus_limit;

            std::string limit(buf.str());
            buf.clear();
            limit.resize(20);

            if (prompt_for_string("Time Bonus Limit (num)", limit)) {
                Sint32 v = std::stoi(limit);

                if (v > 0) {
                    level->time_bonus_limit = v;
                    levelchanged = 1;
                }
            }
        } else if (activate_menu_choice(mx, my, *this, levelResmoothButton)) {
            resmooth_terrain();
            levelchanged = 1;
            redraw = 1;
        } else if (activate_menu_choice(mx, my, *this, levelDeleteTerrainButton)) {
            if (yes_or_no_prompt("Clear Terrain", "Delete all terrain?", false)) {
                clear_terrain();
                myscreen->viewob[0]->myradar->update(*level);
                levelchanged = 1;
            }
            redraw = 1;
        } else if (activate_menu_choice(mx, my, *this, levelDeleteObjectsButton)) {
            if (yes_or_no_prompt("Clear Objects", "Delete all objects?", false)) {
                level->delete_objects();
                myscreen->viewob[0]->myradar->update(*level);
                levelchanged = 1;
            }

            redraw = 1;
        } else if (activate_sub_menu_button(mx, my, current_menu, modeButton, true)) {
            std::set<SimpleButton *> s = {
                &modeTerrainButton,
                &modeObjectButton,
                &modeSelectButton
            };
            current_menu.push_back(std::make_pair(&modeButton, s));
        } else if (activate_menu_choice(mx, my, *this, modeTerrainButton)) {
            mode = TERRAIN;
            modeButton.label = "Edit (Terrain)";
            reset_mode_buttons();
        } else if (activate_menu_choice(mx, my, *this, modeObjectButton)) {
            mode = OBJECT;
            modeButton.label = "Edit (Objects)";
            reset_mode_buttons();
        } else if (activate_menu_choice(mx, my, *this, modeSelectButton)) {
            mode = SELECT;
            modeButton.label = "Edit (Select)";
            reset_mode_buttons();
        } else {
            // Check mode-specific buttons
            for (auto const &e : mode_buttons) {
                if (e->contains(mx, my)) {
                    activate_mode_button(e);
                    redraw = 1;

                    break;
                }
            }
        }
    } else {
        // Either press or release was off of the menus
        // Close open menus
        if (!current_menu.empty()) {
            current_menu.clear();
        }
    }

    if (off_menu) {
        // cliced and released off the menu
        // Zardus: ADD: Can move map by clicking on minimap
        if (((mode != SELECT) || (!rect_selecting && !dragging))
            && (mx > ((myscreen->viewob[0]->endx - myscreen->viewob[0]->myradar->xview) - 4))
            && (my > ((myscreen->viewob[0]->endy - myscreen->viewob[0]->myradar->yview) - 4))
            && (mx < (myscreen->viewob[0]->endx - 4))
            && (my < (myscreen->viewob[0]->endy - 4))) {
            // Radar clicking is done by holding (in the level_editor function)
        } else {
            // In the main window
            Sint32 windowx = (mx + level->topx) - myscreen->viewob[0]->xloc; // - S_LEFT
            Sint32 windowy = (my + level->topy) - myscreen->viewob[0]->yloc; // - S_UP

            if (object_brush.snap_to_grid) {
                windowx -= (windowx % GRID_SIZE);
                windowy -= (windowy % GRID_SIZE);
            }

            if (mode == SELECT) {
                Walker *newob = nullptr;

                if (rect_selecting && ((fabs(selection_rect.w) > 15) || (fabs(selection_rect.h > 15)))) {
                    rect_selecting = false;

                    // Select guys in the rectangle
                    if (!keystates[KEYSTATE_LCTRL] && !keystates[KEYSTATE_RCTRL]) {
                        selection.clear();
                    }

                    add_contained_objects_to_selection(level, selection_rect, selection);
                    reset_mode_buttons();
                } else if (keystates[KEYSTATE_r]) {
                    // (Re)name the current object
                    newob = level->add_ob(ORDER_LIVING, FAMILY_ELF);
                    newob->setxy(windowx, windowy);

                    if (some_hit(windowx, windowy, newob, level)) {
                        std::string name(newob->collide_ob->stats.name);

                        if (prompt_for_string("Rename", name)) {
                            name.resize(11);
                            newob->collide_ob->stats.name = name;
                            levelchanged = 1;
                        }
                    }

                    level->remove_ob(newob);

                    delete newob;
                } else {
                    // Select this object
                    rect_selecting = false;

                    if ((mx < (245 - 4)) || (my > (L_D(7) - 2))) {
                        newob = level->add_ob(ORDER_LIVING, FAMILY_ELF);
                        newob->setxy(windowx, windowy);

                        if (some_hit(windowx, windowy, newob, level)) {
                            // clicked on a guy
                            Walker *w = newob->collide_ob;

                            if (keystates[KEYSTATE_LCTRL] || keystates[KEYSTATE_RCTRL]) {
                                // Select/deselect another guy
                                bool deselected = false;
                                for (std::vector<SelectionInfo>::iterator e = selection.begin(); e != selection.end(); e++) {
                                    // Identify the guy. Not the best way...
                                    if ((e->x == w->xpos)
                                        && (e->y == w->ypos)
                                        && (e->x == w->sizex)
                                        && (e->h == w->sizey)) {
                                        deselected = true;
                                        selection.erase(e);

                                        break;
                                    }
                                }

                                if (!deselected) {
                                    selection.push_back(SelectionInfo(w));
                                }
                            } else {
                                // Choose a single guy
                                selection.clear();
                                selection.push_back(SelectionInfo(w));
                            }
                        } else if (!keystates[KEYSTATE_LCTRL] && !keystates[KEYSTATE_RCTRL]) {
                            // Deselect if not trying to grabe more
                            selection.clear();
                        }

                        level->remove_ob(newob);

                        delete newob;

                        reset_mode_buttons();
                    }
                } // End of info mode
            } else if (mode == OBJECT) {
                if ((mx >= S_RIGHT) && (my >= PIX_TOP) && (my <= PIX_BOTTOM)) {
                    // windowx = (mx - PIX_LEFT) / GRID_SIZE;
                    windowx = (mx - S_RIGHT) / GRID_SIZE;
                    windowy = (my - PIX_TOP) / GRID_SIZE;
                    Sint32 index = (windowx + ((windowy + rowsdown) * PIX_OVER)) % object_pane.size();

                    if (index < static_cast<Sint32>(object_pane.size())) {
                        object_brush.order = object_pane[index].order;
                        object_brush.family = object_pane[index].family;
                    }

                    // End of background grid window
                } else if ((mx < (245 -4)) || (my > (L_D(7) - 2))) {
                    Walker *newob = nullptr;

                    if (!object_brush.picking) {
                        // Create new object here (apply brush)
                        levelchanged = 1;
                        newob = level->add_ob(object_brush.order, object_brush.family);
                        newob->setxy(windowx, windowy);
                        newob->team_num = object_brush.team;
                        newob->stats.level = object_brush.level;
                        newob->dead = 0; // Just in case
                        newob->collide_ob = 0;

                        // Is there already something there?
                        if (object_brush.snap_to_grid && some_hit(windowx, windowy, newob, level)) {
                            if (newob) {
                                level->remove_ob(newob);
                                delete newob;
                                newob = nullptr;
                            }

                            // End of failure to put guy
                        } else if (!object_brush.snap_to_grid) {
                            newob->draw(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                                        myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                                        myscreen->viewob[0]->endx, myscreen->viewob[0]->endy,
                                        myscreen->viewob[0]->control);
                            myscreen->buffer_to_screen(0, 0, 320, 200);
                            start_time_s = query_timer();
                            MouseState &mymouse = query_mouse_no_poll();

                            while (mymouse.left && ((query_timer() - start_time_s) < 36)) {
                                mymouse = query_mouse();
                            }

                            levelchanged = 1;
                        }
                    } else {
                        pick_by_mouse(mx, my);
                        object_brush.picking = false;
                        pickerButton.set_colors_normal();
                    }
                }

                // End of putting a guy
            } else if (mode == TERRAIN) {
                if ((mx >= S_RIGHT) && (my >= PIX_TOP) && (my <= PIX_BOTTOM)) {
                    // windowx = (mx - PIX_LEFT) / GRID_SIZE;
                    windowx = (mx - S_RIGHT) / GRID_SIZE;
                    windowy = (my - PIX_TOP) / GRID_SIZE;
                    terrain_brush.terrain = backgrounds[(windowx + ((windowy + rowsdown) * PIX_OVER)) % (sizeof(backgrounds) / 4)];

                    terrain_brush.terrain %= NUM_BACKGROUNDS;

                    // End of background grid window
                } else {
                    windowx /= GRID_SIZE; // Get the map position...
                    windowy /= GRID_SIZE;

                    // Terrain painting is done by holding in level_editor()

                    if (terrain_brush.picking) {
                        // Set brush to the grid tile
                        pick_by_mouse(mx, my);
                        terrain_brush.picking = false;
                        pickerButton.set_colors_normal();
                    }
                }

                // End of setting grid square
            }

            // End of main window
        }
    }
}

void LevelEditorData::pick_by_mouse(Sint32 mx, Sint32 my)
{
    Sint32 windowx = (mx + level->topx) - myscreen->viewob[0]->xloc; // - S_LEFT
    Sint32 windowy = (my + level->topy) - myscreen->viewob[0]->yloc; // - S_UP

    // Set brush to the grid tile
    if (mode == TERRAIN) {
        // Snap to grid
        windowx -= (windowx % GRID_SIZE);
        windowy -= (windowy % GRID_SIZE);

        // Reduce to array dims
        windowx /= GRID_SIZE;
        windowy /= GRID_SIZE;

        // Get tile from grid array
        if (is_in_grid(windowx, windowy)) {
            terrain_brush.terrain = get_terrain(windowx, windowy);
        }
    } else if (mode == OBJECT) {
        // Snap to grid
        if (object_brush.snap_to_grid) {
            windowx -= (windowx % GRID_SIZE);
            windowy -= (windowy % GRID_SIZE);
        }

        // Get object from level
        Walker *w = get_object(windowx, windowy);

        if (w != nullptr) {
            object_brush.set(w);
        }
    }
}

bool LevelEditorData::is_in_grid(Sint32 x, Sint32 y)
{
    return ((x >= 0) && (y >= 0) && (x < level->grid.w) && (y < level->grid.h));
}

Uint8 LevelEditorData::get_terrain(Sint32 x, Sint32 y)
{
    if (!is_in_grid(x, y)) {
        return 0;
    }

    return level->grid.data[(y * level->grid.w) + x];
}

void LevelEditorData::set_terrain(Sint32 x, Sint32 y, Uint8 terrain)
{
    if (!is_in_grid(x, y)) {
        return;
    }

    level->grid.data[(y * level->grid.w) + x] = terrain;
}

Walker *LevelEditorData::get_object(Sint32 x, Sint32 y)
{
    Walker *result = nullptr;
    Walker *newob = level->add_ob(ORDER_LIVING, FAMILY_ELF);

    newob->setxy(x, y);

    if (some_hit(x, y, newob, level)) {
        result = newob->collide_ob;
    }

    level->remove_ob(newob);

    delete newob;

    return result;
}
