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
#include "help.hpp"

#include <algorithm>
#include <fstream>
#include <string>

#include "input.hpp"
#include "level_data.hpp"
#include "util.hpp"

#define HELPTEXT_LEFT 40
#define HELPTEXT_TOP 40
#define DISPLAY_LINES 15

#define TEXT_DOWN(x) ((x * 7) + HELPTEXT_TOP)

Uint8 helptext[HELP_WIDTH][MAX_LINES];

/*
 * This function reads one text line from file infile, stopping at length
 * (length), or when encountering an end-of-line character...
 */
std::string read_one_line(std::ifstream &input, Sint16 length)
{
    char newline[length];

    input.getline(newline, length);

    if (!input.bad() && !input.eof()) {
        input.clear();
    }

    return std::string(newline);
}

// Note: This code has been redone to work in "scanlines", so that the
//       text scrolls by pixels rather than lines.
Sint16 read_scenario(VideoScreen *myscreen)
{
    Sint32 screenlines = myscreen->level_data.description.size() * 8;
    Sint32 numlines = screenlines;
    Sint32 linesdown = 0;
    bool changed = true;
    Sint32 templines;
    Sint32 text_delay = 1; // Bigger == slower
    Sint32 key_presses = 0;
    Text &mytext = myscreen->text_normal;
    Sint32 start_time = query_timer();
    Sint32 now_time;
    Sint32 bottomrow = screenlines - ((DISPLAY_LINES - 1) * 8);

    clear_keyboard();

    // Do the loop until person hits escape
    while (!query_input_continue()) {
        get_input_events(POLL);

        Sint16 scroll_amount = get_and_reset_scroll_amount();

        // Scrolling down
        if (scroll_amount < 0) {
            now_time = query_timer();

            key_presses = (now_time - start_time) % text_delay;

            if (!key_presses && (linesdown < bottomrow)) {
                while ((linesdown < bottomrow) && (scroll_amount != 0)) {
                    ++linesdown;
                    ++scroll_amount;
                }

                changed = true;
            }
        } // End of KEYSTATE_DOWN

        // Scrolling one page down
        if (keystates[KEYSTATE_PAGEDOWN]) {
            now_time = query_timer();
            key_presses = (now_time - start_time) % (10 * text_delay);

            if (!key_presses && (linesdown < bottomrow)) {
                templines = std::min(linesdown + (DISPLAY_LINES * 7), bottomrow);

                // We actually moved down
                if (linesdown != templines) {
                    linesdown = templines;
                    changed = true;
                }
            }
        } // End of PAGE DOWN

        // Scrolling up
        if (scroll_amount > 0) {
            now_time = query_timer();
            key_presses = (now_time - start_time) % text_delay;

            if (!key_presses && linesdown) {
                while (linesdown && (scroll_amount != 0)) {
                    --linesdown;
                    --scroll_amount;
                }

                changed = true;
            }
        } // End of KEYSTATE_UP

        // Scrolling one page up
        if (keystates[KEYSTATE_PAGEUP]) {
            now_time = query_timer();
            key_presses = (now_time - start_time) % (10 * text_delay);

            if (!key_presses && linesdown) {
                linesdown = std::max(linesdown - (DISPLAY_LINES * 7), 0);
                changed = true;
            }
        } // End of PAGE UP

        // Did we scroll, etc.?
        if (changed) {
            // Which TEXT line are we at?
            templines = linesdown / 8;
            myscreen->draw_button(HELPTEXT_LEFT - 4, HELPTEXT_TOP - 12,
                                  HELPTEXT_LEFT + 200, HELPTEXT_TOP + 107,
                                  3, 1);

            for (Sint16 j = 0; j < DISPLAY_LINES; ++j) {
                std::string s(myscreen->level_data.get_description_line(j + templines));

                if (!s.empty()) {
                    // To buffer!
                    mytext.write_xy(HELPTEXT_LEFT + 2,
                                    static_cast<Uint8>((TEXT_DOWN(j) - linesdown) % 8),
                                    s.c_str(), static_cast<Uint8>(DARK_BLUE), 1);
                }
            }

            // Draw a bounding box (top and bottom edges)...
            myscreen->draw_text_bar(HELPTEXT_LEFT, HELPTEXT_TOP - 8,
                                    HELPTEXT_LEFT + 196, HELPTEXT_TOP - 2);
            myscreen->draw_text_bar(HELPTEXT_LEFT, HELPTEXT_TOP + 97,
                                    HELPTEXT_LEFT + 196, HELPTEXT_TOP + 103);
            mytext.write_xy(HELPTEXT_LEFT + 30, HELPTEXT_TOP + 98,
                            "SCENARIO INFORMATION", static_cast<Uint8>(RED), 1);
            mytext.write_xy(HELPTEXT_LEFT + 30, HELPTEXT_TOP + 98,
                            CONTINUE_ACTION_STRING " TO CONTINUE",
                            static_cast<Uint8>(RED), 1);

            myscreen->buffer_to_screen(0, 0, 320, 200);
            changed = false;
        } // End of changed drawing loop
    } // Loop until ESC is pressed

    // Wait for key release
    while (keystates[KEYSTATE_ESCAPE]) {
        get_input_events(WAIT);
    }

    return static_cast<Sint16>(numlines);
}

