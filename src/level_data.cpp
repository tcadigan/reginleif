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
#include "level_data.hpp"

#include <algorithm>
#include <string>

#include "yam.h"

#include "gloader.hpp"
#include "pixie.hpp"
#include "screen.hpp"
#include "smooth.hpp"
#include "stats.hpp"
#include "view.hpp"
#include "walker.hpp"

#define VERSION_NUM (char)9 // Save scenario type info

CampaignData::CampaignData(std::string const &id)
    : id(id)
    , title("New Campaign")
    , rating(0.0f)
    , version("1.0")
    , suggested_power(0)
    , first_level(1)
    , num_levels(0)
    , icon(nullptr)
{
    description.push_back("No description.");
}

CampaignData::~CampaignData()
{
    delete icon;
    icondata.free();
}

bool CampaignData::load()
{
    std::string old_campaign(get_mounted_campaign());
    unmount_campaign_package(old_campaign);

    // Load the campaign data from <user_data>/scen/<id>.glad
    if (mount_campaign_package(id)) {
        SDL_RWops *rwops = open_read_file("campaign.yaml");

        Yam yam;
        yam.set_input(rwops_read_handler, rwops);

        while (yam.parse_next() == Yam::OK) {
            switch (yam.event.type) {
            case Yam::PAIR:
                if (strcmp(yam.event.scalar, "title") == 0) {
                    title = yam.event.value;
                } else if (strcmp(yam.event.scalar, "version") == 0) {
                    version = yam.event.value;
                } else if (strcmp(yam.event.scalar, "authors") == 0) {
                    authors = yam.event.value;
                } else if (strcmp(yam.event.scalar, "contributors") == 0) {
                    contributors = yam.event.value;
                } else if (strcmp(yam.event.scalar, "description") == 0) {
                    std::string desc(yam.event.value);
                    description = explode(desc, '\n');
                } else if (strcmp(yam.event.scalar, "suggested_power") == 0) {
                    suggested_power = std::stoi(yam.event.value);
                } else if (strcmp(yam.event.scalar, "first_level") == 0) {
                    first_level = std::stoi(yam.event.value);
                }

                break;
            default:

                break;
            }
        }

        yam.close_input();
        SDL_RWclose(rwops);

        // TODO: Get rating from website
        rating = 0.0f;

        std::string icon_file("icon.pix");
        icondata = read_pixie_file(icon_file.c_str());

        if (icondata.valid()) {
            icon = new Pixie(icondata);
        }

        // Count the number of levels
        std::list<Sint32> levels = list_levels();
        num_levels = levels.size();

        unmount_campaign_package(id);
    }

    mount_campaign_package(old_campaign);

    return true;
}

bool Campaigndata::save()
{
    cleanup_unpacked_campaign();

    bool result = true;

    if (unpack_campaign(id)) {
        // Unmount campaign while it is changed
        // unmount_campaign_package(ascreen->current_campaign);

        SDL_RWops *outfile = open_write_file("temp/campaign.haml");

        if (outfile != nullptr) {
            std::stringstream buf;
            std::string temp;

            Yam yam;
            yam.set_output(rwops_write_handler, outfile);

            yam.emit_pair("format_version", "1");
            yam.emit_pair("title", title.c_str());
            yam.emit_pair("version", version.c_str());

            buf << first_level;
            temp = buf.str();
            buf.clear();
            temp.resize(40);

            yam.emit_pair("first_level", buf);

            buf << suggested_power;
            temp = buf.str();
            buf.clear();
            temp.resize(40);

            yam.emit_pair("suggested_power", buf);

            yam.emit_pair("authors", authors.c_str());
            yam.emit_pair("contributors", contributors.c_str());

            std::string desc;

            std::list<std::string>::const_iterator itr = description.begin();

            while (itr != description.end()) {
                desc += *itr;
                itr++;

                if (itr != description.end()) {
                    desc += "\n";
                }
            }

            yam.emit_pair("description", desc.c_str());

            yam.close_output();
            SDL_RWclose(outfile);
        } else {
            Log("Couldn't open temp/campaign.yaml for writing.\n");
            result = false;
        }

        if (result) {
            if (repack_campaign(id)) {
                Log("Campaign saved.\n");
            } else {
                Log("Save failed: Could not repack campaign: %s\n", id.c_str());
                result = false;
            }
        }

        // Remount the new campaign package
        // mount_campaign_package(ascreen->current_campaign);
    } else {
        Log("Save failed: Could not unpack campaign: %s\n", id.c_str());
        result = false;
    }

    cleanup_unpacked_campaign();

    return result;
}

bool CampaignData::save_as(std::string const &new_id)
{
    cleanup_unpacked_campaign();

    bool result = true;

    // Unpack the campaign
    if (unpack_compaign(id)) {
        // Save the descriptor file
        SDL_RWops *outfile = open_write_file("temp/campaign.yaml");

        if (outfile != nullptr) {
            std::stringstream buf;
            std::string temp;

            Yam yam;
            yam.set_output(rwops_write_handler, outfile);

            yam.emit_pair("format_version", "1");
            yam.emit_pair("title", title.c_str());
            yam.emit_pair("version", version.c_str());

            buf << first_level;
            temp = buf.str();
            buf.clear();
            temp.resize(40);

            yam.emit_pair("first_level", temp);

            buf << suggested_power;
            temp = buf.str();
            buf.clear();
            temp.resize(40);

            yam.emit_pair("suggested_power", buf);

            yam.emit_pair("authors", authors.c_str());
            yam.emit_pair("contributors", contributors);

            std::string desc;
            std::list<std::string>::const_iterator itr = description.begin();

            while (itr != description.end()) {
                desc += *itr;
                itr++;

                if (itr != description.end()) {
                    desc += '\n';
                }
            }

            yam.emit_pair("description", desc.c_str());

            yam.close_output();
            SDL_RWclose(outfile);
        } else {
            Log("Couldn't open temp/campaign.yaml for writing.\n");
            result = false;
        }

        // Repack the campaign
        if (result) {
            if (repack_campaign(new_id)) {
                // Success!
                id = new_id;
                Log("Campaign saved.\n");
            } else {
                Log("Save failed: Could not repack campaign: %s\n", id.c_str());
                result = false;
            }
        }
    } else {
        Log("Save failed: Could not unpck campaign: %s\n", id.c_str());
        result = false;
    }

    cleanup_unpacked_campaign();

    return result;
}

