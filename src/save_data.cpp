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
#include "save_data.hpp"

#include "campaign_result.hpp"
#include "guy.hpp"
#include "io.hpp"
#include "picker.hpp"
#include "util.hpp"
#include "walker.hpp"

#include <cstring>
#include <filesystem>

SaveData::SaveData()
    : current_campaign(std::filesystem::path("org.openglad.gladiator"))
    , scen_num(1)
    , score(0)
    , totalcash(0)
    , totalscore(0)
    , my_team(0)
    , numplayers(1)
    , allied_mode(1)
{
    completed_levels.insert(std::make_pair(std::filesystem::path("org.openglad.gladiator"), std::set<Sint32>()));
    current_levels.insert(std::make_pair(std::filesystem::path("org.openglad.gladiator"), 1));

    for (Sint32 i = 0; i < 4; ++i) {
        m_score[i] = 0;
        m_totalcash[i] = 5000;
        m_totalscore[i] = 0;
    }

    team_size = 0;

    for (Sint32 i = 0; i < MAX_TEAM_SIZE; ++i) {
        team_list[i] = nullptr;
    }
}

SaveData::~SaveData()
{
    for (Sint32 i = 0; i < MAX_TEAM_SIZE; ++i) {
        delete team_list[i];
    }
}

void SaveData::reset()
{
    current_campaign = std::filesystem::path("org.openglad.gladiator");
    completed_levels.clear();
    current_levels.clear();
    completed_levels.insert(std::make_pair(std::filesystem::path("org.openglad.gladiator"), std::set<Sint32>()));
    current_levels.insert(std::make_pair(std::filesystem::path("org.openglad.gladiator"), 1));

    totalscore = 0;
    totalcash = totalscore;
    score = totalcash;

    for (Sint32 i = 0; i < 4; ++i) {
        m_score[i] = 0;
        m_totalcash[i] = 5000;
        m_totalscore[i] = 0;
    }

    for (Sint32 i = 0; i < team_size; ++i) {
        delete team_list[i];
        team_list[i] = nullptr;
    }

    team_size = 0;
    scen_num = 1;
    my_team = 0;
    // numplayers = 1;
    // allied_mode = 1;
}

