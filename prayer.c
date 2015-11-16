#include "pray.h"

#include "constants.h"
#include "config.h"
#include "types.h"
#include "externs.h"

/* Pray like HELL...    -RAK- */
void pray()
{
    int i;
    int j;
    int item_val;
    int dir;
    int choice;
    int chance;
    int result;
    int dumy;
    int y_dumy;
    int x_dumy;
    int redraw;
    spell_type *s_ptr;
    struct misc *m_ptr;
    struct flags *f_ptr;
    treasure_type *i_ptr;

    reset_flag = TRUE;

    if(py.flags.blind > 0) {
        msg_print("You can't see to read your prayer!");
    }
    else if(no_light()) {
        msg_print("You have no light to read by");
    }
    else if(py.flags.confused > 0) {
        msg_print("You are too confused...");
    }
    else if(class[py.misc.pclass].pspell) {
        if(inven_ctr > 0) {
            if(find_range(91, -1, &i, &j)) {
                redraw = FALSE;

                if(get_item(&item_val, "Use which Holy Book?", &redraw, i, j)) {
                    result = case_spell("Recite with prayer?", item_val, &choice, &chance, &redraw);

                    if(result == TRUE) {
                        s_ptr = &magic_spell[py.misc.pclass][choice];
                        reset_flag = FALSE;

                        if(randint(100) < chance) {
                            msg_print("You lost your concentration!");
                        }
                        else {
                            y_dumy = char_row;
                            x_dumy = char_col;

                            /* Prayers... */
                            ++choice;

                            switch(choice) {
                            case 1:
                                detect_evil();

                                break;
                            case 2:
                                hp_player(damroll("3d3"), "a prayer.");

                                break;
                            case 3:
                                bless(randint(12) + 12);

                                break;
                            case 4:
                                remove_fear();

                                break;
                            case 5:
                                light_area(char_row, char_col);

                                break;
                            case 6:
                                detect_trap();

                                break;
                            case 7:
                                detect_sdoor();

                                break;
                            case 8:
                                slow_poison();

                                break;
                            case 9:
                                if(get_dir("Which direction?", &dir, &dumy, &y_dumy, &x_dumy)) {
                                    confuse_monster(dir, char_row, char_col);
                                }

                                break;
                            case 10:
                                teleport((int)(py.misc.lev * 3));

                                break;
                            case 11:
                                hp_player(damroll("4d4", "a prayer."));

                                break;
                            case 12:
                                bless(randint(24) + 24);

                                break;
                            case 13:
                                sleep_monsters1(char_row, char_col);

                                break;
                            case 14:
                                create_food();

                                break;
                            case 15:
                                for(i = 0; i < INVEN_MAX; ++i) {
                                    i_ptr = &inventory[i];
                                    i_ptr->flags &= 0x7FFFFFFF;
                                }

                                break;
                            case 16:
                                f_ptr = &py.flags;
                                f_ptr->resist_heat += randint(10) + 10;
                                f_ptr->resist_cold += randint(10) + 10;

                                break;
                            case 17:
                                cure_poison();

                                break;
                            case 18:
                                if(get_dir("Which direction?", &dir, &dumy, &y_dumy, &x_dumy)) {
                                    fire_ball(6, dir, char_row, char_col, (int)(damroll("3d6") + py.misc.lev), "Black Sphere");
                                }

                                break;
                            case 19:
                                hp_player(damroll("8d4"), "a prayer.");

                                break;
                            case 20:
                                detect_inv2(randint(24) + 24);

                                break;
                            case 21:
                                protect_evil();

                                break;
                            case 22:
                                earthquake();

                                break;
                            case 23:
                                map_area();

                                break;
                            case 24:
                                hp_player(damroll("16d4"), "a prayer.");

                                break;
                            case 25:
                                turn_undead();

                                break;
                            case 26:
                                bless(randint(48) + 48);

                                break;
                            case 27:
                                dispell_creature(0x0008, (int)(3 * py.misc.level));

                                break;
                            case 28:
                                hp_player(200, "a prayer.");

                                break;
                            case 29:
                                dispell_creature(0x0004, (int)(3 * py.misc.lev));

                                break;
                            case 30:
                                warding_glyph();

                                break;
                            case 31:
                                dispell_creature(0x0004, (int)(4 * py.misc.lev));
                                cure_confusion();
                                remove_fear();
                                cure_poison();
                                cure_blindness();
                                hp_player(1000, "a prayer.");

                                break;
                            default:

                                break;
                            }

                            /* End or prayers... */
                            if(!reset_flag) {
                                m_ptr = &py.misc;
                                m_ptr->exp += s_ptr->sexp;
                                prt_experience();
                                s_ptr->sexp = 0;
                            }
                        }

                        m_ptr = &py.misc;

                        if(!reset_flag) {
                            if(s_ptr->smana > m_ptr->cmana) {
                                msg_print("You faint from fatigue!");
                                py.flags.paralysis = randint(5 * (int)(s_ptr->smana - (int)m_ptr->cmana));
                                m_ptr->cmana = 0.0;

                                if(randint(3) == 1) {
                                    msg_print("You have damaged your health!");
                                    py.stats.ccon = de_statp(py.stats.ccon);
                                    prt_constitution();
                                }
                            }
                            else {
                                m_ptr->cmana -= (double)s_ptr->smana;
                            }

                            prt_cmana();
                        }
                    }
                    else if(result == -1) {
                        msg_print("You don't know any prayers in that book.");
                    }
                }
                else {
                    if(redraw) {
                        draw_cave();
                    }
                }
            }
            else {
                msg_print("But you are not carrying any Holy Books!");
            }
        }
        else {
            msg_print("But you are not carrying any Holy Books!");
        }
    }
    else {
        msg_print("Pray hard enough and your prayers may be answered.");
    }
}