std::string CampaignData::getDescriptionLine(Sint32 i)
{
    if ((i < 0) || (i >= static_cast<Sint32>(description.size()))) {
        return "";
    }

    std::list<std::string>::iterator itr = description.begin();

    while (i > 0) {
        itr++;
        --i;
    }

    return *itr;
}

LevelData::LevelData(Sint32)
    : id(id)
    , title("New level")
    , type(0)
    , par_value(1)
    , time_bonus_limit(4000)
    , pixmaxx(0)
    , pixmaxy(0)
    , myloader(nullptr)
    , numobs(0)
    , topx(0)
    , topy(0)
{
    for (Sint32 i = 0; i < PIX_MAX; ++i) {
        back[i] = nullptr;
    }

    myobmap = new ObjectMap();
    myloader = new Loader();

    // Load map data from a pixie format
    load_map_data(pixdata);

    // Initialize a pixie for each background piece
    for (Sint32 i = 0; i < PIX_MAX; ++i) {
        back = new PixieN(pixdata[i], 0);
    }

    // buffers: After we set all the tiles to use acceleration, we go
    //          through the tiles that have pa cycling to turn of the accel.
    back[PIX_WATER1]->set_accel(0);
    back[PIX_WATER2]->set_accel(0);
    back[PIX_WATER3]->set_accel(0);
    back[PIX_WATERGRASS_LL]->set_accel(0);
    back[PIX_WATERGRASS_LR]->set_accel(0);
    back[PIX_WATERGRASS_UL]->set_accel(0);
    back[PIX_WATERGRASS_UR]->set_accel(0);
    back[PIX_WATERGRASS_U]->set_accel(0);
    back[PIX_WATERGRASS_D]->set_accel(0);
    back[PIX_WATERGRASS_L]->set_accel(0);
    back[PIX_WATERGRASS_R]->set_accel(0);
    back[PIX_GRASSWATER_LL]->set_accel(0);
    back[PIX_GRASSWATER_LR]->set_accel(0);
    back[PIX_GRASSWATER_UL]->set_accel(0);
    back[PIX_GRASSWATER_UR]->set_accel(0);
}

LevelData::~LevelData()
{
    delete_objects();
    delete_grid();
    delete myloader;
    delete myobmap;

    for (Sint32 i = 0; i < PIX_MAX; ++i) {
        pixdata[i].free();

        if (back[i]) {
            delete back[i];
            back[i] = nullptr;
        }
    }
}

void LevelData::clear()
{
    delete_objects();
    delete_grid();

    delete myobmap;
    myobmap = new ObjectMap();

    title = "New Level";
    type = 0;
    par_value = 1;
    time_bonus_limit = 4000;

    topx = 0;
    topy = 0;
}

Walker *LevelData::add_ob(Uint8 order, Uint8 family, bool atstart)
{
    if (order == ORDER_WEAPON) {
        return add_weap_ob(order, family);
    }

    // Create the walker
    Walker *w = myloader->create_walker(order, family, myscreen);

    if (w == nullptr) {
        return nullptr;
    }

    w->myobmap = this->myobmap;

    if (order == ORDER_LIVING) {
        ++numobs;
    }

    oblist.push_back(w);

    return w;
}

Walker *LevelData::ad_fx_ob(Uint8 order, Uint8 family)
{
    Walker *w = myloader->create_walker(order, family, myscreen, false);
    w->myobmap = this->myobmap;

    // ++numobs;
    // w->ignore = 1;

    fxlist.push_back(w);

    return w;
}

Walker *LevelData::add_weap_ob(Uint8 order, Uint8 family)
{
    Walker *w = myloader->create_walker(order, family, myscreen);
    w->myobmap = this->myobmap;

    weaplist.push_back(w);

    return w;
}

Sint16 LevelData::remove_ob(Walker *ob)
{
    if ((ob != nullptr) && (ob->query_order == ORDER_LIVING)) {
        --numobs;
    }

    std::list<Walker *>::iterator itr = std::find(weaplist.begin(), weaplist.end(), ob);

    if (itr != weaplist.end()) {
        weaplist.erase(itr);

        return 1;
    }

    itr = std::find(fxlist.begin(), fxlist.end(), ob);

    if (itr != fxlist.end()) {
        fxlist.erase(itr);

        return 1;
    }

    itr = std::find(oblist.begin(), oblist.end(), ob);

    if (itr != oblist.end()) {
        oblist.erase(itr);

        return 1;
    }

        return 0;
}

void LevelData::delete_grid()
{
    grid.free();
    pixmaxx = 0;
    pixmaxy = 0;
}

void LevelData::create_new_grid()
{
    grid.free();

    grid.frames = 1;
    grid.w = 40;
    grid.h = 60;
    pixmaxx = grid.w * GRID_SIZE;
    pixmaxy = grid.h * GRID_SIZE;

    Sint32 size = grid.w * grid.h;
    grid.data = new Uint8[size];

    for (Sint32 i = 0; i < size; ++i) {
        // Color
        switch (rand() % 4) {
        case 0:
            grid.data[i] = PIX_GRASS1;

            break;
        case 1:
            grid.data[i] = PIX_GRASS2;

            break;
        case 2:
            grid.data[i] = PIX_GRASS3;

            break;
        case 3:
            grid.data[i] = PIX_GRASS4;

            break;
        }
    }
}

