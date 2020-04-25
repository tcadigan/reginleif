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

#include "io.hpp"

#include "input.hpp"
#include "pixdefs.hpp"
#include "util.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>

std::filesystem::path base_path;
std::filesystem::path write_path;
std::filesystem::path pix_path;
std::filesystem::path sound_path;
std::filesystem::path cfg_path;

static std::string mounted_campaign;

/*
 * File I/O strategy:
 * PhysicsFS is set up to look in the scen, pix, and sound directories and in
 * the current scenario package (campaign). SDL_RWops is used to access the
 * data in the files retrieved from PhysFS.
 *
 * Scenario packages are stored in the user directory so more can be installed,
 * etc. The default pix and sound assets are installed with the rest of the
 * program, presumably without write access.
 */

Sint32 rwops_read_handler(void *data, Uint8 *buffer, size_t size, size_t *size_read)
{
    SDL_RWops *rwops = static_cast<SDL_RWops *>(data);

    *size_read = SDL_RWread(rwops, buffer, 1, size);

    return 1;
}

Sint32 rwops_write_handler(void *data, Uint8 *buffer, size_t size)
{
    SDL_RWops *rwops = static_cast<SDL_RWops *>(data);

    SDL_RWwrite(rwops, buffer, 1, size);

    return 1;
}

std::filesystem::path get_user_path()
{
    std::filesystem::path path = getenv("HOME");

    return path / ".openglad";
}

std::filesystem::path get_asset_path()
{
    // FIXME: This won't typically work for *nix
    std::filesystem::path path("usr/share/openglad");

    return path;
}

SDL_RWops *open_read_file(std::string const &file)
{
    SDL_RWops *rwops = nullptr;

    // Log((std::string("Trying via PHYSFS: ") + file).c_str());
    std::filesystem::path base_file = base_path / file;

    rwops = SDL_RWFromFile(base_file.c_str(), "rb");

    if (rwops != nullptr) {
        return rwops;
    }

    // Now try opening in the current directory
    // LOG((std::string("Trying to open: ") + file).c_str());
    rwops = SDL_RWFromFile(file.c_str(), "rb");

    if (rwops != nullptr) {
        return rwops;
    }

    // Now try opening in the user directory
    std::filesystem::path user_dir = get_user_path() / file;
    // Log((std::string("Trying to open: ") + user_dir).c_str());
    rwops = SDL_RWFromFile(user_dir.c_str(), "rb");

    if (rwops != nullptr) {
        return rwops;
    }

    // Now try opening in the asset directory
    std::string asset_dir = get_asset_path() / file;
    // Log((std::string("Trying to open: ") + asset_dir).c_str());
    rwops = SDL_RWFromFile(asset_dir.c_str(), "rb");

    if (rwops != nullptr) {
        return rwops;
    }

    // Give up
    Log((std::string("Failed to fine: ") + file).c_str());

    return nullptr;
}

SDL_RWops *open_read_file(std::string const &path, std::string const &file)
{
    return open_read_file(path + file);
}

SDL_RWops *open_write_file(std::string const &file)
{
    std::filesystem::path outfile = base_path / file;
    SDL_RWops *rwops = SDL_RWFromFile(outfile.c_str(), "wb");

    if (rwops != nullptr) {
        return rwops;
    }

    return SDL_RWFromFile(file.c_str(), "wb");
}

SDL_RWops *open_write_file(std::string const &path, std::string const &file)
{
    return open_write_file(path + file);
}

std::list<std::string> list_files(std::string const &dirname)
{
    std::list<std::string> fileList;

    std::filesystem::path dir = dirname;
    std::filesystem::directory_iterator itr(dirname);

    for (auto const &p : itr) {
        fileList.push_back(p.path().string());
    }

    fileList.sort();

    return fileList;
}

std::string get_mounted_campaign()
{
    return mounted_campaign;
}

bool mount_campaign_package(std::string const &id)
{
    if (id.empty()) {
        return false;
    }

    Log(std::string("Mounting campaign package: " + id).c_str());

    std::string filename = id + ".glad";
    std::filesystem::path campaign = get_user_path() / "campaigns" / filename;

    if (!std::filesystem::exists(campaign)) {
        Log("Failed to mount campaign %s\n", filename.c_str());
        mounted_campaign.clear();

        return false;
    }

    mounted_campaign = id;

    return true;
}

