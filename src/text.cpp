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
#include "text.hpp"

#include "base.hpp"
#include "graphlib.hpp"
#include "input.hpp"
#include "video_screen.hpp"
#include "view.hpp"

#include <cctype>

static PixieData letters1;
static PixieData letters_big;

Text::Text(std::string const &filename)
    : sizex(0)
    , sizey(0)
{
    if (!letters1.valid()) {
        letters1 = read_pixie_file(TEXT_1);
    }

    if (!letters_big.valid()) {
        letters_big = read_pixie_file(TEXT_BIG);
    }

    std::string temp_filename(filename);

    if (temp_filename.empty() || (temp_filename.size() < 2)) {
        temp_filename = "text.pix";
    }

    if (temp_filename == TEXT_BIG) {
        letters = letters_big;
    } else {
        letters = letters1;
    }

    sizex = letters.w;
    sizey = letters.h;
}

Text::~Text()
{
    // TODO: Free letters somewhere better (it's a global leak for now, so no hurry)
    // Letters is offset by 3 bytes on load
    // free(letters - 3);
    // letters = nullptr;
}

// Returns width, in pixels
Sint16 Text::query_width(std::string const &string)
{
    Uint16 i = 0;
    Sint16 over = 0;

    // Small, monospaced font
    if (sizex < 9) {
        return ((sizex + 1) * string.size());
    }

    while (string[i]) {
        // Uppercase
        if (isupper(string[i])) {
            over += sizex;
        } else {
            over += sizex - 1;
        }

        ++i;
    }

    return over;
}

bool Text::write_xy(Sint16 x, Sint16 y, Uint8 color, std::string const &string)
{
    Uint8 i = 0;

    while (string[i]) {
        write_char_xy(static_cast<Sint16>(x + (i * (sizex + 1))), string[i], static_cast<Uint8>(color));
        ++i;
    }

    return true;
}

Sint16 Text::write_xy(Sint16 x, Sint16 y, Uint8 color, std::stringstream const &stream)
{
    std::string text_buffer(stream.str());
    text_buffer.resize(255);

    Uint8 i = 0;

    if (!text_buffer.empty()) {
        while (text_buffer[i]) {
            write_char_xy(static_cast<Sint16>(x + (i * (sizex + 1))), text_buffer[i], static_cast<Uint8>(color));
            ++i;
        }
    }

    return (i * (sizex + 1));
}

Sint16 Text::write_xy_shadow(Sint16 x, Sint16 y, Uint8 color, std::stringstream const &stream)
{
    std::string text_buffer(stream.str());
    text_buffer.resize(255);

    Uint8 i = 0;

    if (!text_buffer.empty()) {
        while (text_buffer[i]) {
            Sint16 xx = x + (i * (sizex + 1));
            write_char_xy(xx - 1, y + 1, text_buffer[i], static_cast<Uint8>(PURE_BLACK + 2));
            write_char_xy(xx, y, text_buffer[i], static_cast<Uint8>(color));
            ++i;
        }
    }

    return (i * (sizex + 1));
}

bool Text::write_xy_center(Sint16 x, Sint16 y, Uint8 color, std::stringstream const &stream)
{
    std::string text_buffer(stream.str());
    text_buffer.resize(255);

    Uint8 i = 0;
    size_t len = text_buffer.length();

    if (!text_buffer.empty()) {
        while (text_buffer[i]) {
            write_char_xy(static_cast<Sint16>((x + (i * (sizex + 1))) - ((len * (sizex + 1)) / 2)), y, text_buffer[i], static_cast<Uint8>(color));
            ++i;
        }
    }

    return true;
}

bool Text::write_xy_center_alpha(Sint16 x, Sint16 y, Uint8 color, Uint8 alpha, std::stringstream const &stream)
{
    std::string text_buffer(stream.str());
    text_buffer.resize(255);

    Uint8 i = 0;
    size_t len = text_buffer.length();

    if (!text_buffer.empty()) {
        while (text_buffer[i]) {
            write_char_xy_alpha(static_cast<Sint16>((x + (i * (sizex + 1))) - ((len * (sizex + 1)) / 2)), y, text_buffer[i], static_cast<Uint8>(color), alpha);
            ++i;
        }
    }

    return true;
}

