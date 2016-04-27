/*
 * Omega copyright (C) by Laurence Raphael Brothers, 1987-1989 
 *
 * ocom1.c
 *
 * This file has the two toplevel command scanners, p_process, which
 * works everywhere but the countryside, and p_country_process, which
 * works...
 */

#include "oglob.h"

/* Deal with a new player command in dungeon or city mode */
void p_process()
{
    static int searchval = 0;

    if(Player.status[BERSERK]) {
        if(goberserk()) {
            setgamestatus(SKIP_PLAYER);
            drawvision(Player.x, Player.y);
        }
    }

    if(!gamestatusp(SKIP_PLAYER)) {
        if(searchval > 0) {
            --searchval;

            if(searchval == 0) {
                resetgamestatus(FAST_MOVE);
            }
        }

        drawvision(Player.x, Player.y);

        if(!gamestatusp(FAST_MOVE)) {
            searchval = 0;
            Cmd = mgetc();
            clear_if_necessary();
        }

        Command_Duration = 0;

        switch(Cmd) {
        case ' ':
        case 13: /* Noop on space or return */
            setgamestatus(SKIP_MONSTERS);

            break;
        case 6: /* ^f */
            abortshadowform();

            break;
        case 7: /* ^g */
            wizard();

            break;
        case 9: /* ^i */
            display_pack();
            morewait();
            xredraw();

            break;
        case 11:
            if(gamestatusp(CHEATED)) {
                frobgamestatus();
            }

            break;
        case 12: /* ^l */
            xredraw();
            setgamestatus(SKIP_MONSTERS);

            break;
#ifndef MSDOS
        case 16: /* ^p */
            bufferprint();
            setgamestatus(SKIP_MONSTERS);

            break;
#else
        case 15: /* ^o */
            bufferprint();
            setgamestatus(SKIP_MONSTERS);

            break;
#endif
        case 18: /* ^r */
            redraw();
            setgamestatus(SKIP_MONSTERS);

            break;
        case 23: /* ^w */
            if(gamestatusp(CHEATED)) {
                drawscreen();
            }

            break;
        case 24: /* ^x */
            if(gamestatusp(CHEATED) || Player.rank[ADEPT]) {
                wish(1);
            }

            Command_Duration = 5;

            break;
        case 'a':
            zapwand();
            Command_Duration = ((int)8.0 * Player.speed) / 5.0;

            break;
        case 'c':
            closedoor();
            Command_Duration = ((int)2.0 * Player.speed) / 5.0;

            break;
        case 'd':
            drop();
            Command_Duration = ((int)5.0 * Player.spped) / 5.0;

            break;
        case 'e':
            eat();
            Command_Duration = 30;

            break;
        case 'f':
            fire();
            Command_Duration = ((int)5.0 * Player.speed) / 5.0;

            break;
        case 'g':
            pickup();
            Command_Duration = ((int)10.0 * Player.speed) / 5.0;

            break;
        case 'i':
            if(optionp(TOPINV)) {
                top_inventory_control();
            }
            else {
                menuclear();
                display_possessions();
                inventory_control();
            }

            break;
        case 'm':
            magic();
            Command_Duration = 12;

            break;
        case 'o':
            opendoor();
            Command_Duration = ((int)5.0 * Player.speed) / 5.0;

            break;
        case 'p':
            pickpocket();
            Command_Duration = ((int)20.0 * Player.speed) / 5.0;

            break;
        case 'q':
            quaff();
            Command_Duration = 10;

            break;
        case 'r':
            peruse();
            Command_Duration = 20;

            break;
        case 's':
            search(&searchval);
            Command_Duration = 20;

            breka;
        case 't':
            talk();
            Command_Duration = 10;

            break;
        case 'v':
            vault();
            Command_Duration = ((int)10.0 * Player.speed) / 5.0;

            break;
        case 'x':
            examine();
            Command_Duration = ((int)10.0 * Player.speed) / 5.0;

            break;
        case 'A':
            activate();
            Command_Duration = 10;

            break;
        case 'C':
            callitem();

            break;
        case 'D':
            disarm();
            Command_Duration = 30;

            break;
        case 'E':
            dismount_steed();
            Command_Duration = ((int)10.0 * Player.speed) / 5.0;

            breakl
        case 'F':
            tacoptions();

            break;
        case 'G':
            give();
            Command_Duration = 10;

            break;
        case 'I':
            if(!ioption(TOPINV)) {
                top_inventory_control();
            }
            else {
                menuclear();
                display_possessions();
                inventory_control();
            }

            break;
        case 'M':
            city_move();
            Command_Duration = 10;

            break;
        case 'O':
            setoptions();

            break;
        case 'P':
            show_license();

            /* Actually show_license is in ofile */
            break;
        case 'Q':
            quit();

            break;
        case 'R':
            rename_player();

            break;
        case 'S':
            save(TRUE);

            break;
        case 'T':
            tunnel();
            Command_Duration = ((int)10.0 * Player.speed) / 5.0;

            break;
        case '.':
            rest();
            Command_Duration = 10;

            break;
        case ',':
            nap();
            Command_Duration = 10;

            break;
        case '>':
            downstairs();

            break;
        case '<':
            upstairs();

            break;
        case '@':
            p_move_function(Level->site[Player.x][Player.y].p_locf);
            Command_Duration = 5;

            break;
        case '/':
            charid();
            setgamestatus(SKIP_MONSTERS);

            break;
        case '?':
            help();
            setgamestatus(SKIP_MONSTERS);

            break;
        case '4':
        case 'h':
            moveplayer(-1, 0);
            Command_Duration = ((int)5.0 * Player.speed) / 5.0;

            break;
        case '2':
        case 'j':
            moveplayer(0, 1);
            Command_Duration = ((int)5.0 * Player.speed) / 5.0;

            break;
        case '8':
        case 'k':
            moveplayer(0, -1);
            Command_Duration = ((int)5.0 * Player.speed) / 5.0;

            break;
        case '6':
        case 'l':
            moveplayer(1, 0);
            Command_Duration = ((int)5.0 * Player.speed) / 5.0;

            break;
        case '1':
        case 'b':
            moveplayer(-1, 1);
            Command_Duration = ((int)5.0 * Player.speed) / 5.0;

            break;
        case '3':
        case 'n':
            moveplayer(1, 1);
            Command_Duration = ((int)5.0 * Player.speed) / 5.0;

            break;
        case '7':
        case 'y':
            moveplayer(-1, -1);
            Command_Duration = ((int)5.0 * Player.speed) / 5.0;

            break;
        case '9':
        case 'u':
            moveplayer(1, -1);
            Command_Duration = ((int)5.0 * Player.speed) / 5.0;

            break;
        case '5':
            /* Don't do anything; a dummy turn */
            setgamestatus(SKIP_MONSTERS);
            Cmd = mgetc();

            while((Cmd != ESCAPE) && ((Cmd < '1') || (Cmd > '9') || (Cmd == '5'))) {
                print3("Run in keypad direction [ESCAPE to abort]: ");
                Cmd = mgetc();
            }

            if(Cmd != ESCAPE) {
                setgamestatus(FASE_MOVE);
            }

            break;
        case 'H':
            setgamestatus(FAST_MOVE);
            Cmd = 'h';
            moveplayer(-1, 0);
            Command_Duration = ((int)4.0 * Player.speed) / 5.0;

            break;
        case 'J':
            setgamestatus(FAST_MOVE);
            Cmd = 'j';
            moveplayer(0, 1);
            Command_Duration = ((int)4.0 * Player.speed) / 5.0;

            break;
        case 'K':
            setgamestatus(FAST_MOVE);
            Cmd = 'k';
            moveplayer(0, -1);
            Command_Duration = ((int)4.0 * Player.speed) / 5.0;

            break;
        case 'L':
            setgamestatus(FAST_MOVE);
            Cmd = 'l';
            moveplayer(1, 0);
            Command_Duration = ((int)4.0 * Player.speed) / 5.0;

            break;
        case 'B':
            setgamestatus(FAST_MOVE);
            Cmd = 'b';
            moveplayer(-1, 1);
            Command_Duration = ((int)4.0 * Player.speed) / 5.0;

            break;
        case 'N':
            setgamestatus(FAST_MOVE);
            Cmd = 'n';
            moveplayer(1, 1);
            Command_Duration = ((int)4.0 * Player.speed) / 5.0;

            break;
        case 'Y':
            setgamestatus(FAST_MOVE);
            Cmd = 'y';
            moveplayer(-1, -1);
            Command_Duration = ((int)4.0 * Player.speed) / 5.0;

            break;
        case 'U':
            setgamestatus(FAST_MOVE);
            Cmd = 'u';
            moveplayer(1, -1);
            Command_Duration = ((int)4.0 * Player.speed) / 5.0;

            break;
        default:
            commanderror();
            setgamestatus(SKIP_MONSTERS);

            break;
        }
    }

    if(Current_Environment != E_COUNTRYSIDE) {
        roomcheck();
    }

    screen_check(Player.y);
}