bool SaveData::load(std::filesystem::path const &filename)
{
    // For RESERVED
    char filler[50] = "GTLGTLGTLGTLGTLGTLGTLGTLGTLGTLGTLGTLGTLGTL";
    SDL_RWops *infile;
    char temptext[10] = "GTL";
    char savedgame[40];
    char temp_campaign[42] = "org.openglad.gladiator";
    Sint8 temp_version = 9;
    Uint32 newcash;
    Uint32 newscore = 0;
    // Sint16 numguys;
    Sint16 listsize;
    Sint16 i;

    std::string tempname("FRED");
    char guyname[12] = "JOE";
    Sint8 temp_order;
    Sint8 temp_family;
    Sint16 temp_str;
    Sint16 temp_dex;
    Sint16 temp_con;
    Sint16 temp_short;
    Sint16 temp_arm;
    Sint16 temp_lev;
    Uint8 temp_numplayers;
    Uint32 temp_exp;
    Sint16 temp_kills;
    Sint32 temp_level_kills;
    Sint32 temp_td;
    Sint32 temp_th;
    Sint32 temp_ts;
    Sint16 temp_teamnum; // Version 5+
    Sint16 temp_allied; // v.7+
    Sint16 temp_registered; // v.7+

    /*
     * Format of a team list file is:
     * 3-byte header: 'GTL'
     * 1-byte version number
     * 2-byte registered mark // Version 7+
     * 40-byte saved game name // Version 2+
     * 40-byte current campaign ID // Version 8+
     * 2-byte (Sint16): scenario number
     * 4-byte (Sint32): cash (unsigned)
     * 4-byte (Sint32): score (unsigned)
     * 4-byte (Sint32): cash-B (unsigned) // All alternate scores
     * 4-byte (Sint32): score-B (unsigned) // Version 6+
     * 4-byte (Sint32): cash-C (unsigned)
     * 4-byte (Sint32): score-C (unsigned)
     * 4-byte (Sint32): cash-D (unsigned)
     * 4-byte (Sint32): score-D (unsigned)
     * 2-byte allied mode // Version 7+
     * 2-byte (Sint16): # of team members in list
     * 1-byte number of players
     * List of N objects, each 58-byte of form:
     * 1-byte ORDER
     * 1-byte FAMILY
     * 12-byte name
     * 2-byte strength
     * 2-byte dexterity
     * 2-byte constitution
     * 2-byte intelligence
     * 2-byte armor
     * 2-byte level // Does not include upgrades stats in version 8 or lower
     * 4-byte unsigned experience
     * 2-byte # kills // Version 3+
     * 4-byte # total levels killed // Version 3+
     * 4-byte total damage dealt // Version 4+
     * 4-byte total hits inflicted // Version 4+
     * 4-byte total shots made // Version 4+
     * 2-byte team number
     * 8-byte RESERVED
     * List of 200 or 500 (max levels) 1-byte scenario level status // Version 1-7
     * 2-byte Number of campaigns in list // Version 8+
     * List of N campaigns // Version 8+
     * 40-byte campaign ID string
     * 2-byte current level in this campaign
     * 2-byte number of level indices in list
     * List of N level indices
     * 2-byte level index
     */

    std::filesystem::path temp_filename("save" / filename);
    // Gladiator team list
    temp_filename.replace_extension(".gtl");
    Log("Loading save: %s\n", temp_filename.c_str());

    infile = open_read_file(temp_filename);

    if (infile == nullptr) {
        Log("Failed to open save file: %s\n", temp_filename.c_str());

        return false;
    }

    completed_levels.clear();
    current_levels.clear();

    for (Sint32 i = 0; i < team_size; ++i) {
        delete team_list[i];
        team_list[i] = nullptr;
    }

    team_size = 0;

    // Read ID header
    SDL_RWread(infile, temptext, 3, 1);

    if (strcmp(temptext, "GTL")) {
        SDL_RWclose(infile);
        Log("Error, selected file is not a GTL file: %s\n", temp_filename.c_str());

        // Not a GTL file
        return false;
    }

    // Read version number
    SDL_RWread(infile, &temp_version, 1, 1);

    // Versions 7+ have a registered mark...
    if (temp_version >= 7) {
        SDL_RWread(infile, &temp_registered, 2, 1);
    }

    // Do other stuff based on version...
    if (temp_version != 1) {
        if (temp_version >= 2) {
            // Read and ignore the name
            SDL_RWread(infile, savedgame, 40, 1);
        } else {
            SDL_RWclose(infile);
            Log("Error, selected files is not version one: %s\n", filename.c_str());

            return false;
        }
    }

    save_name = savedgame;

    // Read campaign ID
    if (temp_version >= 8) {
        SDL_RWread(infile, temp_campaign, 1, 40);
        temp_campaign[40] = '\0';

        if (strlen(temp_campaign) > 3) {
            current_campaign = std::filesystem::path(temp_campaign);
        } else {
            current_campaign = std::filesystem::path("org.openglad.gladiator");
        }
    }

    // Read scenario number
    Sint16 temp_scenario;
    SDL_RWread(infile, &temp_scenario, 2, 1);
    scen_num = temp_scenario;

    // Read cash
    SDL_RWread(infile, &newcash, 4, 1);
    totalcash = newcash;

    // Read score
    SDL_RWread(infile, &newscore, 4, 1);
    totalscore = newscore;

    // Versions 6+ have a score for each possible team, 0-3
    if (temp_version >= 6) {
        for (i = 0; i < 4; ++i) {
            SDL_RWread(infile, &newcash, 4, 1);
            m_totalcash[i] = newcash;
            SDL_RWread(infile, &newscore, 4, 1);
            m_totalscore[i] = newscore;
        }
    }

    // Versions 7+ have the allied information...
    if (temp_version >= 7) {
        SDL_RWread(infile, &temp_allied, 2, 1);
        allied_mode = temp_allied;
    }

    // Get # of guys to read
    SDL_RWread(infile, &listsize, 2, 1);

    // Read the # of players
    SDL_RWread(infile, &temp_numplayers, 1, 1);

    temp_numplayers = 1;

    numplayers = temp_numplayers;

    // Read the reserved area, 31 bytes
    SDL_RWread(infile, filler, 31, 1);

    // Okay, we've read header...now read the team list data...
    for (Sint32 i = 0; i < listsize; ++i) {
        Guy *temp_guy = new Guy();
        team_list[i] = temp_guy;
        ++team_size;

        // Get temp values to be read
        temp_order = ORDER_LIVING; // May be changed later
        // Read name of current guy...
        memset(guyname, 0, 12);
        strcpy(guyname, tempname.c_str());
        // Now write all those values
        SDL_RWread(infile, &temp_order, 1, 1);
        SDL_RWread(infile, &temp_family, 1, 1);
        SDL_RWread(infile, guyname, 12, 1);
        SDL_RWread(infile, &temp_str, 2, 1);
        SDL_RWread(infile, &temp_dex, 2, 1);
        SDL_RWread(infile, &temp_con, 2, 1);
        SDL_RWread(infile, &temp_short, 2, 1);
        SDL_RWread(infile, &temp_arm, 2, 1);
        SDL_RWread(infile, &temp_lev, 2, 1);
        SDL_RWread(infile, &temp_exp, 4, 1);
        // Below here is Version 3 and up...
        SDL_RWread(infile, &temp_kills, 2, 1); // How many kills we have
        SDL_RWread(infile, &temp_level_kills, 4, 1); // Levels of kills
        // Below here is Version 4 and up...
        SDL_RWread(infile, &temp_td, 4, 1); // Total damage
        SDL_RWread(infile, &temp_th, 4, 1); // Total hits
        SDL_RWread(infile, &temp_ts, 4, 1); // Total shots
        SDL_RWread(infile, &temp_teamnum, 2, 1); // Team number

        // And the filler
        SDL_RWread(infile, filler, 8, 1);

        // Now set the values...
        temp_guy->family = temp_family;
        temp_guy->name = guyname;
        temp_guy->strength = temp_str;
        temp_guy->dexterity = temp_dex;
        temp_guy->constitution = temp_con;
        temp_guy->intelligence = temp_short;
        temp_guy->armor = temp_arm;

        if (temp_version >= 9) {
            temp_guy->set_level_number(temp_lev);
        } else {
            temp_guy->upgrade_to_level(temp_lev);
        }

        temp_guy->exp = temp_exp;

        if (temp_version >= 3) {
            temp_guy->kills = temp_kills;
            temp_guy->level_kills = temp_level_kills;
        } else {
            // Version 2 or earlier
            temp_guy->kills = 0;
            temp_guy->level_kills = 0;
        }

        if (temp_version >= 4) {
            temp_guy->total_damage = temp_td;
            temp_guy->total_hits = temp_th;
            temp_guy->total_shots = temp_ts;
        } else {
            temp_guy->total_damage = 0;
            temp_guy->total_hits = 0;
            temp_guy->total_shots = 0;
        }

        if (temp_version >= 5) {
            temp_guy->teamnum = temp_teamnum;
        } else {
            temp_guy->teamnum = 0;
        }
    }

    // Make sure the default campaign is included
    completed_levels.insert(std::make_pair(std::filesystem::path("org.openglad.gladiator"),
                                           std::set<Sint32>()));
    current_levels.insert(std::make_pair(std::filesystem::path("org.openglad.gladiator"), 1));

    if (temp_version < 8) {
        char levelstatus[MAX_LEVELS];
        memset(levelstatus, 0, 500);

        if (temp_version >= 5) {
            SDL_RWread(infile, levelstatus, 500, 1);
        } else {
            SDL_RWread(infile, levelstatus, 200, 1);
        }

        // Guaranteed to be the default campaign if version < 8
        for (Sint32 i = 0; i < 500; ++i) {
            if (levelstatus[i]) {
                add_level_completed(current_campaign, i);
            }
        }
    } else {
        Sint16 num_campaigns = 0;
        char campaign[41];
        Sint16 num_levels = 0;

        // How many campaigns are stored?
        SDL_RWread(infile, &num_campaigns, 2, 1);

        for (Sint32 i = 0; i < num_campaigns; ++i) {
            // Get the campaignID (40 chars)
            SDL_RWread(infile, campaign, 1, 40);
            campaign[40] = '\0';

            Sint16 index = 1;
            // Get the current level for this campaign
            SDL_RWread(infile, &index, 2, 1);
            current_levels[campaign] = index;

            // Get hte number of cleared levels
            SDL_RWread(infile, &num_levels, 2, 1);

            for (Sint32 j = 0; j < num_levels; ++j) {
                // Get the level index
                SDL_RWread(infile, &index, 2, 1);

                // Add it to our list
                add_level_completed(campaign, index);
            }
        }
    }

    Log("Loading campaign: %s\n", current_campaign.c_str());
    Sint32 current_level = load_campaign(current_campaign, current_levels);

    if (current_level >= 0) {
        if (scen_num != current_level) {
            Log("Error: Loaded scen_num %d, but found current_level %s\n", scen_num, current_level);
        }

        // scen_num = current_level;
    }

    SDL_RWclose(infile);

    return true;
}