bool Text::write_xy_center_shadow(Sint16 x, Sint16 y, Uint8 color, std::stringstream const &stream)
{
    std::string text_buffer(stream.str());
    text_buffer.resize(255);

    Uint8 i = 0;
    size_t len = text_buffer.length();

    if (!text_buffer.empty()) {
        while (text_buffer[i]) {
            Sint16 xx = (x + (i * (sizex + 1))) - ((len * (sizex + 1)) / 2);
            write_char_xy(xx - 1, xx - 1, y + 1, text_buffer[i], static_cast<Uint8>(PURE_BLACK + 2));
            write_char_xy(xx, y, text_buffer[i], static_cast<Uint8>(color));
            ++i;
        }
    }

    return true;
}

bool Text::write_xy(Sint16 x, Sint16 y, std::string const &string)
{
    Uint16 i = 0;

    while (string[i]) {
        write_char_xy(static_cast<Sint16>(x + (i * (sizex + 1))), y, string[i], static_cast<Uint8>(DEFAULT_TEXT_COLOR));
        ++i;
    }

    return true;
}

Sint16 Text::write_xy(Sint16 x, Sint16 y, std::string const &string, Uint8 color, Sint16 to_buffer)
{
    Uint16 i = 0;
    Uint16 width;
    Sint16 over = 0;

    // Small, monospaced font
    if (sizex < 9) {
        while (string[i]) {
            if (!to_buffer) {
                write_char_xy(static_cast<Sint16>(x + (i * (sizex + 1))), y, string[i], static_cast<Uint8>(color));
            } else {
                write_char_xy(static_cast<Sint16>(x + (i * (sizex + 1))), y, string[i], static_cast<Uint8>(color), static_cast<Sint16>(1));
            }

            ++i;
            over += (sizex + 1);
        }
    } else {
        // Larger font, help out the lowercase...
        while (string[i]) {
            write_char_xy(static_cast<Sint16>(x + over), string[i], static_cast<Uint8>(color), static_cast<Sint16>(1));

            // Uppercase
            if (std::isupper(string[i])) {
                over += sizex;
            } else {
                // Lowercase, other things
                over += (sizex - 1);
            }

            ++i;
        }
    }

    if (to_buffer) {
        width = static_cast<Uint16>((sizex + 1) * string.length());
        width -= (width % 4);
        width += 4;
        // myscreen->buffer_to_screen(x, y, width, sizey);
    }

    return over;
}

bool Text::write_xy(Sint16 x, Sint16 y, std::string const &string, Sint16 to_buffer)
{
    Sint16 i = 0;
    Sint16 width;

    while (string[i]) {
        if (!to_buffer) {
            write_char_xy(static_cast<Sint16>(x + (i * (sizex + 1))), y, string[i], static_cast<Uint8>(DEFAULT_TEXT_COLOR));
        } else {
            write_char_xy(static_cast<Sint16>(x + (i * (sizex + 1))), y, string[i], static_cast<Uint8>(DEFAULT_TEXT_COLOR), static_cast<Sint16>(1));
        }

        ++i;
    }

    if (to_buffer) {
        width = static_cast<Uint16>((sizex + 1) * string.length());
        width -= (width % 4);
        width += 4;
        // myscreen->buffer_to_screen(x, y, width, sizey);
    }

    return true;
}

bool Text::write_xy(Sint16 x, Sint16 y, std::string const &string, Uint8 color,
                    Sint16 xloc, Sint16 yloc, Sint16 endx, Sint16 endy)
{
    Uint16 i = 0;

    while (string[i]) {
        Sint16 xpos = x + (i * sizex + 1);

        myscreen->walkputbuffertext(xpos + xloc, y + yloc,
                                    sizex, sizey,
                                    xloc, yloc,
                                    endx, endy,
                                    &letters.data[(string[i] * sizex) * sizey],
                                    color);

        ++i;
    }

    return true;
}

