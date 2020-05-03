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
// intro.cpp

/*
 * Changelog
 *     buffers: 08/09/02: * Added return 0 to finish func
 */
#include "intro.hpp"

#include "base.hpp"
#include "graphlib.hpp"
#include "input.hpp"
#include "pal32.hpp"
#include "pixie.hpp"
#include "util.hpp"
#include "video_screen.hpp"
#include "view.hpp"
// Z's script: #include <process.h>

#include <cstring>
#include <sstream>

#define SHOW_TIME 130
#define FROM 1
#define TO 0

Sint32 show();
Sint32 show(Sint32 howlong);
Sint32 cleanup();

SDL_Color mypalette[256];

void intro_main()
{
    // Zardus: PORT: Doesn't seem to be used, and causes a memory leak
    // Uint8 **args = static_cast<Uint8 **>(new int);
    Text &mytext = myscreen->text_normal;
    Pixie *gladiator;
    Pixie *bigfoot;
    // For full-screen displays
    Pixie *ul = nullptr;
    Pixie *ur = nullptr;
    Pixie *ll = nullptr;
    Pixie *lr = nullptr;
    PixieData uldata;
    PixieData urdata;
    PixieData lldata;
    PixieData lrdata;
    PixieData gladdata;
    PixieData bigdata;

    myscreen->viewob[0]->resize(PREF_VIEW_FULL);

    load_and_set_palette(mypalette);

    myscreen->fadeblack(TO);

    myscreen->clear();

    gladdata = read_pixie_file("3mages2.pix");
    gladiator = new Pixie(gladdata);
    gladiator->setxy(120, 55);
    gladiator->drawMix(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                       myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                       myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    mytext.write_y(100, "FORGOTTEN SAGES PRESENTS", 230,
                   myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                   myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    // myscreen->refresh();
    delete gladiator;
    gladdata.free();

    if (show() < 0) {
        cleanup();

        return;
    }

    // gladdata = read_pixie_file("glad.pix");
    gladdata = read_pixie_file("glad2.pix");
    gladiator = new Pixie(gladdata);
    bigdata = read_pixie_file("bigfoot.pix");
    bigfoot = new Pixie(bigdata);
    myscreen->clear();
    bigfoot->setxy(120, 50);
    bigfoot->drawMix(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                     myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                     myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    // gladiator->drawMix(110, 65, myscreen->viewob[0]);
    bigfoot->setxy(100, 110);
    bigfoot->draw(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
                  myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                  myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    // myscreen->refresh();

    gladdata.free();
    delete gladiator;
    delete bigfoot;
    bigdata.free();

    if (show() < 0) {
        cleanup();

        return;
    }

    myscreen->clear();
    mytext.write_y(70, "THOSE WHO ARE ABOUT TO DIE SALUTE YOU", 230,
                   myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                   myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    // myscreen->refresh();

    if (show() < 0) {
        cleanup();

        return;
    }

    // Programming credits, page 1
    myscreen->clear();
    mytext.write_y(80, "Programming By:", 230,
                   myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                   myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    mytext.write_y(100, "Chad Lawrence  Doug McCreary", 230,
                   myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                   myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    mytext.write_y(110, "Tom Ricket  Michael Scnadizzo", 230,
                   myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                   myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);


    // myscreen->refresh();

    if (show() < 0) {
        cleanup();

        return;
    }

    // First 'interlude' snapshot
    myscreen->clear();
    uldata = read_pixie_file("game2ul.pix");
    ul = new Pixie(uldata);
    ul->setxy(41, 12);
    ul->draw(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
             myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
             myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    delete ul;
    uldata.free();
    ul = nullptr;

    urdata = read_pixie_file("game2ur.pix");
    ur = new Pixie(urdata);
    ur->setxy(160, 12);
    ur->draw(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
             myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
             myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    delete ur;
    urdata.free();
    ur = nullptr;

    lldata = read_pixie_file("game2ll.pix");
    ll = new Pixie(lldata);
    ll->setxy(41, 103);
    ll->draw(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
             myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
             myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    delete ll;
    lldata.free();
    ll = nullptr;

    lrdata = read_pixie_file("game2lr.pix");
    lr = new Pixie(lrdata);
    lr->setxy(160, 103);
    lr->draw(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
             myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
             myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    delete lr;
    lrdata.free();
    lr = nullptr;

    // myscreen->refresh();

    if (show(SHOW_TIME + 30) < 0) {
        cleanup();

        return;
    }

    // Programming credits, page 2
    myscreen->clear();
    mytext.write_y(90, "Additional coding by Doug Ricket", 230,
                   myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                   myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);

    // buffers: PORT: w00t w00t
    mytext.write_y(110, "SDL port by Odo and Zardus", 230,
                   myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                   myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);

    // myscreen->refresh();

    if (show() < 0) {
        cleanup();

        return;
    }

    // Second 'interlude' & extra credits
    myscreen->clear();
    uldata = read_pixie_file("game4.pix");
    ul = new Pixie(uldata);
    ul->setxy(0, 0);
    ul->draw(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
             myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
             myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    delete ul;
    uldata.free();
    ul = nullptr;

    lldata = read_pixie_file("game5.pix");
    ll = new Pixie(lldata);
    ll->setxy(160, 78);
    ll->draw(myscreen->viewob[0]->topx, myscreen->viewob[0]->topy,
             myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
             myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);
    delete ll;
    lldata.free();
    ll = nullptr;

    std::stringstream message("Additional Artwork By:");
    mytext.write_xy(310 - mytext.query_width(message.str()), 30,
                    message.str(), 230,
                    myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                    myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);

    message.clear();
    message << "Doug Ricket";
    mytext.write_xy(310 - mytext.query_width(message.str()), 50,
                    message.str(), 230,
                    myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                    myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);

    message.clear();
    message << "Stefan Scandizzo";
    mytext.write_xy(310 - mytext.query_width(message.str()), 60,
                    message.str(), 230,
                    myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                    myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);

    message.clear();
    message << "Special Thanks to:";
    mytext.write_xy(2, 130, message.str(), 230,
                    myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                    myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);

    message.clear();
    message << "Kim Kelly  Lara Kirkendall";
    mytext.write_xy(2, 150, message.str(), 230,
                    myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                    myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);

    message.clear();
    message << "Lee Martin  Karyn McCreary";
    mytext.write_xy(2, 160, message.str(), 230,
                    myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                    myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);

    message.clear();
    message << "Loki, Ishara, & Mootz";
    mytext.write_xy(2, 170, message.str(), 230,
                    myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                    myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);

    message.clear();
    message << "and many others!";
    mytext.write_xy(2, 180, message.str(), 230,
                    myscreen->viewob[0]->xloc, myscreen->viewob[0]->yloc,
                    myscreen->viewob[0]->endx, myscreen->viewob[0]->endy);


    // myscreen->refresh();

    if (show(SHOW_TIME * 4) < 0) {
        cleanup();

        return;
    }

    cleanup();
}

Sint32 cleanup()
{
    myscreen->clear();
    myscreen->refresh();

    load_and_set_palette(mypalette);

    return 1;
}

Sint32 show()
{
    // Default uses SHOW_TIME
    return show(SHOW_TIME);
}

Sint32 show(Sint32 howlong)
{
    if (myscreen->fadeblack(FROM) == -1) {
        return -1;
    }

    reset_timer();

    while (query_timer() < howlong) {
        get_input_events(POLL);

        if (query_key_press_event()) {
            return -1;
        }
    }

    if (myscreen->fadeblack(TO) == -1) {
        return -1;
    }

    return 1;
}