bool unmount_campaign_package(std::string const &id)
{
    if (id.empty()) {
        return true;
    }

    std::string filename = id + ".glad";
    std::filesystem::path campaign = get_user_path() / "campaigns" / filename;

    if (!std::filesystem::exists(campaign)) {
        Log("Failed to unmount campaign file %s\n", campaign.c_str());

        return false;
    }

    mounted_campaign.clear();

    return true;
}

bool remount_campaign_package()
{
    std::string id(get_mounted_campaign());

    if (!unmount_campaign_package(id)) {
        return false;
    }

    return mount_campaign_package(id);
}

std::list<std::string> list_campaigns()
{
    std::list<std::string> ls = list_files("campaigns/");

    for (std::list<std::string>::iterator e = ls.begin(); e != ls.end(); e++) {
        size_t pos = e->rfind(".glad");

        if (pos == std::string::npos) {
            // Not a campaign package
            e = ls.erase(e);
        } else {
            // Remove the extension
            *e = e->substr(0, pos);
        }
    }

    return ls;
}

std::list<Sint32> list_levels()
{
    std::list<std::string> ls = list_files("scen/");
    std::list<Sint32> result;

    std::list<std::string>::iterator e = ls.begin();

    while (e != ls.end()) {
        size_t pos = e->rfind(".fss");

        if (pos == std::string::npos) {
            // Not a scen file
            e = ls.erase(e);

            continue;
        } else {
            // Remove the extension
            *e = e->substr(0, pos);

            if (e->substr(0, 4) != "scen") {
                e = ls.erase(e);

                continue;
            }

            *e = e->substr(4, std::string::npos);
            result.push_back(atoi(e->c_str()));
        }

        e++;
    }

    result.sort();

    return result;
}

std::vector<Sint32> list_levels_v()
{
    std::list<Sint32> levels(list_levels());

    std::vector<Sint32> result(levels.begin(), levels.end());

    return result;
}

// Delete this level from the mounted campaign
void delete_level(Sint32 id)
{
    std::string campaign = get_mounted_campaign();

    if (campaign.empty()) {
        return;
    }

    cleanup_unpacked_campaign();

    std::stringstream path;

    // Delete data file
    path << "scen" << id << ".fss";
    std::filesystem::path data_file = get_user_path() / "temp" / path.str();
    path.clear();

    std::ios state(nullptr);
    state.copyfmt(path);
    path << std::setfill('0') << std::setw(4) << id;
    path.copyfmt(state);
    path << ".pix";

    // Delete terrain file
    std::filesystem::path terrain_file = get_user_path() / "temp" / "pix" / path.str();
    std::filesystem::remove(terrain_file);

    repack_campaign(campaign);

    // Remount for consistency in PhysFS
    remount_campaign_package();
}

void delete_campaign(std::string const &id)
{
    std::string filename = id + ".glad";
    std::filesystem::remove(get_user_path() / "campaigns" / filename);
}

std::list<std::string> explode(std::string const &str, Uint8 delimiter)
{
    std::list<std::string> result;

    size_t oldPos = 0;
    size_t pos = str.find_first_of(delimiter);

    while (pos != std::string::npos) {
        result.push_back(str.substr(oldPos, pos - oldPos));
        oldPos = pos + 1;
        pos = str.find_first_of(delimiter, oldPos);
    }

    result.push_back(str.substr(oldPos, std::string::npos));

    return result;
}

void create_dataopenglad()
{
    std::filesystem::create_directory(get_user_path());
    std::filesystem::permissions(get_user_path(),
                                 std::filesystem::perms::owner_all
                                 | std::filesystem::perms::group_all);

    std::filesystem::create_directory(get_user_path() / "campaigns");
    std::filesystem::permissions(get_user_path() / "campaigns",
                                 std::filesystem::perms::owner_all
                                 | std::filesystem::perms::group_all);

    std::filesystem::create_directory(get_user_path() / "save");
    std::filesystem::permissions(get_user_path() / "save",
                                 std::filesystem::perms::owner_all
                                 | std::filesystem::perms::group_all);

    std::filesystem::permissions(get_user_path() / "cfg",
                                 std::filesystem::perms::owner_all
                                 | std::filesystem::perms::group_all);
}

