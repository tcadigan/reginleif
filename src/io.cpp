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

#include "dirent.h"
#include "physfs.h"
#include "physfsrwops.h"
#include "yam.h"
#include "zip.h"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>

#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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

std::string get_user_path()
{
    std::string path = getenv("HOME");
    path += "/.openglad/";

    return path;
}

std::string get_asset_path()
{
    // FIXME: This won't typically work for *nix

    return "/usr/share/openglad/";
}

SDL_RWops *open_read_file(std::string const &file)
{
    SDL_RWops *rwops = nullptr;

    // Log((std::string("Trying via PHYSFS: ") + file).c_str());
    rwops = PHYSFSRWOPS_openRead(file.c_str());

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
    std::string user_dir = get_user_path() + "/" + file;
    // Log((std::string("Trying to open: ") + user_dir).c_str());
    rwops = SDLRwFromFile(user_dir.c_str(), "rb");

    if (rwops != nullptr) {
        return rwops;
    }

    // Now try opening in the asset directory
    std::string asset_dir = get_asset_path() + "/" + file;
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
    SDL_RWops *rwops = PHYSFSRWOPS_openWrite(file.c_str());

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

    Uint8 **files = PHYSFS_enumerateFiles(dirname.c_str());
    Uint8 **p = files;

    while ((p != nullptr) && (*p != nullptr)) {
        fileList.push_back(*p);
        ++p;
    }

    PHYSFS_freeList(files);

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

    std::string filename = get_user_path() + "campaigns/" + id + ".glad";

    if (!PHYSFS_mount(filename.c_str(), nullptr, 0)) {
        Log("Failed to mount campaign %s: %s\n", filename.c_str(), PHYSFS_getLastError());
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

    std::string filename = get_user_path() + "campaigns/" + id + ".glad";

    if (!PHYSFS_removeFromSearchPath(filename.c_str())) {
        Log("Failed to unmount campaign file %s: %s\n", filename.c_str(), PHYSFS_getLastError());

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

    for (std::list<std::string>::iterator e = ls.begin(); e != ls.end; e++) {
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

    stringstream path;
    path << get_user_path() << "temp/scen/scen" << id << ".fss";

    // Delete data file
    remove(std::string(path.str(), 256).c_str());

    path.clear();
    std::ios state(nullptr);
    state.copyfmt(path);
    path << get_user_path() << "temp/pix/scen" << std::setfill('0') << std::setw(4) << id;
    path.copyfmt(state);
    path << ".pix";

    // Delete terrain file
    remove(std::string(path.str(), 256).c_str());

    replack_campaign(campaign);

    // Remount for consistency in PhysFS
    remount_campaign_package();
}

void delete_campaign(std::string const &id)
{
    stringstream path;
    path << get_user_path() << "campaigns/" << id << ".glad";

    remove(std::string(path.str(), 256).c_str());
}

std::list<std::string> explode(std::string const &str, Uint8 delimiter)
{
    std::list<std::string> result;

    size_t oldPos = 0;
    size_t pos = str.find_first_of(delimiter);

    while (pos != str::string::npos) {
        result.push_back(str.substr(oldPos, pos - oldPos));
        oldPos = pos + 1;
        pos = str.find_first_of(delimiter, oldPos);
    }

    result.push_back(str.substr(oldPos, std::string::npos));

    return result;
}

void copy_file(std::string const &filename, std::string const &dest_filename)
{
    Log("Copying file: %s\n", filname.c_str());
    SDL_RWops *in = SDL_RWFromFile(filename.c_str(), "rb");

    if (in == nullptr) {
        Log("Could not open file to copy.\n");

        return;
    }

    Sint32 size = 0;

    // Grab the data
    Uint8 *data = static_cast<Uint8 *>(malloc(size));

    // Save it to another file
    Log("Copying to: %s\n", dest_filename.c_str());

    SDL_RWops *out = SDL_RWFromFile(dest_filename.c_str(), "wb");

    if (out == nullptr) {
        Log("Could not open destination file.\n");
        SDL_RWclose(in);

        return;
    }

    Sint32 total = 0;
    Sint32 len = SDL_RWread(in, data, 1, size);

    while (len > 0) {
        SDL_RWwrite(out, data, 1, len);
        total += len;
        len = SDL_RWread(in, data, 1, size);
    }

    SDL_RWclose(in);
    SDL_RWclose(out);
    free(data);

    Log("Copied %d bytes.\n", total);
}

void create_dataopenglad()
{
    std::string user_path = get_user_path();
    mkdir(user_path.c_str(), 0770);
    mkdir((user_path + "campaigns/").c_str(), 0770);
    mkdir((user_path + "save/").c_str(), 0770);
    mkdir((user_path + "cfg/").c_str(), 0770);
}

void restore_default_campaigns()
{
#ifndef FORCE_RESTORE_DEFAULT_CAMPAIGNS
    if (!PHYSFS_exists("campaigns/org.openglad.gladiator.glad")) {
        copy_file(get_asset_path() + "builtin/org.openglad.gladiator.glad",
                  get_user_path() + "campaigns/org.openglad.gladiator.glad");
    }
#else

    copy_file(get_asset_path() + "builtin/org/openglad.gladiator.glad",
              get_user_path() + "campaigns/org.openglad.gladiator.glad");
#endif
}

void restore_default_settings()
{
    copy_file(get_asset_path() + "cfg/openglad.yaml",
              get_user_path() + "cfg/openglad.yaml");
}

void io_init(std::string const &path)
{
    // Make sure our directory tree exists and is set up
    create_dataopenglad();

    PHYSFS_init(path.c_str());
    PHYSFS_setWriteDir(get_user_path().c_str());

    if (!PHYSFSmount(get_user_path().c_str(), nullptr, 1)) {
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

    if (!mount_compaign_package("org.openglad.gladiator")) {
        Log("Failed to mount default campaign: %s\n", PHYSFS_getLastError());

        exit(1);
    }

    Log("Mounted default campaign...");

    // Set up paths for default assets
    if (!PHYSFS_mount((get_asset_path() + "pix/").c_str(), "pix/", 1)) {
        Log("Failed to mount default pix path.\n");
    }

    if (!PHYSFS_mount((get_asset_path() + "sound/").c_str(), "sound/", 1)) {
        Log("Failed to mount default sound path.\n");
    }

    if (!PHYSFS_mount((get_asset_path() + "cfg/").c_str(), "cfg/", 1)) {
        Log("Failed to mount default cfg path.\n");
    }
}

void io_exit()
{
    PHYSFS_deinit();
}

// Zip utils

/*
 * Need to implement for real. PhysFS would work, but the paths would have to be
 * in the search path and the RWops would have to be gotten from PhysFS and I
 * would have to rewire the zip input (could the archive be opened through
 * PhysFS too?). Doing it with goodio would be nice.
 */
std::list<std::string> list_paths_recursively(std::string const &dirname)
{
    std::string _dirname = dirname;

    if (!_dirname.empty() && (_dirname.back() != '/')) {
        _dirname += '/';
    }

    std::list<std::string> ls;

    DIR *dir = opendir(_dirname.c_str());

    if (dir == nullptr) {
        return ls;
    }

    dirent *entry = readdir(dir);

    while (entry != nullptr) {
        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
            continue;
        }

        bool cond;

        cond = (entry->d_type == DT_DIR);

        if (cond) {
            std::list<std::string> sublist = list_paths_recursively(_dirname + entry->d_name);
            std::string subdir = entry->d_name;

            if (!subdir.empty() && (subdir.back() != '/')) {
                subdir += '/';
            }

            ls.push_back(subdir);

            for (std::list<std::string>::iterator e = sublist.begin(); e != sublist.end(); e++) {
                ls.push_back(subdir + *e);
            }
        } else {
            ls_push_back(entry->d_name);
        }

        entry = readdir(dir);
    }

    closedir(dir);

    return ls;
}

bool zip_contents(std::string const &indirectory, std::string const &outfile)
{
    std::string indir = indirectory;

    if (!indir.empty() && (indir.back() != '/')) {
        indir += '/';
    }

    // Log("Zipping %s as %s\n", indir.c_str(), outfile.c_str());

    Sint32 err = 0;
    zip *archive = zip_open(outfile.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err);

    if (archive == nullptr) {
        return false;
    }

    struct zip_source *s;
    std::string src_name;
    std::string dest_name;

    std::list<std::string> files = list_paths_recursively(indir);

    for (std::list<std::string>::iterator e = files.begin(); e != files.end(); e++) {
        src_name = indir + e;
        src_name.resize(512);

        dest_name = e;
        dest_name.resize(512);

        if (src_name[strlen(src_name.c_str()) - 1] == '/') {
            if (zip_dir_add(archive, dest_name, ZIP_FL_ENC_GUESS) < 0) {
                // Error
                Log("Errors adding dir: %s\n", zip_strerror(archive));
            }
        } else {
            s = zip_source_file(archive, src_name, 0, -1);

            if ((s == nullptr) || (zip_file_add(archive, dest_name, s, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS) < 0)) {
                // Error
                zip_source_free(s);
                Log("Error adding file: %s\n", zip_strerror(archive));
            }
        }
    }

    if (zip_close(archive) < 0) {
        Log("Error flushing zip file output: %s\n", zip_strerror(archive));

        return false;
    }

    return true;
}

/*
 * From http://niallohiggins.com/2009/0108/mkpath-mkdir-p-alike-in-c-for-unix/
 * Function with behavior liek `mkdir -p`
 */
Sint32 mkpath(std::string const &s, mode_t mode)
{
    Sint32 rv;

    rv = -1;

    if ((strcmp(s, ".") == 0) || (strcmp(s, "/") == 0) || ((strlen(s) == 3) && (s[2] == '/'))) {
        return 0;
    }

    if (s.empty()) {
        exit(1);
    }

    std::string path(s);
    std::string parent(dirname(s));

    if (parent == ".") {
        return rv;
    }

    if ((mkpath(parent, mode) == -1) && (errno != EEXIST)) {
        return rv;
    }

    if ((mkdir(path, mode) == -1) && (errno != EEXIST)) {
        rv = -1;
    } else {
        rv = 0;
    }

    return rv;
}

bool create_path_to_file(Uint8 const *filename)
{
    Uint8 const *c = strrchr(filename, '/');

    if (c == nullptr) {
        c = strrchr(filename, '\\');
    }

    if (c == nullptr) {
        return true;
    }

    std::string buf(filename, 512);
    buf[c - filename] = '\0';

    return (mkpath(buf, 0755) >= 0);
}

bool create_dir(std::string const &dirname)
{
    return (mkpath(dirname.c_str(), 0755) >= 0);
}

bool unzip_into(std::string const &infile, std::string const &outdirectory)
{
    std::string outdir(outdirectory);

    if (!outdir.empty() && (outdir.back() != '/')) {
        outdir += '/';
    }

    // Log("Unzipping %s\n", infile.c_str());

    Sint32 err = 0;
    zip *archive = zip_open(infile.c_str(), 0, &err);

    if (archive == nullptr) {
        return false;
    }

    struct zip_stat status;
    struct zip_file *file;
    Sint32 buf_size = 512;
    Uint8 buf[buf_size];

    for (Sint32 i = 0; i < zip_get_num_entries(archive, 0), ++i) {
        if (zip_stat_index(archive, i, 0, &status) == 0) {
            Sint32 len = strlen(status.name);

            if (status.name[len - 1] == '/') {
                snprintf(buf, buf_size, "%s%s", outdir.c_str(), status.name);
                create_dir(buf);
            } else {
                file = zip_fopen_index(archive, i, 0);

                if (file == nullptr) {
                    // Error
                    continue;
                }

                snprintf(buf, buf_size, "%s%s", outdir.c_str(), status.name);
                create_path_to_file(buf);
                SDL_RWops *rwops = open_write_file(outdir.c_str(), status.name);

                if (rwops == nullptr) {
                    // Error
                    continue;
                }

                size_t sum = 0;

                while (sum < status.size) {
                    len = zip_fread(file, buf, buf_size);

                    if (len < 0) {
                        // Error
                    }

                    SDL_RWwrite(rwops, buf, 1, len);
                    sum += len;
                }

                SDL_RWclose(rwops);
                zip_fclose(file);
            }
        } else {
            // Error
        }
    }

    return (zip_close(archive) >= 0);
}

bool create_new_map_pix(std::string const &filename, Sint32 w, Sint32 h)
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

bool create_new_pix(std::string const &filename, Sint32 w, Sint32 h, Uint8 fill_color)
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

bool create_new_campaign_descriptor(std::string const &filename)
{
    SDL_RWops *outfile = open_write_file(filename.c_str());

    if (outfile == nullptr) {
        return false;
    }

    Yam yam;
    yam.set_output(rwops_write_handler, outfile);

    yam.emit_pair("format_version", "1");
    yam.emit_pair("title", "New Campaign");
    yam.emit_pair("version", "1");
    yam.emit_pair("first_level", "1");
    yam.emit_pair("suggested_power", "0");
    yam.emit_pair("authors", "");
    yam.emit_pair("contributors", "");
    yam.emit_pair("description", "A new campaign.");

    yam.close_output();
    SDL_RWclose(outfile);

    return true;
}

bool create_new_scen_file(std::string const &scenfile, std::string const &gridname)
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
    SDL_RWwrite(outfile, grid_file_name, 1, 8);
    SDL_RWwrite(outfile, scenario_title.c_str(), 1, 30);
    SDL_RWwrite(outfile, &scenario_type, 1, 1);
    SDL_RWwrite(outfile, &par_value, 2, 1);

    SDL_RWwrite(outfile, &num_objects, 2, 1);
    // No objects to write

    SDL_RWwrite(outfile, &numlines, 1, 1);
    SDL_RWwrite(outfile, &line_length, 1, 1);
    SDL_RWwrite(outfile, line_text.c_str(), line_length, 1);

    SDL_RWclose(outfile);

    return true;
}

bool unpack_campaign(std::string const &campaign_id)
{
    return unzip_into(get_user_path() + "campaigns/" + campaign_id + ".glad",
                      get_user_path() + "temp/");
}

bool repack_campaign(std::string const &campaign_id)
{
    std::string outfile(get_user_path() + "campaigns/" + campaign_id + ".glad");
    remove_file(outfile.c_str());

    return zip_contents(get_user_path() + "temp/", outfile);
}

void cleanup_unpacked_campaign()
{
    // Recursive delete
    std::list<std::string> ls = list_paths_recursively(get_user_path() + "temp");

    for (std::list<std::string>::reverse_iterator e = ls.rbegin(); e != ls.rend(); e++) {
        std::string path = get_user_path() + "temp/" + *e;
        remove(path.c_str());
        rmdir(path.c_str());
    }

    rmdir((get_user_path() + "temp").c_str());
}