void SaveData::update_guys(std::list<Walker *> &oblist)
{
    // Delete our old guys
    for (Sint32 i = 0; i < team_size; ++i) {
        delete team_list[i];
        team_list[i] = nullptr;
    }

    team_size = 0;

    // Remove new (existing) "guys" from the list and store them in this
    // SaveData to be save and trained.
    for (auto const & ob : oblist) {
        if (!ob->dead && ob->myguy) {
            // Take this one
            team_list[team_size] = new Guy(*ob->myguy);
            // Update his level from the experience
            Uint32 exp = team_list[team_size]->exp;
            team_list[team_size]->upgrade_to_level(calculate_level(team_list[team_size]->exp));
            team_list[team_size]->exp = exp;
            ++team_size;
        }
    }
}

bool SaveData::save(std::filesystem::path const &filename)
{
    // For RESERVED
    char filler[50] = "GTLGTLGTLGTLGTLGTLGTLGTLGTLGTLGTLGTLGTLGTL";
    SDL_RWops *outfile;
    char savedgame[41];
    memset(savedgame, 0, 41);
    char temp_campaign[41];
    memset(temp_campaign, 0, 41);
    char temptext[10] = "GTL";
    char temp_version = 9;

    Uint32 newcash = totalcash;
    Uint32 newscore = totalscore;
    // Sint16 numguys;
    Sint16 listsize;
    Sint16 i;

    std::string guyname("JOE");
    Sint8 temp_order;
    Sint8 temp_family;
    Sint16 temp_str;
    Sint16 temp_dex;
    Sint16 temp_con;
    Sint16 temp_short;
    Sint16 temp_arm;
    Sint16 temp_lev;
    Uint8 numplayers = this->numplayers;
    Uint32 temp_exp;
    Sint16 temp_kills;
    Sint32 temp_level_kills;
    Sint32 temp_td;
    Sint32 temp_th;
    Sint32 temp_ts;
    Sint16 temp_teamnum;
    Sint16 temp_allied;
    Sint16 temp_registered;

    /*
     * Format of a team list file is:
     * 3-byte header: 'GTL'
     * 1-byte version number
     * 2-byte registered mark // Version 7+
     * 40-byte saved game name // Version 2+
     * 40-byte current campaign ID // Version 8+
     * 2-byte (Sint16): scenario number
     * 4-byte (Sint32): cash (unsigned)
     * 4-byte (Sint32): score (unsigned)
     * 4-byte (Sint32): cash-B (unsigned) // All alternate scores
     * 4-byte (Sint32): score-B (unsigned) // Version 6+
     * 4-byte (Sint32): cash-C (unsigned)
     * 4-byte (Sint32): score-C (unsigned)
     * 4-byte (Sint32): cash-D (unsigned)
     * 4-byte (Sint32): score-D (unsigned)
     * 2-byte allied mode // Version 7+
     * 2-byte (Sint16): # of team members in list
     * 1-byte number of players
     * 31-byte RESERVED
     * List of N objects, each of 58-byte of form:
     * 1-byte ORDER
     * 1-byte FAMILY
     * 12-byte name
     * 2-byte strength
     * 2-byte dexterity
     * 2-byte constitution
     * 2-byte intelligence
     * 2-byte armor
     * 2-byte level // Does not include upgraded stats in Version 8 or lower
     * 4-byte unsigned experience
     * 2-byte # kills // Version 3+
     * 4-byte # total levels killed // Version 3+
     * 4-byte total damange dealt // Version 4+
     * 4-byte total hits inflicted // Version 4+
     * 4-byte total shots made // Version 4+
     * 2-byte team number
     * 8-byte RESERVED
     * List of 200 or 500 (max levels)
     * 1-byte scenario level status // Version 1-7
     * 2-byte number of campaigns in list // Version 8+
     * List of N campaigns // Version 8+
     * 40-byte campaign ID string
     * 2-byte current level in this campaign
     * 2-byte number of level indices in list
     * List of N level indices:
     * 2-byte level index
     */

    // strcpy(temp_filename, scen_directory);
    std::filesystem::path temp_filename("save" / filename);
    // Gladiator team list
    temp_filename.replace_extension(".gtl");
    Log("Saving save: %s\n", temp_filename.c_str());

    // Open for write
    outfile = open_write_file(temp_filename);

    if (outfile == nullptr) {
        // gotoxy(1, 22);
        Log("Error in writing team file %s\n", temp_filename.c_str());

        return false;
    }

    // Write ID header
    SDL_RWwrite(outfile, temptext, 3, 1);

    // Write version number
    SDL_RWwrite(outfile, &temp_version, 1, 1);

    // Version 7+ include a mark for registered or not
    temp_registered = 1;
    SDL_RWwrite(outfile, &temp_registered, 2, 1);

    // Write the name
    strncpy(savedgame, save_name.c_str(), 40);
    SDL_RWwrite(outfile, savedgame, 40, 1);

    // Write current campaign
    Log("Saving campaign status: %s\n", current_campaign.c_str());
    strncpy(temp_campaign, current_campaign.c_str(), 40);
    SDL_RWwrite(outfile, temp_campaign, 40, 1);

    // Write scenario number
    Sint16 temp_scenario = scen_num;
    SDL_RWwrite(outfile, &temp_scenario, 2, 1);

    // Write cash
    SDL_RWwrite(outfile, &newcash, 4, 1);

    // Write score
    SDL_RWwrite(outfile, &newscore, 4, 1);

    // Version 6+ have a score for each possible team
    for (i = 0; i < 4; ++i) {
        newcash = m_totalcash[i];
        SDL_RWwrite(outfile, &newcash, 4, 1);
        newscore = m_totalscore[i];
        SDL_RWwrite(outfile, &newscore, 4, 1);
    }

    // Version 7+ include the allied mode information
    temp_allied = allied_mode;
    SDL_RWwrite(outfile, &temp_allied, 2, 1);

    // Determine size of team list...
    listsize = team_size;

    // gotoxy(1, 22);
    // Log("Team size: %d", listsize);
    SDL_RWwrite(outfile, &listsize, 2, 1);

    SDL_RWwrite(outfile, &numplayers, 1, 1);

    // Write the reserved area, 31 bytes
    SDL_RWwrite(outfile, filler, 31, 1);

    // Okay, we've written header...now dump the data...
    for (Sint32 i = 0; i < team_size; ++i) {
        Guy *temp_guy = team_list[i];

        // Get temp values to be saved
        temp_order = ORDER_LIVING;
        temp_family = temp_guy->family;
        // Write name of current guy...
        guyname = temp_guy->name;
        // Set any chars under 12 not used to 0...
        guyname.resize(12);

        temp_str = temp_guy->strength;
        temp_dex = temp_guy->dexterity;
        temp_con = temp_guy->constitution;
        temp_short = temp_guy->intelligence;
        temp_arm = temp_guy->armor;
        temp_lev = temp_guy->get_level();
        temp_exp = temp_guy->exp;
        // Version 3+ below here
        temp_kills = temp_guy->kills;
        temp_level_kills = temp_guy->level_kills;
        // Version 4+ below here
        temp_td = temp_guy->total_damage;
        temp_td = temp_guy->total_hits;
        temp_ts = temp_guy->total_shots;

        // Version 5+ below here
        temp_teamnum = temp_guy->teamnum;

        // Now write all those values
        SDL_RWwrite(outfile, &temp_order, 1, 1);
        SDL_RWwrite(outfile, &temp_family, 1, 1);
        SDL_RWwrite(outfile, guyname.c_str(), 12, 1);
        SDL_RWwrite(outfile, &temp_str, 2, 1);
        SDL_RWwrite(outfile, &temp_dex, 2, 1);
        SDL_RWwrite(outfile, &temp_con, 2, 1);
        SDL_RWwrite(outfile, &temp_short, 2, 1);
        SDL_RWwrite(outfile, &temp_arm, 2, 1);
        SDL_RWwrite(outfile, &temp_lev, 2, 1);
        SDL_RWwrite(outfile, &temp_exp, 4, 1);
        SDL_RWwrite(outfile, &temp_kills, 2, 1);
        SDL_RWwrite(outfile, &temp_level_kills, 4, 1);
        SDL_RWwrite(outfile, &temp_td, 4, 1);
        SDL_RWwrite(outfile, &temp_th, 4, 1);
        SDL_RWwrite(outfile, &temp_ts, 4, 1);
        SDL_RWwrite(outfile, &temp_teamnum, 2, 1);
        // And the filler
        SDL_RWwrite(outfile, filler, 8, 1);
    }

    // Write the completed levels
    // Make sure our current level is saved
    std::map<std::filesystem::path, Sint32>::iterator cur = current_levels.find(current_campaign);

    if (cur != current_levels.end()) {
        cur->second = scen_num;
    } else {
        current_levels.insert(std::make_pair(current_campaign, scen_num));
    }

    // Number of campaigns
    Sint16 num_campaigns = completed_levels.size();
    SDL_RWwrite(outfile, &num_campaigns, 2, 1);

    for (auto const & e : completed_levels) {
        // Campaign ID
        char campaign[41];
        memset(campaign, 0, 41);
        strcpy(campaign, e.first.c_str());
        SDL_RWwrite(outfile, campaign, 1, 40);

        Sint16 index = 1;
        std::map<std::filesystem::path, Sint32>::const_iterator g = current_levels.find(e.first);

        if (g != current_levels.end()) {
            index = g->second;
        }

        SDL_RWwrite(outfile, &index, 2, 1);

        // Number of levels
        Sint16 num_levels = e.second.size();
        SDL_RWwrite(outfile, &num_levels, 2, 1);

        for (auto const & index : e.second) {
            // Level index
            SDL_RWwrite(outfile, &index, 2, 1);
        }
    }

    SDL_RWclose(outfile);

    return true;
}