Sint16 Text::write_y(Sint16 y, std::string const &string, Uint8 color)
{
    Uint16 len = 0;
    Uint16 xstart;
    len = static_cast<Uint16>(string.length());
    xstart = static_cast<Uint16>((320 - (len * (sizex + 1))) / 2);

    return write_xy(xstart, y, string, static_cast<Uint8>(color));
}

Sint16 Text::write_y(Sint16 y, std::string const &string)
{
    return write_y(y, string, static_cast<Uint8>(DEFAULT_TEXT_COLOR));
}

Sint16 Text::write_y(Sint16 y, std::string const &string, Uint8 color, Sint16 to_buffer)
{
    Uint16 len = 0;
    Uint16 xstart;
    len = static_cast<Uint16>(string.length());
    xstart = static_cast<Uint16>((320 - (len * (sizex + 1))) / 2);

    if (to_buffer == 0) {
        return write_xy(xstart, y, string, static_cast<Uint8>(color));
    } else {
        return write_xy(xstart, y, string, static_cast<Uint8>(color), to_buffer);
    }
}

Sint16 Text::write_y(Sint16 y, std::string const &string, Sint16 to_buffer)
{
    return write_y(y, string, static_cast<Uint8>(DEFAULT_TEXT_COLOR), to_buffer);
}

Sint16 Text::write_y(Sint16 y, std::string const &string, Uint8 color, Sint16 xloc, Sint16 yloc, Sint16 endx, Sint16 endy)
{
    Uint16 len = 0;
    Uint16 xstart;
    len = static_cast<Uint16>(string.length());
    xstart = static_cast<Uint16>((320 - (len * (sizex + 1))) / 2);

    return write_xy(xstart, y, string, static_cast<Uint8>(color), xloc, yloc, endx, endy);
}

// This version writes to the buffer and then writes the
// buffer to the screen...(double buffered to eliminate flashing)
bool Text::write_char_xy(Sint16 x, Sint16 y, Uint8 letter, Uint8 color, Sint16 to_buffer)
{
    if (to_buffer == 0) {
        return write_char_xy(x, y, letter, static_cast<Uint8>(color));
    }

    myscreen->walkputbuffertext(x, y, sizex, sizey, 0, 0, 319, 199, &letters.data[(letter * sizex) * sizey], static_cast<Uint8>(color));
    // myscreen->buffer_to_screen(x, y, (sizex + 4) - (sizex % 4), (sizey + 4) - (sizey % 4));

    return true;
}

bool Text::write_char_xy(Sint16 x, Sint16 y, Uint8 letter, Sint16 to_buffer)
{
    return write_char_xy(x, y, letter, static_cast<Uint8>(DEFAULT_TEXT_COLOR), to_buffer);
}

bool Text::write_char_xy(Sint16 x, Sint16 y, Uint8 letter, Uint8 color)
{
    myscreen->putdatatext(x, y, sizex, sizey, &letters.data[(letter * sizex) * sizey], static_cast<Uint8>(color));

    return true;
}

bool Text::write_char_xy_alpha(Sint16 x, Sint16 y, Uint8 letter, Uint8 color, Uint8 alpha)
{
    myscreen->walkputbuffertext_alpha(x, y, sizex, sizey, 0, 0, 319, 199, &letters.data[(letter * sizex) * sizey], static_cast<Uint8>(color), alpha);

    return true;
}

bool Text::write_char_xy(Sint16 x, Sint16 y, Uint8 letter)
{
    myscreen->putdatatext(x, y, sizex, sizey, &letters.data[(letter * sizex) * sizey]);

    return true;
}

std::string Text::input_string(Sint16 x, Sint16 y, Sint16 maxlength, std::string const &begin)
{
    return input_string(x, y, maxlength, begin, DARK_BLUE, 13);
}