void restore_default_campaigns()
{
#ifndef FORCE_RESTORE_DEFAULT_CAMPAIGNS
    std::filesystem::path campaigns = base_path / "campaigns" / "org.openglad.gladiator.glad";
    if (!std::filesystem::exists(campaigns)) {
        copy_file(get_asset_path() / "builtin" / "org.openglad.gladiator.glad",
                  get_user_path() / "campaigns" / "org.openglad.gladiator.glad");
    }
#else

    copy_file(get_asset_path() / "builtin" / "org.openglad.gladiator.glad",
              get_user_path() / "campaigns" / "org.openglad.gladiator.glad");
#endif
}

void restore_default_settings()
{
    copy_file(get_asset_path() / "cfg" / "openglad.ini",
              get_user_path() / "cfg" / "openglad.ini");
}

void io_init(std::string const &path)
{
    // Make sure our directory tree exists and is set up
    create_dataopenglad();

    base_path = path;
    write_path = get_user_path();

    if (!std::filesystem::exists(write_path)) {
        Log("Failed to mount user data path.\n");

        exit(1);
    }

    restore_default_campaigns();

    /*
     * NOTES!
     *
     * PhysFS cannot grab files from the assets folder because they're actually
     * inside the apk. SDL_RWops does some magic to figure out a file descriptor
     * from JIN. This means that I cannot use PhysFS to get any assets at all.
     * So for simple assests, I need to check PhysFS first, then fall back to
     * SDL_RWops from the assets folder. For campaign packages, I can copy them
     * to the internal storage and they'll live happily there, accessed by
     * PhysFS. SDL_RWops size checking on Android doesn't seem to work!
     */

    // Open up the default campaign
    Log("Mounting default campaign...");

    if (!mount_campaign_package("org.openglad.gladiator")) {
        Log("Failed to mount default campaign\n");

        exit(1);
    }

    Log("Mounted default campaign...");

    // Set up paths for default assets
    pix_path = get_asset_path() / "pix";
    if (!std::filesystem::exists(pix_path)) {
        Log("Failed to mount default pix path.\n");
    }

    sound_path = get_asset_path() / "sound";
    if (!std::filesystem::exists(sound_path)) {
        Log("Failed to mount default sound path.\n");
    }

    cfg_path = get_asset_path() / "cfg";
    if (!std::filesystem::exists(cfg_path)) {
        Log("Failed to mount default cfg path.\n");
    }
}

void io_exit()
{
    base_path = "";
    write_path = "";
    pix_path = "";
    sound_path = "";
    cfg_path = "";
}

// Zip utils

/*
 * Need to implement for real. PhysFS would work, but the paths would have to be
 * in the search path and the RWops would have to be gotten from PhysFS and I
 * would have to rewire the zip input (could the archive be opened through
 * PhysFS too?). Doing it with goodio would be nice.
 */
std::list<std::string> list_paths_recursively(std::filesystem::path const &dirname)
{
    std::list<std::string> paths;

    for (auto const &p : std::filesystem::directory_iterator(dirname)) {
        if (p.is_directory()) {
            std::list<std::string> inner = list_paths_recursively(p);
            paths.insert(paths.end(), inner.begin(), inner.end());
        } else {
            paths.push_back(p.path().string());
        }
    }

    return paths;
}

bool zip_contents(std::string const &indirectory, std::string const &outfile)
{
    // TODO: Implement zip/compression

    std::filesystem::copy(indirectory, outfile);

    return true;
}

/*
 * From http://niallohiggins.com/2009/0108/mkpath-mkdir-p-alike-in-c-for-unix/
 * Function with behavior like `mkdir -p`
 */
