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
#ifndef __IO_HPP__
#define __IO_HPP__

#include <filesystem>
#include <list>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

void io_init(std::filesystem::path const &path);
void io_exit();

std::filesystem::path get_user_path();
bool create_dir(std::filesystem::path const &dirname);

SDL_RWops *open_read_file(std::filesystem::path const &file);
SDL_RWops *open_write_file(std::filesystem::path const &file);

std::list<std::filesystem::path> list_files(std::filesystem::path const &dirname);

std::filesystem::path get_mounted_campaign();
bool mount_campaign_package(std::filesystem::path const &id);
bool unmount_campaign_package(std::filesystem::path const &id);
std::list<std::filesystem::path> list_campaigns();
std::list<Sint32> list_levels();
std::vector<Sint32> list_levels_v();

void restore_default_campaigns();
void restore_default_settings();

bool save_settings();
bool load_settings();

void delete_level(Sint32 id);
void delete_campaign(std::filesystem::path const &id);

bool zip_contents(std::filesystem::path const &indirectory, std::filesystem::path const &outfile);
bool unzip_into(std::filesystem::path const &infile, std::filesystem::path const &outdirectory);

bool create_new_campaign(std::filesystem::path const &campaign_id);
bool unpack_campaign(std::filesystem::path const &campaign_id);
bool repack_campaign(std::filesystem::path const &campaign_id);
bool remount_campaign_package();

void cleanup_unpacked_campaign();

bool create_new_map_pix(std::filesystem::path const &filename, Sint32 w, Sint32 h);
bool create_new_pix(std::filesystem::path const &filename, Sint32 w, Sint32 h, Uint8 fill_color=0);
bool create_new_campaign_descriptor(std::filesystem::path const &filename);
bool create_new_scen_file(std::filesystem::path const &scenfile, std::string const &gridname);

#endif