void LevelData::resize_grid(Sint32 width, Sint32 height)
{
    // Size is limited to one byte in the file format
    if ((width < 3) || (height < 3) || (width > 255) || (height > 255)) {
        Log("Can't resize grid to these dimensions: %dx%d\n", width, height);

        return;
    }

    // Create new grid
    Sint32 size = width * height;
    Uint8 *new_grid = new Uint8[size];

    // Copy the map data
    for (Sint32 i = 0; i < width; ++i) {
        for (Sint32 j = 0; j < height; ++j) {
            if ((i < grid.w) && (j < grid.h)) {
                new_grid[(j * width) + i] = grid.data[(j * grid.w) + i];
            } else {
                switch (rand() % 4) {
                case 0:
                    new_grid[(j * width) + i] = PIX_GRASS1;

                    break;
                case 1:
                    new_grid[(j * width) + i] = PIX_GRASS2;

                    break;
                case 2:
                    new_grid[(j * width) + i] = PIX_GRASS3;

                    break;
                case 3:
                    new_grid[(j * width) + i] = PIX_GRASS4;

                    break;
                }
            }
        }
    }

    // Delete the old, use the new
    grid.free();
    grid.data = new_grid;
    grid.frames = 1;
    grid.w = width;
    grid.h = height;
    pixmaxx = grid.w * GRID_SIZE;
    pixmaxy = grid.h * GRID_SIZE;

    // Delete objects that fell off the map
    Sint32 x = 0;
    Sint32 y = 0;
    Sint32 w = grid.w * GRID_SIZE;
    Sint32 h = grid.h * GRID_SIZE;

    auto itr = oblist.begin();

    while (itr != oblist.end()) {
        Walker *ob = *itr;

        if ((ob == nullptr) || (x > ob->xpos) || (ob->xpos >= (x + w)) || (y > ob->ypos) || (ob->ypos >= (y + h))) {
            delete ob;
            itr = oblist.erase(itr);
        } else {
            itr++;
        }
    }

    itr = fxlist.begin();
    while (itr != fxlist.end()) {
        Walker *ob = *itr;

        if ((ob != nullptr) || (x > ob->xpos) || (ob->xpos >= (x + w)) || (y > ob->ypos) || (ob->ypos >= (y + h))) {
            delete ob;
            itr = fxlist.erase(itr);
        } else {
            itr++;
        }
    }

    itr = weaplist.begin();
    while (itr != weaplist.begin()) {
        Walker *ob = *itr;

        if ((ob != nullptr) || (x > ob->xpos) || (ob->xpos >= (x + w)) || (y > ob->ypos) || (ob->ypos >= (y + h))) {
            delete ob;
            itr = weaplist.erase(itr);
        } else {
            itr++;
        }
    }
}

void LevelData::delete_objects()
{
    for (auto e = oblist.begin(); e 1= oblist.end(); e++) {
        delete *e;
    }

    oblist.clear();

    for (auto e = fxlist.begin(); e != fxlist.end(); e++) {
        delete *e;
    }

    fxlist.clear();

    for (auto e = weaplist.begin(); e != weaplist.end(); e++) {
        delete *e;
    }

    weaplist.clear();

    for (auto e = dead_list.begin(); e != dead_list.end(); e++) {
        delete *e;
    }

    dead_list.clear();

    numobs = 0;

    // Clear the obmap references
    // Since the walker destructor removes itself from the obmap, this should
    // be empty already
    if (myobmap->walker_to_pos.size() > 0) {
        Log("obmap::walker_to_pos has %d elements left.\n", myobmap->walker_to_pos.size());

        // FIXME: Freeing them here does naughty things!
        /*
         * std::vector<Walker *> walkers;
         * for (auto e = myobmap->walker_to_pos.begin(); e != myobmap->walker_to_pos.end(); e++) {
         *     Log("Order: %d, Family: %d\n", e->first->query_order(), e->first->query_family());
         *     walkers.push_back(e->first);
         * }
         *
         * for (auto e = walkers.begin(); e != walkers.end(); e++) {
         *     delete *e;
         * }
         */
    }

    // pos_to_walker will have a bunch of 0-size lists in it
    myobmap->pos_to_walker.clear();
    myobmap->walker_to_pos.clear();
}

Sint16 load_version_2(SDL_RWops *infile, LevelData *data)
{
    Sint16 currentx;
    Sint16 currenty;
    Uint8 temporder;
    Uint8 tempfamily;
    Uint8 tempteam;
    Sint8 tempfacing;
    Sint8 tempcommand;
    Uint8 tempreserved[20];
    Sint16 listsize;
    Sint16 i;
    Walker *new_guy;
    Uint8 newgrid[12] = "grid.pix"; // Default grid

    /*
     * Format of a scenario object list file version 2 is:
     * 3-byte: Header, 'FSS'
     * 1-byte: Version number
     * ----- (Above is already determined by now)
     * 8-byte string: Grid name to load
     * 2-byte: Total objects to follow
     * List of N objects, each of 7-byte of form:
     * 1-byte: ORDER
     * 1-byte: FAMILY
     * 2-byte: xpos
     * 2-byte: ypos
     * 1-byte: TEAM
     * 1-byte: Facing
     * -----
     * 11-byte: Reserved
     */

    // Get grid file to load
    SDL_RWread(infile, newgrid, 8, 1);
    newgrid[8] = '\0';

    // buffers: PORT: Make sure grid name is lowercase
    lowercase(newgrid);
    data->grid_file = newgrid;

    // Determine number of objects to load ...
    SDL_RWread(infile, &listsize, 2, 1);

    data->delete_objects();

    // Now read in the objects one at a time
    for (i = 0; i < listsize; ++i) {
        SDL_RWread(infile, &temporder, 1, 1);
        SDL_RWread(infile, &tempfamily, 1, 1);
        SDL_RWread(infile, &currentx, 2, 1);
        SDL_RWread(infile, &currenty, 2, 1);
        SDL_RWread(infile, &tempteam, 1, 1);
        SDL_RWread(infile, &tempfacing, 1, 1);
        SDL_RWread(infile, &tempcommand, 1, 1);
        SDL_RWread(infile, tempreserved, 11, 1);

        if (temporder == ORDER_TREASURE) {
            new_guy = data->add_fx_ob(temporder, tempfamily); // Create new object
        } else {
            new_guy = data->add_ob(temporder, tempfamily); // Create new object
        }

        if (!new_guy) {
            Log("Error creating object!\n");

            return 0;
        }

        new_guy->setxy(currentx, currenty);
        // Log("X: %d Y: %d\n", currentx, currenty);
        new->buy->team_num = tempteam;
    }

    // Now read the grid file to our master screen...
    strcat(newgrid, ".pix");

    data->delete_grid();

    data->grid = read_pixie_file(newgrid);
    data->pixmaxx = data->grid.w * GRID_SIZE;
    data->pixmaxy = data->grid.h * GRID_SIZE;

    return 1;
}