bool SaveData::is_level_completed(Sint32 level_index) const
{
    std::map<std::filesystem::path, std::set<Sint32>>::const_iterator e = completed_levels.find(current_campaign);

    // Campaign not found? Then this level is not done.
    if (e == completed_levels.end()) {
        return false;
    }

    // If the level is listed, then it is completed
    std::set<Sint32>::const_iterator f = e->second.find(level_index);

    return (f != e->second.end());
}

Sint32 SaveData::get_num_levels_completed(std::filesystem::path const &campaign) const
{
    std::map<std::filesystem::path, std::set<Sint32>>::const_iterator e = completed_levels.find(campaign);

    // Campaign not found?
    if (e == completed_levels.end()) {
        return 0;
    }

    return e->second.size();
}

void SaveData::add_level_completed(std::filesystem::path const &campaign, Sint32 level_index)
{
    std::map<std::filesystem::path, std::set<Sint32>>::iterator e = completed_levels.find(campaign);

    // Campaign not found? Add it in.
    if (e == completed_levels.end()) {
        e = completed_levels.insert(std::make_pair(campaign, std::set<Sint32>())).first;
    }

    // Add the completed level
    e->second.insert(level_index);
}

void SaveData::reset_campaign(std::filesystem::path const &campaign)
{
    std::map<std::filesystem::path, std::set<Sint32>>::iterator e = completed_levels.find(campaign);

    if (e != completed_levels.end()) {
        e->second.clear();
    }
}