Sint16 read_campaign_intro(VideoScreen *myscreen)
{
    CampaignData data(myscreen->save_data.current_campaign);

    if (!data.load()) {
        return 1;
    }

    Sint32 linesdown = 0;
    bool changed = true;
    Sint32 templines;
    Sint32 text_delay = 1; // Bigger == slower
    Sint32 key_presses = 0;
    Text &mytext = myscreen->text_normal;
    Sint32 start_time = query_timer();
    Sint32 now_time;

    clear_keyboard();

    // Fill in the helptext array with data...
    Sint32 screenlines = data.description.size() * 8;
    Sint32 numlines = screenlines;
    Sint32 bottomrow = screenlines- ((DISPLAY_LINES - 1) * 8);

    // Do the loop until person hits escape
    while (!query_input_continue()) {
        get_input_events(POLL);

        Sint16 scroll_amount = get_and_reset_scroll_amount();

        // Scrolling down
        if (scroll_amount < 0) {
            now_time = query_timer();

            key_presses = (now_time - start_time) % text_delay;

            if (!key_presses && (linesdown < bottomrow)) {
                while((linesdown < bottomrow) && (scroll_amount != 0)) {
                    ++linesdown;
                    ++scroll_amount;
                }

                changed = true;
            }
        } // End of KEYSTATE_DOWN

        // Scrolling one page down
        if (keystates[KEYSTATE_PAGEDOWN]) {
            now_time = query_timer();
            key_presses = (now_time - start_time) % (10 * text_delay);

            if (!key_presses && (linesdown < bottomrow)) {
                templines = std::min(linesdown + (DISPLAY_LINES * 7), bottomrow);

                // We actually moved down
                if (linesdown != templines) {
                    linesdown = templines;
                    changed = true;
                }
            }
        } // End of PAGE DOWN

        // Scrolling up
        if (scroll_amount > 0) {
            now_time = query_timer();
            key_presses = (now_time - start_time) % text_delay;

            if (!key_presses && linesdown) {
                while (linesdown && (scroll_amount != 0)) {
                    --linesdown;
                    --scroll_amount;
                }

                changed = true;
            }
        } // End of KEYSTATE_UP

        // Scrolling one page up
        if (keystates[KEYSTATE_PAGEUP]) {
            now_time = query_timer();
            key_presses = (now_time - start_time) % (10 * text_delay);

            if (!key_presses && linesdown) {
                std::max(linesdown - (DISPLAY_LINES * 7), 0);
                changed = true;
            }
        } // end of PAGE UP

        // Did we scroll, etc.?
        if (changed) {
            // Which TEXT line are we at?
            templines = linesdown / 8;

            myscreen->draw_button(HELPTEXT_LEFT - 4, HELPTEXT_TOP - 12,
                                  HELPTEXT_LEFT + 240, HELPTEXT_TOP + 107, 3, 1);

            for (Sint16 j = 0; j < DISPLAY_LINES; ++j) {
                if (!data.getDescriptionLine(j + templines).empty()) {
                    // To buffer!
                    mytext.write_xy(HELPTEXT_LEFT + 2,
                                    static_cast<Uint16>((TEXT_DOWN(j) - linesdown) % 8),
                                    data.getDescriptionLine(j + templines).c_str(),
                                    static_cast<Uint8>(DARK_BLUE), 1);
                }
            }

            // Draw a bounding box (top and bottom edges)...
            myscreen->draw_text_bar(HELPTEXT_LEFT, HELPTEXT_TOP - 8,
                                    HELPTEXT_LEFT + 236, HELPTEXT_TOP - 2);
            myscreen->draw_text_bar(HELPTEXT_LEFT, HELPTEXT_TOP + 97,
                                    HELPTEXT_LEFT + 236, HELPTEXT_TOP + 103);
            mytext.write_xy((HELPTEXT_LEFT + 120) - (data.title.size() * 3),
                            HELPTEXT_TOP - 7, data.title.c_str(),
                            static_cast<Uint8>(RED), 1);
            mytext.write_xy(HELPTEXT_LEFT + 52, HELPTEXT_TOP + 98,
                            CONTINUE_ACTION_STRING " TO CONTINUE",
                            static_cast<Uint8>(RED), 1);
            // myscreen->buffer_to_screen(0, 0, 320, 200);
            myscreen->buffer_to_screen(HELPTEXT_LEFT - 4, HELPTEXT_TOP - 12, 244, 119);

            changed = false;
        } // End of changed loop
    } // Loop until ESC is pressed

    // Wait for key release
    while (keystates[KEYSTATE_ESCAPE]) {
        get_input_events(WAIT);
    }

    // delete mytext;

    return static_cast<Sint16>(numlines);
}

/*
 * This function fills the array with the help file text...
 * It returns the number of lines successfully filled...
 */
Sint16 fill_help_array(std::string somearray[MAX_LINES], std::ifstream &input)
{
    Sint16 i = 0;

    std::string content(read_one_line(input, HELP_WIDTH));

    while ((i <= MAX_LINES) && input.good()) {
        somearray[i] = content;
        ++i;
        content = read_one_line(input, 10);
    }

    return i;
}