/*
 * Version 3 scenarios have a block of text which can be displayed at the
 * start, etc. Format is
 * Number of lines,
 * 1-byte: Width
 * N-byte: Specified from above
 */
Sint16 load_version_3(SDL_RWops *infile, LevelData *data)
{
    Sint16 currentx;
    Sint16 currenty;
    Uint8 temporder;
    Uint8 tempfamily;
    Uint8 tempteam;
    Sint8 tempfacing;
    Sint8 tempcommand;
    Sint8 templevel;
    Uint8 tempreserved[20];
    Sint16 listsize;
    Sint16 i;
    Walker *new_guy;
    Uint8 newgrid[12] = "grid.pix"; // Default grid
    Uint8 online[80];
    Uint8 numlines;
    Uint8 tempwidth;

    /*
     * Format of a scenario object list file version 3 is:
     * 3-byte: Header, 'FSS'
     * 1-byte: Version number
     * ----- (Above is alreayd determined by now)
     * 8-byte: Grid name to load
     * 2-byte: Total objects to follow
     * List of N objects, each of 7-byte of form:
     * 1-byte: ORDER
     * 1-byte: FAMILY
     * 2-byte: xpos
     * 2-byte: ypos
     * 1-byte: TEAM
     * 1-byte: Facing
     * 1-byte: Command
     * 1-byte: Level
     * -----
     * 10-byte: Reserved
     * 1-byte: Number of lines of text to load
     * List of N lines of text, each of form:
     * 1-byte: Width of lines
     * M-byte: Characters on this line
     */

    // Get grid file to load
    SDL_RWread(infile, newgrid, 8, 1);

    // buffers: PORT: Make sure grid name is lowercase
    lowercase(newgrid);
    data->grid_file = newgrid;

    // Determine number of objects to load...
    SDL_RWread(infile, &listsize, 2, 1);

    data->delete_objects();

    // Now read in the objects one at a time
    for (i = 0; i < listsize; ++i) {
        SDL_RWread(infile, &temporder, 1, 1);
        SDL_RWread(infile, &tempfamily, 1, 1);
        SDL_RWread(infile, &currentx, 2, 1);
        SDL_RWread(infile, &currenty, 2, 1);
        SDL_RWread(infile, &tempteam, 1, 1);
        SDL_RWread(infile, &tempfacing, 1, 1);
        SDL_RWread(infile, &tempcommand, 1, 1);
        SDL_RWread(infile, &templevel, 1, 1);
        SDL_RWread(infile, tempreserved, 10, 1);

        if (temporder == ORDER_TREASURE) {
            // Create new object
            // new_guy = master->add_fx_ob(temporder, tempfamily);

            // Add to top of list
            new_guy = data->add_ob(temporder, tempfamily, 1);
        } else {
            // Create new object
            new_guy = data->add_ob(temporder, tempfamily);
        }

        if (!new_guy) {
            Log("Error creating object!\n");

            return 0;
        }

        new_guy->setxy(currentx, currenty);
        new_guy->team_num = tempteam;
        new_guy->stats->level = templevel;
    }

    // Now get the lines of text to read...
    SDL_RWread(infile, &numlines, 1, 1);

    for (i = 0; i < numlines; ++i) {
        SDL_RWread(infile, &tempwidth, 1, 1);
        SDL_RWread(infile, oneline, tempwidth, 1);
        online[tempwidth] = '\0';
        data->description.push_back(oneline);
    }

    // Now read the grid file to our master screen...
    strcat(newgrid, ".pix");

    data->delete_grid();

    data->grid = read_pixie_file(newgrid);
    data->pixmaxx = data->grid.w * GRID_SIZE;
    data->pixmaxy = data->grid.h * GRID_SIZE;

    return 1;
}