/*
 * input_string reads a string from the keyboard, displaying it at screen
 * position x,y. The maximum length of the string is maxlength, and any string
 * in 'begin' will automatically be entered at the start. Fore- and backcolor
 * are used for the text foreground and background color
 */
std::string Text::input_string(Sint16 x, Sint16 y, Sint16 maxlength, std::string const &begin, Uint8 forecolor, Uint8 backcolor)
{
    bool string_done = false;
    Sint16 strmax = 100;
    std::string editstr;
    std::stringstream editstring;
    std::stringstream firststring;

    SDL_Keycode tempchar;
    std::string temptext;
    // Hasn't typed yet
    bool has_typed = false;
    bool return_null = false;

    if (!begin.empty()) {
        editstring << begin;
    }

    // default case
    firststring << begin;
    myscreen->draw_box(x, y, (x + maxlength) * (sizex + 1), y + sizey, backcolor, 1, 1);

    if (!begin.empty()) {
        myscreen->draw_box(x, y, x + query_width(begin), (y + sizey) - 2, forecolor, 1, 1);
    }

    editstr = editstring.str();
    editstr.resize(strmax);
    write_xy(x, y, editstr, WHITE, 1);
    myscreen->buffer_to_screen(0, 0, 320, 200);

    clear_keyboard();
    clear_key_press_event();
    clear_text_input_event();

    SDL_StartTextInput();

    while (!string_done) {
        // Wait for a key to be pressed...
        while (!query_key_press_event() && !query_text_input_event()) {
            // ++dumbcount;
            get_input_events(WAIT);
        }

        if (query_key_press_event()) {
            tempchar = query_key();
            clear_key_press_event();

            if (tempchar == SDLK_RETURN) {
                string_done = true;
            } else if (tempchar == SDLK_ESCAPE) {
                editstring << firststring.str();
                string_done = true;
                return_null = true;
            } else if ((tempchar == SDLK_BACKSPACE) && (editstring.tellg() > 0)) {
                // First char, so replace text
                if (!has_typed) {
                    editstring.clear();

                    myscreen->draw_button(x, y, (x + maxlength) * (sizex + 1), (y + sizey) + 1, 1);
                } else {
                    editstring.unget();
                }

                has_typed = true;
            } else if ((tempchar == SDLK_LEFT) || (tempchar == SDLK_RIGHT)
                       || (tempchar == SDLK_UP) || (tempchar == SDLK_DOWN)
                       || (tempchar == SDLK_HOME) || (tempchar == SDLK_END)) {
                // Other keys which will deselect the whole line
                has_typed = true;
            }
        }

        if (query_text_input_event()) {
            temptext = query_text_input();

            // First char, so replace text
            if (!has_typed) {
                editstring.clear();

                myscreen->draw_button(x, y, (x + maxlength) * (sizex + 1), (y + sizey) + 1, 1);
            }

            editstr = editstring.str();
            if (!temptext.empty() && (static_cast<Sint16>(editstr.length() + temptext.length()) < maxlength)) {
                for (auto const & c : temptext) {
                    if (std::isprint(c)) {
                        editstring << c;
                    }
                }
            }

            clear_text_input_event();
            has_typed = true;
        }

        myscreen->draw_box(x, y, (x + maxlength) * (sizex + 1), (y + sizey) + 1, backcolor, 1, 1);

        if (!has_typed && (editstring.tellg() > 0)) {
            editstr = editstring.str();
            editstr.resize(strmax);
            myscreen->draw_box(x, y, x + query_width(editstr), (y + sizey) - 2, forecolor, 1, 1);
            write_xy(x, y, editstr, WHITE, 1);
        } else {
            write_xy(x, y, editstr, forecolor, 1);
        }

        myscreen->buffer_to_screen(0, 0, 320, 200);
    }

    SDL_StopTextInput();

    clear_keyboard();

    if (return_null) {
        return "";
    }

    editstr = editstring.str();
    editstr.resize(strmax);
    return editstr;
}

