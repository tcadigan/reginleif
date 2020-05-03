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
#include "campaign_data.hpp"

#include "graphlib.hpp"
#include "io.hpp"
#include "util.hpp"

#include <sstream>

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
        SDL_RWops *rwops = open_read_file("campaign.ini");

        Sint64 size = SDL_RWsize(rwops);

        std::stringstream buf;
        Sint64 chunk_size = 1024;
        char str[chunk_size];
        Sint64 total = 0;
        Sint64 read = 1;

        while ((total < size) && (read != 0)) {
            read = SDL_RWread(rwops, str, sizeof(char), std::min(chunk_size, size - total));
            total += read;
            std::string inp(str, read);
            buf << inp;
        }

        SDL_RWclose(rwops);

        if (total != size) {
            Log("Unable to read complete file");

            return false;
        }

        std::string line;
        std::string key;
        std::string value;
        std::string::size_type delim_pos;

        while (std::getline(buf, line)) {
            delim_pos = line.find_first_of("=");

            if (delim_pos == std::string::npos) {
                Log("Skipping unknown ini line\n");
                continue;
            }

            key = line.substr(delim_pos);
            value = line.substr(delim_pos + 1, line.size());

            if (key == "title") {
                title = value;
            } else if (key == "version") {
                version = value;
            } else if (key == "authors") {
                authors = value;
            } else if (key == "contributors") {
                contributors = value;
            } else if (key == "description") {
                description = explode(value, '|');
            } else if (key == "suggested_power") {
                suggested_power = std::stoi(value);
            } else if (key == "first_level") {
                first_level = std::stoi(value);
            } else {
                Log("Unknown ini key '%s'\n", key.c_str());
            }
        }

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

bool CampaignData::save()
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

            buf << "format_version" << "=" << "1" << std::endl;
            temp = buf.str();
            buf.clear();

            size_t written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "format_version", SDL_GetError());

                return false;
            }

            buf << "title" << "=" << title << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "title", SDL_GetError());

                return false;
            }

            buf << "version" << "=" << version << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "version", SDL_GetError());

                return false;
            }

            buf << "first_level" << "=" << first_level << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "first_level", SDL_GetError());

                return false;
            }

            buf << "suggested_power" << "=" << suggested_power << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "suggested_power", SDL_GetError());

                return false;
            }

            buf << "authors" << "=" << authors << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "authors", SDL_GetError());

                return false;
            }

            buf << "contributors" << "=" << contributors << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "contributors", SDL_GetError());

                return false;
            }

            buf << "description" << "=";

            for (auto itr = description.begin(); itr != description.end(); ++itr) {
                if (itr != description.begin()) {
                    buf << "|";
                }

                buf << *itr;
            }

            buf << std::endl;

            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "description", SDL_GetError());

                return false;
            }

            SDL_RWclose(outfile);
        } else {
            Log("Couldn't open temp/campaign.ini for writing.\n");
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
    if (unpack_campaign(id)) {
        // Save the descriptor file
        SDL_RWops *outfile = open_write_file("temp/campaign.ini");

        if (outfile != nullptr) {
            std::stringstream buf;
            std::string temp;

            buf << "format_version" << "=" << "1" << std::endl;
            temp = buf.str();
            buf.clear();

            size_t written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "format_version", SDL_GetError());

                return false;
            }

            buf << "title" << "=" << title << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "title", SDL_GetError());

                return false;
            }

            buf << "version" << "=" << version << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "version", SDL_GetError());

                return false;
            }

            buf << "first_level" << "=" << first_level << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "first_level", SDL_GetError());

                return false;
            }

            buf << "suggested_power" << "=" << suggested_power << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "suggested_power", SDL_GetError());

                return false;
            }

            buf << "authors" << "=" << authors << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "authors", SDL_GetError());

                return false;
            }

            buf << "contributors" << "=" << std::endl;
            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "contributors", SDL_GetError());

                return false;
            }

            buf << "description" << "=";

            for (auto itr = description.begin(); itr != description.end(); ++itr) {
                if (itr != description.begin()) {
                    buf << "|";
                }

                buf << *itr;
            }

            buf << std::endl;

            temp = buf.str();
            buf.clear();

            written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

            if (written != temp.size()) {
                Log("Unable to write '%s': %s\n", "description", SDL_GetError());

                return false;
            }

            SDL_RWclose(outfile);
        } else {
            Log("Couldn't open temp/campaign.ini for writing.\n");
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