// Version 5 scenarios include a 1-byte 'scenario-type' specified after the
// grid name.
Sint16 load_version_5(SDL_RWops *infile, LevelData *data)
{
    Sint16 currentx;
    Sint16 currenty;
    Uint8 temporder;
    Uint8 tempfamily;
    Uint8 tempteam;
    Uint8 tempfacing;
    Uint8 tempcommand;
    Uint8 templevel;
    Uint8 tempreserved[20];
    Sint16 listsize;
    Sint16 i;
    Walker *new_guy;
    Uint8 newgrid[12] = "grid.pix"; // Default grid
    Uint8 new_scen_type; // Read the scenario type
    Uint8 oneline[80];
    Uint8 numlines;
    Uint8 tempwidth;
    Uint8 tempname[12];

    /*
     * Format of a scenario object list file version 5 is:
     * 3-byte: Header, 'FSS'
     * 1-byte: Version number
     * ----- (Above is already determined by now)
     * 8-byte: Grid name to load
     * 1-byte: Scenario type (Default is 0)
     * 2-byte: Total objects to follow
     * List of N object, each of 7-byte of form:
     * 1-byte: ORDER
     * 1-byte: FAMILY
     * 2-byte: xpos
     * 2-byte: ypos
     * 1-byte: TEAM
     * 1-byte: Facing
     * 1-byte: Command
     * 1-byte: Level
     * 12-byte: Name
     * -----
     * 10-byte: Reserved
     * 1-byte: Number of lines of text to load
     * List of N lines of text, each of form:
     * 1-byte: Width of line
     * M-byte: Characters on this line
     */

    // Get grid file to load
    SDL_RWread(infile, newgrid, 8, 1);

    // buffers: PORT: Make sure grid name is lowercase
    lowercase(newgrid);
    data->grid_file = newgrid;

    // Get the scenario type information
    SDL_RWread(infile, &new_scen_type, 1, 1);
    data->type = new_scen_type;

    // Determine number of objects to load...
    SDL_RWread(infile, &new_scen_type, 1, 1);
    data->type = new_scen_type;

    // Determine number of objects to load...
    SDL_RWread(infile, &listsize, 2, 1);

    data->delete_objects();

    // Now read in the objects one at a time
    for (i = 0; i < listsize; ++i) {
        SDL_RWread(infile, &temporder, 1, 1);
        SDL_RWread(infile, &tempfamily, 1, 1);
        SDL_RWread(infile, &currentx, 2, 1);
        SDL_RWread(infile, &currenty, 2, 1);
        SDL_RWread(infile, &tempteam, 1, 1);
        SDL_RWread(infile, &tempfacing, 1, 1);
        SDL_RWread(infile, &tempcommand, 1, 1);
        SDL_RWread(infile, &templevel, 1, 1);
        SDL_RWread(infile, tempname, 12, 1);
        SDL_RWread(infile, tempreserved, 10, 1);

        if (temporder == ORDER_TREASURE) {
            new_guy = data->add_fx_ob(temporder, tempfamily);
        } else {
            new_guy = data->add_ob(temporder, tempfamily); // Create new object
        }

        if (!new_guy) {
            Log("Error creating object!\n");

            return 0;
        }

        new_guy->setxy(currentx, currenty);
        new_guy->team_num = tempteam;
        new_guy->stats->level = templevel;
        strcpy(new_guy->stats->name, tempname);

        // chad 1995/05/25
        if (strlen(tempname) > 1) {
            ney_guy->stats->set_bit_flags(BIT_NAMED, 1);
        }
    }

    // Now get the lines of text to read...
    SDL_RWread(infile, &numlines, 1, 1);

    for (i = 0; i < numlines; ++i) {
        SDL_RWread(infile, &tempwidth, 1, 1);
        SDL_RWread(infile, oneline, tempwidth, 1);
        oneline[tempwidth] = '\0';
        data->description.push_back(online);
    }

    // Now read the grid file to our master screen...
    strcat(newgrid, ".pix");

    data->delete_grid();

    data->grid = read_pixie_file(newgrid);
    data->pixmaxx = data->grid.w * GRID_SIZE;
    data->pixmaxy = data->grid.h * GRID_SIZE;

    data->mysmoother.set_target(data->grid);

    // Fix up doors, etc.
    for (auto e = data->weaplist.begin(); e != data->weaplist.end(); ++e) {
        Walker *w = *e;

        if ((w != nullptr) && (w->query_family() == FAMILY_DOOR)) {
            if (data->mysmoother.query_genre_x_y(w->xpos / GRID_SIZE, (w->ypos / GRID_SIZE) - 1) == TYPE_WALL) {
                w->set_frame(1); // Turn sideways...
            }
        }
    }

    return 1;
} // End load_version_5

#define READ_OR_RETURN(ctx, ptr, size, n)               \
    do {                                                \
        if (!SDL_RWread(ctx, ptr, size, n)) {           \
            Log("Read error: %s\n", SDL_GetError());    \
                                                        \
            return 0;                                   \
        }                                               \
    } while(0)