// This version passes DARK_BLUE and a grey color as defaults...
std::string Text::input_string_ex(Sint16 x, Sint16 y, Sint16 maxlength, std::string const &message, std::string const &begin)
{
    return input_string_ex(x, y, maxlength, message, begin, DARK_BLUE, 13);
}

std::string Text::input_string_ex(Sint16 x, Sint16 y, Sint16 maxlength, std::string const &message, std::string const &begin, Uint8 forecolor, Uint8 backcolor)
{
    bool string_done = false;
    Sint16 strmax = 100;
    std::string editstr;
    std::stringstream editstring;
    std::stringstream firststring;

    SDL_Keycode tempchar;
    std::string temptext;
    // Hasn't typed yet
    bool has_typed = false;
    bool return_null = false;

    if (!begin.empty()) {
        editstring << begin;
    }

    // default case
    firststring << begin;
    myscreen->draw_button(x, y, (x + maxlength) * (sizex + 1), y + sizey, 1);

    if (!begin.empty()) {
        myscreen->draw_box(x, y, x + query_width(begin), (y + sizey) - 2, forecolor, 1, 1);
    }
    write_xy(x, y - 10, message, DARK_GREEN, 1);
    editstr = editstring.str();
    editstr.resize(strmax);
    write_xy(x, y, editstr, WHITE, 1);
    myscreen->buffer_to_screen(0, 0, 320, 200);

    clear_keyboard();
    clear_key_press_event();
    clear_text_input_event();

    SDL_StartTextInput();

    while (!string_done) {
        // Wait for a key to be pressed...
        while (!query_key_press_event() && !query_text_input_event()) {
            get_input_events(WAIT);
        }

        if (query_key_press_event()) {
            tempchar = query_key();
            clear_key_press_event();

            if (tempchar == SDLK_RETURN) {
                string_done = true;
            } else if (tempchar == SDLK_ESCAPE) {
                editstring << firststring.str();
                string_done = true;
                return_null = true;
            } else if ((tempchar == SDLK_BACKSPACE) && (editstring.tellg() > 0)) {
                // First char, so replace text
                if (!has_typed) {
                    editstring.clear();

                    myscreen->draw_button(x, y, (x + maxlength) * (sizex + 1), (y + sizey) + 1, 1);
                } else {
                    editstring.unget();
                }

                has_typed = true;
            } else if ((tempchar == SDLK_LEFT) || (tempchar == SDLK_RIGHT)
                       || (tempchar == SDLK_UP) || (tempchar == SDLK_DOWN)
                       || (tempchar == SDLK_HOME) || (tempchar == SDLK_END)) {
                // Other keys which will deselect the whole line
                has_typed = true;
            }
        }

        if (query_text_input_event()) {
            temptext = query_text_input();

            // First char, so replace text
            if (!has_typed) {
                editstring.clear();

                myscreen->draw_button(x, y, (x + maxlength) * (sizex + 1), (y + sizey) + 1, 1);
            }

            editstr = editstring.str();
            if (!temptext.empty() && (static_cast<Sint16>(editstr.length() + temptext.length()) < maxlength)) {
                for (auto const & c : temptext) {
                    if (std::isprint(c)) {
                        editstring << c;
                    }
                }
            }

            clear_text_input_event();
            has_typed = true;
        }

        myscreen->draw_button(x, y, (x + maxlength) * (sizex + 1), (y + sizey) + 1, 1);
        write_xy(x, y - 10, message, DARK_GREEN, 1);

        if (!has_typed && (editstring.tellg() > 0)) {
            editstr = editstring.str();
            editstr.resize(strmax);
            myscreen->draw_box(x, y, x + query_width(editstr), (y + sizey) - 2, forecolor, 1, 1);
            write_xy(x, y, editstr, WHITE, 1);
        } else {
            write_xy(x, y, editstr, forecolor, 1);
        }

        myscreen->buffer_to_screen(0, 0, 320, 200);
    }

    SDL_StopTextInput();

    clear_keyboard();

    if (return_null) {
        return "";
    }

    editstr = editstring.str();
    editstr.resize(strmax);
    return editstr;
}