bool mkpath(std::filesystem::path const &s, std::filesystem::perms perms)
{
    if (std::filesystem::exists(s)) {
        return true;
    }

    bool result = mkpath(s, perms);

    if (result) {
        result = std::filesystem::create_directory(s);
        std::filesystem::permissions(s, perms);
    }

    return result;
}

bool create_dir(std::string const &dirname)
{
    return mkpath(dirname,
                  std::filesystem::perms::owner_all
                  | std::filesystem::perms::group_read
                  | std::filesystem::perms::group_exec
                  | std::filesystem::perms::others_read
                  | std::filesystem::perms::others_exec);
}

bool unzip_into(std::string const &infile, std::string const &outdirectory)
{
    // TODO: implement unzip/decompression
    std::filesystem::copy(infile, outdirectory);

    return true;
}

bool create_new_map_pix(std::filesystem::path const &filename, Sint32 w, Sint32 h)
{
    /*
     * file data in form:
     * <number of frames>  1 byte
     * <x size>            1 byte
     * <y size>            1 byte
     * <pixie data>        x * y * frames bytes
     */

    Uint8 c;
    SDL_RWops *outfile = open_write_file(filename.c_str());

    if (outfile == nullptr) {
        return false;
    }

    c = 1; // Frames
    SDL_RWwrite(outfile, &c, 1, 1);
    c = w; // x size
    SDL_RWwrite(outfile, &c, 1, 1);
    c = h; // y size
    SDL_RWwrite(outfile, &c, 1, 1);

    Sint32 size = w * h;

    for (Sint32 i = 0; i < size; ++i) {
        // Color
        switch (rand() % 4) {
        case 0:
            c = PIX_GRASS1;

            break;
        case 1:
            c = PIX_GRASS2;

            break;
        case 2:
            c = PIX_GRASS3;

            break;
        case 3:
            c = PIX_GRASS4;

            break;
        }

        SDL_RWwrite(outfile, &c, 1, 1);
    }

    SDL_RWclose(outfile);

    return true;
}

bool create_new_pix(std::filesystem::path const &filename, Sint32 w, Sint32 h, Uint8 fill_color)
{
    /*
     * File data in form:
     * <number of frames>  1 byte
     * <x size>            1 byte
     * <y size>            1 byte
     * <pixie data>        x * y * frames bytes
     */

    Uint8 c;
    SDL_RWops *outfile = open_write_file(filename.c_str());

    if (outfile == nullptr) {
        return false;
    }

    c = 1; // Frames
    SDL_RWwrite(outfile, &c, 1, 1);
    c = w; // x size
    SDL_RWwrite(outfile, &c, 1, 1);
    c = h; // y size
    SDL_RWwrite(outfile, &c, 1, 1);

    // Color
    c = fill_color;
    Sint32 size = w * h;

    for (Sint32 i = 0; i < size; ++i) {
        SDL_RWwrite(outfile, &c, 1, 1);
    }

    SDL_RWclose(outfile);

    return true;
}

bool create_new_campaign_descriptor(std::filesystem::path const &filename)
{
    SDL_RWops *outfile = open_write_file(filename.c_str());

    if (outfile == nullptr) {
        return false;
    }

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

    buf << "title" << "=" << "New Campaign" << std::endl;
    temp = buf.str();
    buf.clear();

    written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

    if (written != temp.size()) {
        Log("Unable to write '%s': %s\n", "title", SDL_GetError());

        return false;
    }

    buf << "version" << "=" << "1" << std::endl;
    temp = buf.str();
    buf.clear();

    written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

    if (written != temp.size()) {
        Log("Unable to write '%s': %s\n", "version", SDL_GetError());

        return false;
    }

    buf << "first_level" << "=" << "1" << std::endl;
    temp = buf.str();
    buf.clear();

    written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

    if (written != temp.size()) {
        Log("Unable to write '%s': %s\n", "first_level", SDL_GetError());

        return false;
    }

    buf << "suggested_power" << "=" << "0" << std::endl;
    temp = buf.str();
    buf.clear();

    if (written != temp.size()) {
        Log("Unable to write '%s': %s\n", "suggested_power", SDL_GetError);

        return false;
    }

    buf << "authors" << "=" << "" << std::endl;
    temp = buf.str();
    buf.clear();

    written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

    if (written != temp.size()) {
        Log("Unable to write '%s': %s\n", "authors", SDL_GetError());

        return false;
    }

    buf << "contributors" << "=" << "" << std::endl;
    temp = buf.str();
    buf.clear();

    written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

    if (written != temp.size()) {
        Log("Unable to write '%s': %s\n", "contributors", SDL_GetError());

        return false;
    }

    buf << "description" << "=";
    buf << "A new campaign.";
    buf << std::endl;

    temp = buf.str();
    buf.clear();

    written = SDL_RWwrite(outfile, temp.c_str(), sizeof(char), temp.size());

    if (written != temp.size()) {
        Log("Unable to write '%s': %s\n", "description", SDL_GetError());

        return false;
    }

    SDL_RWclose(outfile);

    return true;
}