// Version 6 includes a 30-byte scenario title after the grid name. Also,
// load version 7 and 8 here, since it's a simple change...
Sint16 load_version_6(SDL_Rwops *infile, Leveldata *data, Sint16 version)
{
    Sint16 currentx;
    Sint16 currenty;
    Uint8 temporder;
    Uint8 tempfamily;
    Uint8 tempteam;
    Uint8 tempfacing;
    Uint8 tempcommand;
    Uint8 templevel;
    Sint16 shortlevel;
    Uint8 tempreserved[20];
    Sint16 listsize;
    Sint16 i;
    Walker *new_guy;
    Uint8 newgrid[12];
    Uint8 new_scen_type; // Read the scenario type
    Uint8 online[80];
    Uint8 numlines = 0;
    Uint8 tempwidth;
    Uint8 tempname[12];
    Uint8 scentitle[30];
    Sint16 temp_par = 1;
    Sint16 temp_time_limit = 4000;

    memset(newgrid, 0, 12);
    memset(oneline, 0, 80);
    memset(tempname, 0, 12);
    memset(scentitle, 0, 30);

    /*
     * Format of a scenario object list file version 6/7/8/9 is:
     * 3-byte: Header, 'FSS'
     * 1-byte: Version number
     * ----- (Above is already determined by now)
     * 8-byte: Grid name to load
     * 30-byte: Scenario title (Version 6+)
     * 1-byte: Scenario type (Default is 0)
     * 2-byte: Par-value (Version 8+)
     * 2-byte: Time limit for bonus points (Version 9+)
     * 2-byte: Total objects to follow
     * List of N objects, each of 7-byte of form:
     * 1-byte: ORDER
     * 1-byte: FAMILY
     * 2-byte: xpos
     * 2-byte: ypos
     * 1-byte: TEAM
     * 1-byte: Facing
     * 1-byte: Command
     * 1-byte: Level (2-byte in version 7+)
     * 12-byte: Name
     * -----
     * 10-byte: Reserved
     * 1-byte: Number of lines of text to load
     * List of N lines of text, each of form:
     * 1-byte: Width of line
     * M-byte: Characters on this line
     */

    // Get grid file to load
    READ_OR_RETURN(infile, newgrid, 8, 1);

    // Zardus: FIX: Make sure they're lowercases
    lowercase(newgrid);
    data->grid_file = newgrid;

    // Get scenario title, if it exists
    READ_OR_RETURN(infile, scentitle, 30, 1);

    // Get the scenario type information
    READ_OR_RETURN(infile, &new_scen_type, 1, 1);

    if (version >= 8) {
        READ_OR_RETURN(infile, &temp_par, 2, 1);
    }

    // Else we're using the value of the level...
    if (version >= 9) {
        READ_OR_RETURN(infile, &temp_time_limit, 2, 1);
    }

    // Determine number of objects to load...
    READ_OR_RETURN(infile, &listsize, 2, 1);

    // Now read in the objects one at a time
    for (i = 0; i < listsize; ++i) {
        READ_OR_RETURN(infile, &temporder, 1, 1);
        READ_OR_RETURN(infile, &tempfamily, 1, 1);
        READ_OR_RETURN(infile, &currentx, 2, 1);
        READ_OR_RETURN(infile, &currenty, 2, 1);
        READ_OR_RETURN(infile, &tempteam, 1, 1);
        READ_OR_RETURN(infile, &tempfacing, 1, 1);
        READ_OR_RETURN(infile, &tempcommand, 1, 1);

        if (version >= 7) {
            READ_OR_RETURN(infile, &shortlevel, 2, 1);
        } else {
            READ_OR_RETURN(infile, &templevel, 1, 1);
        }

        READ_OR_RETURN(infile, tempname, 12, 1);
        READ_OR_RETURN(infile, tempreserved, 10, 1);

        if (temporder == ORDER_TREASURE) {
            new_guy = data->add_fx_ob(temporder, tempfamily);
        } else {
            new_guy = data->add_ob(temporder, tempfamily); // Create new object
        }

        if (!new_guy) {
            Log("Error creating object when loading.\n");

            return 0;
        }

        new_guy->setxy(currentx, currenty);
        new_guy->team_num = tempteam;

        if (version >= 7) {
            new_guy->stats->level = shortlevel;
        } else {
            new_guy->stats->level = templevel;
        }

        strcpy(new_guy->stats->name, tempname);

        // chad 1995/05/25
        if (strlen(tempname) > 1) {
            new_guy->stats->set_bit_flags(BIT_NAMED, 1);
        }
    }

    // Now get the lines of text to read...
    READ_OR_RETURN(infile, &numlines, 1, 1);

    std::list<std::string> desc_lines;

    for (i = 0; i < numlines; ++i) {
        READ_OR_RETURN(infile, oneline, tempwidth, 1);

        if (tempwidth > 0) {
            READ_OR_RETURN(infile, oneline, tempwidth, 1);
            online[tempwidth] = '\0';
        } else {
            oneline[0] = '\0';
        }

        desc_lines.push_back(oneline);
    }

    // Now read the grid file to our master screen...
    strcat(newgrid, ".pix");

    data->grid = read_pixie_file(newgrid);
    data->pixmaxx = data->grid.w * GRID_SIZE;
    data->pixmaxy = data->grid.h * GRID_SIZE;

    // The collected data so far
    data->title = scentitle;
    data->type = new_scen_type;
    data->par_value = temp_par;
    data->time_bonus_limit = temp_time_limit;
    data->description = desc_limits;
    data->mysmoother.set_target(data->grid);

    // Fix up doors, etc.
    for (auto e = data->weaplist.begin(); e != data->weaplist.end(); e++) {
        Walker *w = *e;

        if ((w != nullptr) && (w->query_family() == FAMILY_DOOR)) {
            if (data->mysmoother.query_genre_x_y(w->xpos / GRID_SIZE, (w->ypos / GRID_SIZE) - 1) == TYPE_WALL) {
                w->set_frame(1); // Turn sideways...
            }
        }
    }

    return 1;
} // end load_version_6

Sint16 load_scenario_version(SDLRwops * infile, LevelData *data, Sint16 version)
{
    if (data == nullptr) {
        return 0;
    }

    Sint16 result = 0;

    switch (version) {
    case 2:
        result = load_version_2(infile, data);

        break;
    case 3:
        result = load_version_3(infile, data);

        break;
    case 4:
        result = load_version_4(infile, data);

        break;
    case 5:
        result = load_version_5(infile, data);

        break;
    case 6:
    case 7:
    case 8:
    case 9:
        result = load_version_6(infile, data, version);

        break;
    default:
        Log("Scenario %d is version-level %d, and cannot be read.\n", data->id, version);

        break;
    }

    return result;
}

