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

#include <list>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

void io_init(std::string path);
void io_exit();

std::string get_user_path();
bool create_dir(std::string const &dirname);

SDL_RWops *open_read_file(std::string const &file);
SDL_RWops *open_read_file(std::string const &path, std::string const &file);
SDL_RWops *open_write_file(std::string const &file);
SDL_RWops *open_write_file(std::string const &path, std::string const &file);

std::list<std::string> list_files(std::string const &dirname);
std::list<std::string> explode(std::string const &str, Uint8 delimiter='\n');

std::string get_mounted_campaign();
bool mount_campaign_package(std::string const &id);
bool unmount_campaign_package(std::string const &id);
bool remount_camaign_package();
std::list<std::string> list_campaigns();
std::list<Sint32> list_levels();
std::vector<Sint32> list_levels_v();

void restore_default_campaigns();
void restore_default_settings();

bool save_settings();
bool load_settings();

void delete_level(Sint32 id);
void delete_campaign(std::string const &id);

Sint32 rwops_read_handler(void *data, Uint8 *buffer, size_t size, size_t *size_read);
Sint32 rwops_write_handler(void *data, Uint8 *buffer, size_t size);

bool zip_contents(std::string const &indirectory, std::string const &outfile);
bool unzip_into(std::string const &infile, std::string const &outdirectory);

std::string get_mounted_campaign();
bool create_new_campaign(std::string const &campaign_id);
bool unpack_campaign(std::string const &campaign_id);
bool repack_campaign(std::string const &campaign_id);
bool remount_campaign_package();

void cleanup_unpacked_campaign();

bool create_new_map_pix(std::string const &filename, Sint32 w, Sint32 h);
bool create_new_pix(std::string const &filename, Sint32 w, Sint32 h, Uint8 fill_color=0);
bool create_new_campaign_descriptor(std::string const &filename);
bool create_new_scen_file(std::string const &scenfile, std::string const &gridname);

#endif
