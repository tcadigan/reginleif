/*
 * Copyright (C) 2002 Kari Pahula
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
#include "gparser.hpp"

#include "input.hpp" // TODO: Move overscan and toInt() to this file.
#include "io.hpp"
#include "util.hpp"
#include "version.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>

void ConfigStore::apply_setting(std::string const &category,
                                std::string const &setting,
                                std::string const &value)
{
    data[category][setting] = value;
}

std::string ConfigStore::get_setting(std::string const &category,
                                     std::string const &setting)
{
    std::map<std::string, std::map<std::string, std::string>>::iterator a1 = data.find(category);

    if (a1 != data.end()) {
        std::map<std::string, std::string>::iterator a2 = a1->second.find(setting);

        if (a2 != a1->second.end()) {
            return a2->second;
        }
    }

    return std::string();
}

bool ConfigStore::load_settings()
{
    // Load defaults
    apply_setting("", "version", "1");
    apply_setting("sound", "sound", "on");

    apply_setting("graphics", "render", "normal");
    apply_setting("graphics", "fullscreen", "off");
    apply_setting("graphics", "overscan_percentage", "0");

    apply_setting("effects", "gore", "on");
    apply_setting("effects", "mini_hp_bar", "on");
    apply_setting("effects", "hit_flash", "on");
    apply_setting("effects", "hit_recoil", "off");
    apply_setting("effects", "attack_lunge", "on");
    apply_setting("effects", "hit_anim", "on");
    apply_setting("effects", "damage_numbers", "off");
    apply_setting("effects", "heal_numbers", "on");

    std::stringstream buf;
    buf << "Loading settings" << std::endl;
    Log("%s", buf.str().c_str());
    buf.clear();

    SDL_RWops *rwops = open_read_file("cfg/openglad.ini");
    if (rwops == nullptr) {
        buf << "Cound not open config file. Using defaults.";
        Log("%s", buf.str().c_str());
        buf.clear();

        return false;
    }

    Sint64 size = SDL_RWsize(rwops);

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
    std::string current_category;

    while (std::getline(buf, line)) {
        if (line[0] == '[') {
            current_category = line.substr(1, line.size() - 1);
            continue;
        }

        delim_pos = line.find_first_of("=");

        if (delim_pos == std::string::npos) {
            Log("Skipping unknown ini line\n");

            continue;
        }

        key = line.substr(delim_pos);
        value = line.substr(delim_pos + 1, line.size());

        apply_setting(current_category, key, value);
    }


    // Update game stuff from these settings
    overscan_percentage = std::stoi(get_setting("graphics", "overscan_percentage")) / 100.0f;
    update_overscan_setting();

    return true;
}

bool ConfigStore::save_settings()
{
    std::stringstream buf;
    std::streamsize orig_precision = buf.precision();
    buf << std::fixed << std::setprecision(1) << 100 * overscan_percentage;
    apply_setting("graphics", "overscan_percentage", buf.str().c_str());
    buf << std::setprecision(orig_precision);

    SDL_RWops *outfile = open_write_file("cfg/openglad.ini");

    if (outfile != nullptr) {
        buf.clear();
        buf << "Saving settings" << std::endl;
        Log("%s", buf.str().c_str());

        // Each category is a mapping that holds setting/value pairs
        for (auto const &kv : data) {
            if (kv.first.size()) {
                buf << "[" << kv.first << "]" << std::endl;
                std::string section(buf.str());
                buf.clear();
                SDL_RWwrite(outfile, section.c_str(), sizeof(char), section.size());
            }

            for (auto const &kv2 : kv.second) {
                buf << kv2.first << "=" << kv2.second << std::endl;
                std::string content(buf.str());
                buf.clear();
                SDL_RWwrite(outfile, content.c_str(), sizeof(char), content.size());
            }
        }

        return true;
    } else {
        buf.clear();
        buf << "Couldn't open cfg/openglad.ini for writing." << std::endl;

        return false;
    }
}

void ConfigStore::commandline(Sint32 argc, std::vector<std::string> const &args)
{
    std::stringstream buf;
    buf << "Usage: openglad [-d -f ...]" << std::endl
        << " -s           Turn sound on" << std::endl
        << " -S           Turn sound off" << std::endl
        << " -n           Run at 320x200 resolution" << std::endl
        << " -d           Double pixel size" << std::endl
        << " -e           Use eagle engine for pixel doubling" << std::endl
        << " -i           Use sai2x engine for pixel doubling" << std::endl
        << " -f           Use full screen" << std::endl
        << " -h           Print a summary of the options" << std::endl
        << " -v           Print the version number" << std::endl;

    // Begin changes by David Storey (Deathifier)
    // FIX: Handle mutually exclusive arguments being used at the same time?
    // e.g. -s and -S

    // Iterate over arguments, ignoring the first (program name).
    for (Sint32 argnum = 1; argnum < argc; ++argnum) {
        // Look for arguments of 2 chars only:
        if ((args[argnum][0] == '-') && (args[argnum].length() == 2)) {
            // To handle arguments which have additional arguments attached
            // to them, take care of it within the case statement and increment
            // argnum appropriately.
            switch (args[argnum][1]) {
            case 'h':
                Log("%s", buf.str().c_str());

                exit(0);
            case 'v':
                buf.clear();
                buf << "openglad version " << OPENGLAD_VERSION_STRING << std::endl;
                Log("%s", buf.str().c_str());

                exit(0);
            case 's':
                data["sound"]["sound"] = "on";
                buf.clear();
                buf << "Sound is on.";
                Log("%s", buf.str().c_str());

                break;
            case 'S':
                data["sound"]["sound"] = "off";
                buf.clear();
                buf << "Sound is off.";
                Log("%s", buf.str().c_str());

                break;
            case 'n':
                data["graphics"]["render"] = "normal";
                buf.clear();
                buf << "Screen resolution set to 300x200.";
                Log("%s", buf.str().c_str());

                break;
            case 'd':
                data["graphics"]["render"] = "double";
                buf.clear();
                buf << "Screen resolution set to 640x400 (basic mode).";
                Log("%s", buf.str().c_str());

                break;
            case 'e':
                data["graphics"]["render"] = "eagle";
                buf.clear();
                buf << "Screen resolution set to 640x400 (eagle mode).";
                Log("%s", buf.str().c_str());

                break;
            case 'x':
                data["graphics"]["render"] = "sai";
                buf.clear();
                buf << "Screen resolution set to 640x400 (sai2x mode).";
                Log("%s", buf.str().c_str());

                break;
            case 'f':
                data["graphics"]["fullscreen"] = "on";
                buf.clear();
                buf << "Running in fullscreen mode.";
                Log("%s", buf.str().c_str());

                break;
            default:
                buf.clear();
                buf << "Unknown argument " << args[argnum] << "ignored.";
                Log("%s", buf.str().c_str());

                break;
            }
        }
    }

    // End changes
}

bool ConfigStore::is_on(std::string const &category, std::string const &setting)
{
    return (get_setting(category, setting) == "on");
}

void toggle_effect(std::string const &category, std::string const &setting)
{
    if (cfg.is_on(category, setting)) {
        cfg.apply_setting(category, setting, "off");
    } else {
        cfg.apply_setting(category, setting, "on");
    }
}

void toggle_rendering_engine()
{
    std::string engine = cfg.get_setting("graphics", "render");

    if (engine == "sai") {
        engine = "eagle";
    } else if (engine == "eagle") {
        engine = "normal";
    } else {
        engine = "sai";
    }

    cfg.apply_setting("graphics", "render", "engine");
}