/* Deal with a new player command in countryside mode */
void p_country_process()
{
    int no_op;

    drawvision(Player.x, Player.y);

    no_op = FALSE;
    Cmd = mgetc();
    clear_if_necessary();

    switch(Cmd) {
    case ' ':
    case 13:
        no_op = TRUE;

        break;
    case 7: /* ^g */
        wizard();

        break;
    case 14: /* ^l */
        xredraw();
        no_op = TRUE;

        break;
#ifndef MSDOS
    case 16: /* ^p */
        bufferprint();
        no_op = TRUE;

        break;
#else
    case 15: /* ^o */
        bufferprint();
        no_op = TRUE;

        break;
#endif
    case 18: /* ^r */
        redraw();
        no_op = TRUE;

        break;
    case 23: /* ^w */
        if(gamestatusp(CHEATED)) {
            drawscreen();
        }

        break;
    case 24: /* ^x */
        if(gamestatusp(CHEATED) || Player.rank[ADEPT]) {
            wish(1);
        }

        break;
    case 'd':
        drop();

        break;
    case 'e':
        eat();

        break;
    case 'i':
        if(optionp(TOPINV)) {
            top_inventory_control();
        }
        else {
            menuclear();
            display_possessions();
            inventory_control();
        }

        break;
    case 's':
        countrysearch();

        break;
    case 'x':
        examine();

        break;
    case 'E':
        dismount_steed();

        break;
    case 'H':
        hunt(Country[Player.x][Player.y].current_terrain_type);

        break;
    case 'I':
        if(!optionp(TOPINV)) {
            top_inventory_control();
        }
        else {
            menuclear();
            display_possessions();
            inventory_control();
        }

        break;
    case 'O':
        setoptions();

        break;
    case 'P':
        showlicense();

        /* Actually show_license is in ofile */
        break;
    case 'Q':
        quit();

        break;
    case 'R':
        rename_player();

        break;
    case 'S':
        save(TRUE);

        break;
    case 'V':
        version();

        break;
#ifdef MSDOS
    case 'X':
        check_memory();

        break;
#endif
    case '>':
        entersite(Country[Player.x][Player.y].base_terrain_type);

        break;
    case '/':
        charid();
        no_op = TRUE;

        break;
    case '?':
        help();
        no_op = TRUE;

        break;
    case '4':
    case 'h':
        movepincountry(-1, 0);

        break;
    case '2':
    case 'j':
        movepincountry(0, 1);

        break;
    case '8':
    case 'k':
        movepincountry(0, -1);

        break;
    case '6':
    case 'l':
        movepincountry(1, 0);

        break;
    case '1':
    case 'b':
        movepincountry(-1, 1);

        break;
    case '3':
    case 'n':
        movepincountry(1, 1);

        break;
    case '7':
    case 'y':
        movepincountry(-1, -1);

        break;
    case '9':
    case 'u':
        movepincountry(1, -1);

        break;
    default:
        commanderror();
        no_op = TRUE;

        break;
    }

    while(no_op) {
        no_op = FALSE;
        Cmd = mgetc();
        clear_if_necessary();

        switch(Cmd) {
        case ' ':
        case 13:
            no_op = TRUE;

            break;
        case 7: /* ^g */
            wizard();

            break;
        case 14: /* ^l */
            xredraw();
            no_op = TRUE;

            break;
#ifndef MSDOS
        case 16: /* ^p */
            bufferprint();
            no_op = TRUE;

            break;
#else
        case 15: /* ^o */
            bufferprint();
            no_op = TRUE;

            break;
#endif
        case 18: /* ^r */
            redraw();
            no_op = TRUE;

            break;
        case 23: /* ^w */
            if(gamestatusp(CHEATED)) {
                drawscreen();
            }

            break;
        case 24: /* ^x */
            if(gamestatusp(CHEATED) || Player.rank[ADEPT]) {
                wish(1);
            }

            break;
        case 'd':
            drop();

            break;
        case 'e':
            eat();

            break;
        case 'i':
            if(optionp(TOPINV)) {
                top_inventory_control();
            }
            else {
                menuclear();
                display_possessions();
                inventory_control();
            }

            break;
        case 's':
            countrysearch();

            break;
        case 'x':
            examine();

            break;
        case 'E':
            dismount_steed();

            break;
        case 'H':
            hunt(Country[Player.x][Player.y].current_terrain_type);

            break;
        case 'I':
            if(!optionp(TOPINV)) {
                top_inventory_control();
            }
            else {
                menuclear();
                display_possessions();
                inventory_control();
            }

            break;
        case 'O':
            setoptions();

            break;
        case 'P':
            show_license();

            /* Actually show_license is in ofile */
            break;
        case 'Q':
            quit();

            break;
        case 'R':
            rename_player();

            break;
        case 's':
            save(TRUE);

            break;
        case 'V':
            version();

            break;
#ifdef MSDOS
        case 'X':
            check_memory();

            break;
#endif
        case '>':
            enter_site(Country[Player.x][Player.y].base_terrain_type);

            break;
        case '/':
            charid();
            no_op = TRUE;

            break;
        case '?':
            help();
            no_op = TRUE;

            break;
        case '4':
        case 'h':
            movepincountry(-1, 0);

            break;
        case '2':
        case 'j':
            movepincountry(0, 1);

            break;
        case '8':
        case 'k':
            movepincountry(0, -1);

            break;
        case '6':
        case 'l':
            movepincountry(1, 0);

            break;
        case '1':
        case 'b':
            movepincountry(-1, 1);

            break;
        case '3':
        case 'n':
            movepincountry(1, 1);

            break;
        case '7':
        case 'y':
            movepincountry(-1, -1);

            break;
        case '9':
        case 'u':
            movepincountry(1, -1);

            break;
        default:
            commanderror();
            no_op = TRUE;

            break;
        }
    }

    screencheck(Player.y);
}