bool create_new_scen_file(std::filesystem::path const &scenfile, std::string const &gridname)
{
    /*
     * TODO: It would be nice to store all the level data in a class, then have
     *       saving code all in one place.
     *
     * Format of a scenario object list file is: (ver. 8)
     * 3-byte header: 'FSS'
     * 1-byte version number (from graphlib.hpp)
     * 8-byte grid file name
     * 30-byte scenario title
     * 1-byte scenario_type
     * 2-byte par-value for level
     * 2-byte (Sint32) = total objects to follow
     * List of n objects, each of 20-bytes of form:
     * 1-byte ORDER
     * 1-byte FAMILY
     * 2-byte Sint32 xpos
     * 2-byte Sint32 ypos
     * 1-byte TEAM
     * 1-byte current facing
     * 1-byte current command
     * 1-byte level // This is 2 bytes in version 7+
     * 12-byte name
     * 10-byte RESERVED
     * ---
     * 1-byte Number of lines of text to load
     * List of n lines of text, each of form:
     * 1-byte character width of line
     * m-byte == Characters on this line
     */

    std::string header("FSS");
    Uint8 version = 8;
    std::string grid_file_name(gridname, 8);
    std::string scenario_title("New Level", 30);
    Uint8 scenario_type = 1; // SCEN_TYPE_CAN_EXIT
    Uint16 par_value = 1;
    Uint16 num_objects = 0;
    // std::string reserved("MSTRMSTRMSTRMSTR");
    Uint8 num_lines = 1;
    std::string line_text("A new scenario.", 50);
    Uint8 line_length = strlen(line_text.c_str());

    SDL_RWops *outfile = open_write_file(scenfile.c_str());

    if (outfile == nullptr) {
        Log("Could not open file for writing: %s\n", scenfile.c_str());

        return false;
    }

    // Write it out
    SDL_RWwrite(outfile, header.c_str(), 1, 3);
    SDL_RWwrite(outfile, &version, 1, 1);
    SDL_RWwrite(outfile, grid_file_name.c_str(), 1, 8);
    SDL_RWwrite(outfile, scenario_title.c_str(), 1, 30);
    SDL_RWwrite(outfile, &scenario_type, 1, 1);
    SDL_RWwrite(outfile, &par_value, 2, 1);

    SDL_RWwrite(outfile, &num_objects, 2, 1);
    // No objects to write

    SDL_RWwrite(outfile, &num_lines, 1, 1);
    SDL_RWwrite(outfile, &line_length, 1, 1);
    SDL_RWwrite(outfile, line_text.c_str(), line_length, 1);

    SDL_RWclose(outfile);

    return true;
}

bool unpack_campaign(std::string const &campaign_id)
{
    std::string filename = campaign_id + ".glad";
    return unzip_into(get_user_path() / "campaigns" / filename,
                      get_user_path() / "temp");
}

bool repack_campaign(std::string const &campaign_id)
{
    std::string filename = campaign_id + ".glad";
    std::filesystem::path outfile = get_user_path() / "campaigns" / filename;
    std::filesystem::remove(outfile);

    return zip_contents(get_user_path() / "temp", outfile);
}

void cleanup_unpacked_campaign()
{
    std::filesystem::remove_all(get_user_path() / "temp");
}