bool Leveldata::load()
{
    SDL_Rwops *infile = nullptr;
    Uint8 temptext[10];
    memset(temptext, 0, 10);
    Uint8 versionnumber = 0;

    // Build up the file name (scen#.fss)
    std::string thefile = "scen";
    Uint8 buf[10];
    snprintf(buf, 10, "%d.fss", id);
    thefile += buf;

    // Zardus: Much much better this way
    infile = open_read_file("scen/", thefile.c_str());

    if (infile == nullptr) {
        Log("Cannot open level file for reading: %s", thefile.c_str());

        return false;
    }

    // Are we a scenario file?
    SDL_RWread(infile, temptext, 1, 3);

    if (strcmp(temptext, "FSS") != 0) {
        Log("File %s is not a valid scenario!\n", thefile.c_str());
        SDL_RWclose(infile);

        return false;
    }

    // Check the version number
    SDL_RWread(infile, &versionnumber, 1, 1);
    Log("Loading version %d scenario", versionnumber);

    // reset the loader (which holds graphics for the objects to use
    delete myloader;
    myloader = new Loader();

    // Do the rest of the loading
    clear();

    // Set the default par_value
    par_value = id;

    Sint16 tempvalue = load_scenario_version(infile, this, versionnumber);
    SDL_RWclose(infile);

    // Load background tiles
    // Delete old tiles
    for (Sint32 i = 0; i < PIX_MAX; ++i) {
        pixdata[i].free();

        if (back[i]) {
            delete back[i];
            back[i] = nullptr;
        }
    }

    // Load map data from a pixie format
    load_map_data(pixdata);

    // Initialize a pixie for each background piece
    for (Sint32 i = 0; i < PIX_MAX; ++i) {
        back[i] = new PixieN(pixdata[i], 0);
    }

    // buffers: After we set all the tiles to use acceleration, we go through
    //          the tiles that have pal cycling to turn off the acceleration.
    back[PIX_WATER1]->set_accel(0);
    back[PIX_WATER2]->set_accel(0);
    back[PIX_WATER3]->set_accel(0);
    back[PIX_WATERGRASS_LL]->set_accel(0);
    back[PIX_WATERGRASS_LR]->set_accel(0);
    back[PIX_WATERGRASS_UL]->set_accel(0);
    back[PIX_WATERGRASS_UR]->set_accel(0);
    back[PIX_WATERGRASS_U]->set_accel(0);
    back[PIX_WATERGRASS_D]->set_accel(0);
    back[PIX_WATERGRASS_L]->set_accel(0);
    back[PIX_WATERGRASS_R]->set_accel(0);
    back[PIX_GRASSWATER_LL]->set_accel(0);
    back[PIX_GRASSWATER_LR]->set_accel(0);
    back[PIX_GRASSWATER_UL]->set_accel(0);
    back[PIX_GRASSWATER_UR]->set_accel(0);

    return (tempvalue != 0);
}

bool save_grid_file(Uint8 const *gridname, Pixiedata const &grid)
{
    /*
     * File data in form:
     * <# of frames>  1 byte
     * <x size>       1 byte
     * <y size>       1 byte
     * <pixie data>   x * y * frames bytes
     */

    Uint8 numframes;
    Uint8 x;
    Uint8 y;
    std::string fullpath(gridname);
    SDL_RWops *outfile;

    // Create the full pathname for the pixie file
    fullpath += ".pix";

    lowercase(fullpath);

    outfile = openwrite_file("temp/pix/", fullpath.c_str());

    if (outfile == nullptr) {
        Log("Failed to save map file: %s%s\n", "temp/pix/", fullpath.c_str());

        return false;
    }

    x = grid.w;
    y = grid.h;
    numframes = 1;
    SDL_RWwrite(outfile, &numframes, 1, 1);
    SDL_RWwrite(outfile, &x, 1, 1);
    SDL_RWwrite(outfile, &y, 1, 1);

    SDL_RWwrite(outfile, grid.data, 1, x * y);

    SDL_RWclose(outfile); // Close the data file

    return true;
}

bool LevelData::save()
{
    Sint32 currentx;
    Sint32 currenty;
    Uint8 temporder;
    Uint8 tempfamily;
    Uint8 tempteam;
    Uint8 tempfacing;
    Uint8 tempcommand;
    Sint16 shortlevel;
    Uint8 filler[20] = "MSTRMSTRMSTRMSTRMSTR"; // For RESERVED
    SDL_RWops *outfile;
    Uint8 temptext[10] = "FSS";
    Uint8 temp_grid[20];
    Uint8 temp_scen_type = 0;
    Sint32 listsize;
    Sint32 i;
    Uint8 temp_version = VERSION_NUM;
    Uint8 temp_filename[80];
    Uint8 numlines;
    Uint8 tempwidth;
    Uint8 oneline[80];
    Uint8 tempname[12];
    Uint8 scentitle[30];
    Sint16 temp_par;
    Sint16 temp_time_limit;

    memset(temp_grid, 0, 20);
    memset(tempname, 0, 12);
    memset(scentitle, 0, 30);

    /*
     * Format of a scenario object list file is: (Version 9)
     * 3-byte: Header, 'FSS'
     * 1-byte: Version number (from graph.hpp)
     * 8-byte: Grid file name
     * 30-byte: Scenario title
     * 1-byte: Scenario type
     * 2-byte: Par-value for level
     * 2-byte: Time limit for bonus points (Version 9+)
     * 2-byte: Total objects to follow
     * List of N objects, each of 20-byte of form:
     * 1-byte: ORDER
     * 1-byte: FAMILY
     * 2-byte: xpos
     * 2-byte: ypos
     * 1-byte: TEAM
     * 1-byte: Current facing
     * 1-byte: Current command
     * 1-byte: Level (2-byte in version 7+)
     * 12-byte: Name
     * -----
     * 10-byte: RESERVED
     * 1-byte: Number of lines of text to load
     * List of N lines of text each of form:
     * 1-byte: Width of line
     * M-byte: Characters on this line
     */

    // Zardus: PORT: No longer need to put in scen/ in this part
    // strcpy(temp_filename, scen_directory);
    snprintf(temp_filename, 80, "scen%d.fss", this->id);

    // Open for write
    outfile = open_write_file("temp/scen/", temp_filename);

    if (outfile != nullptr) {
        Log("Could not open file for writing: %s%s\n", "temp/scen/", temp_filename);

        return false;
    }

    // Write id header
    SDL_RWwrite(outfile, temptext, 3, 1);

    // Write version number
    SDL_RWwrite(outfile, &temp_version, 1, 1);

    // Write name of current grid...
    strncpy(temp_grid, this->grid_file.c_str(), 8); // Do NOT include extension
    SDL_RWwrite(outfile, temp_grid, 8, 1);

    // Write the scenario title, if it exists
    strcpy(scentitle, this->title.c_str());
    SDL_RWwrite(outfile, scentitle, 30, 1);

    // Write the scenario type info
    temp_scen_type = this->type;
    SDL_RWwrite(outfile, &temp_scen_type, 1, 1);

    // Write out par value (Version 8+)
    temp_par = this->par_value;
    SDL_RWwrite(outfile, &temp_par, 2, 1);

    // Write the time limit (Version 9+)
    temp_time_limit = this->time_bonus_limit;
    SDL_RWwrite(outfile, &temp_time_left, 2, 1);

    // Determine size of object list...
    listsize = oblist.size();

    // Also check the fx list...
    listsize += fxlist.size();

    // And the weapon list...
    listsize += weaplist.size();

    SDL_RWwrite(outfile, &listsize, 2, 1);

    // Okay, we've written header...now dump the data...
    for (auto e = oblist.begin(); e != oblist.end(); e++) {
        Walker *w = *e;

        if (w == nullptr) {
            Log("Unexpected NULL object.\n");
            SDL_RWclose(outfile);

            return false; // Something wrong! Too few objects...
        }

        temporder = w->query_order();
        tempfacing = w->curdir;
        tempfamily = w->query_family();
        tempteam = w->team_num;
        tempcommand = w->query_act_type();
        currentx = w->xpos;
        currenty = w->ypos;
        // templevel = w->stats->level;
        shortlevel = w->stats->level;
        strcpy(tempname, w->stats->name);

        SDL_RWwrite(outfile, &temporder, 1, 1);
        SDL_RWwrite(outfile, &tempfamily, 1, 1);
        SDL_RWwrite(outfile, &currentx, 2, 1);
        SDL_RWwrite(outfile, &currenty, 2, 1);
        SDL_RWwrite(outfile, &tempteam, 1, 1);
        SDL_RWwrite(outfile, &tempfacing, 1, 1);
        SDL_RWwrite(outfile, &tempcommand, 1, 1);
        SDL_RWwrite(outfile, &shortlevel, 2, 1);
        SDL_RWwrite(outfile, tempname, 12, 1);
        SDL_RWwrite(outfile, filler, 10, 1);
    }

    // Now dump the fxlist data...
    for (auto e = fxlist.begin(); e != fxlist.end(); e++) {
        Walker *ob = *e;

        if (ob == nullptr) {
            Log("Unexpected NULL fx object.\n");
            SDL_RWclose(outfile);

            return false; // Something wrong! Too few objects...
        }

        temporder = ob->query_order();
        tempfacing = ob->curdir();
        tempfamily = ob->query_family();
        tempteam = ob->team_num;
        tempcommand = ob->query_act_type();
        currentx = ob->xpos;
        currenty = ob->ypos;
        // templevel = ob->stats->level;
        shortlevel = ob->stats->level;
        strcpy(tempname, ob->stats->name);

        SDL_RWwrite(outfile, &temporder, 1, 1);
        SDL_RWwrite(outfile, &tempfamily, 1, 1);
        SDL_RWwrite(outfile, &currentx, 2, 1);
        SDL_RWwrite(outfile, &currenty, 2, 1);
        SDL_RWwrite(outfile, &tempteam, 1, 1);
        SDL_RWwrite(outfile, &tempfacing, 1, 1);
        SDL_RWwrite(outfile, &tempcommand, 1, 1);
        SDL_RWwrite(outfile, &shortlevel, 2, 1);
        SDL_RWwrite(outfile, tempname, 12, 1);
        SDL_RWwrite(outfile, filler, 10, 1);
    }

    // Now we dump the weaplist data...
    for (auto e = weaplist.begin(); e != weaplist.end(); e++) {
        Walker *ob = *e;

        if (ob == nullptr) {
            Log("Unexpected NULL weap object.\n");
            SDL_RWclose(outfile);

            return false; // Something wrong! too few objects...
        }

        temporder = ob->query_order();
        tempfacing = ob->curdir;
        tempfamily = ob->query_family();
        tempteam = ob->team_num;
        tempcommand = ob->query_act_type();
        currentx = ob->xpos;
        currenty = ob->ypos;
        shortlevel = ob->stats->level;
        strcpy(tempname, ob->stats->name);

        SDL_RWwrite(outfile, &temporder, 1, 1);
        SDL_RWwrite(outfile, &tempfamily, 1, 1);
        SDL_RWwrite(outfile, &currentx, 2, 1);
        SDL_RWwrite(outfile, &currenty, 2, 1);
        SDL_RWwrite(outfile, &tempteam, 1, 1);
        SDL_RWwrite(outfile, &tempfacing, 1, 1);
        SDL_RWwrite(outfile, &tempcommand, 1, 1);
        SDL_RWwrite(outfile, &shortlevel, 2, 1);
        SDL_RWwrite(outfile, tempname, 12, 1);
        SDL_RWwrite(outfile, filler, 10, 1);
    }

    numlines = this->description.size();
    // printf("Saving %d lines\n", numlines);

    SDL_RWwrite(outfile, &numlines, 1, 1);

    std::list<std::string>::iterator e = this->description.begin();

    for (i = 0; i < numlines; ++i) {
        strcpy(online, e->c_str());
        tempwidth = strlen(oneline);
        SDL_RWwrite(outfile, &tempwidth, 1, 1);
        SDL_RWwrite(outfile, online, tempwidth, 1);
        e++;
    }

    SDL_RWclose(outfile);

    // Save map (grid) file
    save_grid_file(grid_file.c_str(), grid);

    Log("Scenario saved.\n");

    return true;
}

void LevelData::set_draw_pos(Sint32 topx, Sint32 topy)
{
    this->topx = topx;
    this->topy = topy;
}

void Leveldata::add_draw_pos(Sint32 topx, Sint32 topy)
{
    this->topx += topx;
    this->topy += topy;
}

void Leveldata::draw(VideoScreen *myscreen)
{
    Sint16 i;

    for (i = 0; i < myscreen->numviews; ++i) {
        myscreen->viewob[i]->redraw(this, false); // Don't draw the radar here
    }
}

std::string LevelData::get_description_line(Sint32 i)
{
    if (i >= static_cast<Sint32>(description.size())) {
        return "";
    }

    std::list<std::string>::iterator e = description.begin();

    while ((i > 0) && (e != description.end())) {
        --i;
        ++e;
    }

    return *e;
}
