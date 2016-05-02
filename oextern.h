/*
 * Omega copyright (c) 1987-1989 by Laurence Raphael Brothers
 *
 * oextern.h
 *
 * Function delcarations
 */

/* o.c functions */
void initrand();
void inititem();
void init_world();
int game_restore();
int signalexit();
int main();

/* oabyss functions */
void load_abyss();

/* oaux1 and oaux3 functions */
void pickup_at();
void setspot();
void foodcheck();
void p_death();
void hourly_check();
void calc_melee();
void gain_experience();
void p_damage();
void minute_check();
void p_process();
void p_teleport();
void tacplayer();
void roomcheck();
void tunnelcheck();
void searchat();
void showroom();
void weapon_use();
void damange_item();
void tacmode();
void hand_wield();
void optionset();
void optionreset();
void describe_player();
void lightroom();
void surrender();
void threaten();
void darkenroom();
void fight_monster();
void indoors_random_event();
void outdoors_random_event();
void tenminute_check();
char getlocation();
char *itemid();
char *mstatus_string();
char *trapid();
char *levelname();
int p_moveable();
int cursed();
int getdir();
int optionp();
int goberserk();
int player_on_sancuary();
int maneuvers();

/* oaux2 and oaux3 functions */
char *actionlocstr();
void break_weapon();
void moon_check();
void toggle_item_use();
void p_fumble();
void p_win();
void p_hit();
void player_miss();
void addring();
void torch_check();
void top_level();
void printactions();
void movecursor();
void p_drown();
void enter_site();
void tacexecute();
void weapon_use();
void minute_status_check();
void tenminute_status_check();
void expandsiteabbrev();
void gain_level();
void terrain_check();
void countrysearch();
void default_maneuvers();
void alert_guards();
int expval();
int player_hit();
int statmod();
int item_value();
int true_item_value();
int parsecitysite();
int expandsite();
int hostilemonstersnear();
int magic_resist();
char *countryid();
char *citysiteid();

/* ochar functions */
void initplayer();
void initstats();
void save_omegarc();
void omegan_character_stats();
void user_character_stats();
int calcmana();
int fixnpc();
int competence_check();
FILE *omegarc_check();

/* ocity function */
void load_city();
void randommazesite();
void assign_city_function();
void make_justiciar();
void ressurect_guards();
void mazesite();
void make_minor_undead();
void make_major_undead();
void repair_jail();

/* ocom1 functions */
void p_country_process();
void p_process();

/* ocom3 and ocom2 functions */
void pickup();
void drop();
void magic();
void disarm();
void save();
void restore();
void inventory();
void opendoor();
void closedoor();
void fire();
void fight();
void city_move();
void eat();
void version();
void nap();
void vault();
void bash_item();
void frobgamestatus();
void search();
void peruse();
void quaff();
void talk();
void bash_location();
void wizard();
void give();
void rest();
void examine();
void help();
void zapwand();
void floor_inv();
void callitem();
void charid();
void moveplayer();
void upstairs();
void downstairs();
void setoptions();
void activate();
void pickpocket();
void rename_player();
void abortshadowform();
void tunnel();
void change_environment();
void movepincountry();
void tacoptions();
void hunt();
void dismount_steed();
int quit();

/* oeffect1 functions */
void bless();
void identify();
void bolt();
void ball();
void amnesia();
void p_poison();
void heal();
void mondet();
void objdet();
void enchant();
void wish();
void manastorm();
void acquire();
void cleanse();
void annihilate();
void sleep_monster();
void apport();
void sleep_player();
void summon();
void hide();
void clairvoyance();
void aggravate();
void fbolt();
void lbolt();
void lball();
void fball();
void nbolt();
void learnspell();
void level_drain();
void disintegrate();
void disrupt();
void snowball();
void p_teleport();
void acid_cloud();
void strategic_teleport();
int random_item();
int itemlist();
int monsterlist();

/* oeffect2 functions */
void knowledge();
void flux();
void displace();
void breathe();
void invisible();
void warp();
void alert();
void haste();
void regenerate();
void recover_stat();
void augment();
void truesight();
void disease();
void i_chaos();
void i_law();
void sanctify();
void hero();
void level_return();
void cure();
void dispel();
void polymorph();
void hellfire();
void illuminate();
void drain_life();
void drain();
void inflict_fear();
void levitate();
void accuracy();
void sanctuary();
void shadowform();
void invulnerable();
void deflection();

/* oetc functions */
void hint();
void learnclericalspells();
void add_new_npc();
char *slotstr();
char *wordnum();
char *nameprint();

/* ofile functions */
void commandline();
void helpfile();
void showfile();
void showscores();
void showmotd();
void extendlog();
void filescanstring();
void checkhigh();
void user_intro();
void theologyfile();
void cityguidefile();
void combat_help();
void inv_help();
void abyss_file();
void wishfile();
void adeptfile();
void copyfile();
void displayfile();
int filecheck();
FILE *checkfopen();

/* ogen functions */
int loc_statusp();
int mazeok();
void lset();
void lreset();
void load_arena();
void make_country_screen();
void room_corridor();
void clear_level();
void create_level();
void build_square_room();
void build_diamond_room();
void connect_room();
void corridor_crawl();
void build_room();
void sewer_corridor();
void clear_dungeon();
void init_dungeon();
void install_traps();
void straggle_corridor();
void room_connect_corridor();
void sewer_level();
void room_level();
void make_prime();
void change_level();
void cavern_level();
void makedoor();
void make_archmage();
void maze_level();
void maze_corridor();
void install_specials();
void make_stairs();
void make_plains();
void make_road();
void make_forest();
void make_jungle();
void make_river();
void make_swamp();
void make_mountains();
void load_circle();
void load_court();
void find_stairs();
char *roomname();
plv findlevel();

/* outil functions */
int inbounds();
int random_range();
int hitp();
int sign();
int max();
int min();
int distance();
int unblocked();
int los_p();
int screenmod();
int offscreen();
int calc_points();
int bitp();
int nighttime();
int hour();
int showhour();
int day();
int showminute();
int view_unblocked();
int m_unblocked();
int strmem();
int view_lost_p();
int confirmation();
int gamestatusp();
char inversedir();
char *getarticle();
char *ordinal();
char *month();
char *salloc();
void initdirs();
void do_los();
void do_object_los();
void bitset();
void bitreset();
void findspace();
void setgamestatus();
void resetgamestatus();
void calc_weight();

/* otime functions */
void time_clock();

/* ospell functions */
void s_firebolt();
void s_lball();
void s_slee();
void s_objdet();
void s_mondet();
void s_identify();
void s_teleport();
void s_disrupt();
void s_disintegrate();
void s_missile();
void s_heal();
void s_dispel();
void s_breathe();
void s_invisible();
void s_warp();
void s_enchant();
void s_bless();
void s_truesight();
void s_hellfire();
void s_knowledge();
void s_hero();
void s_restore();
void s_cure();
void s_return();
void s_desecrate();
void s_summon();
void s_accuracy();
void s_ritual();
void s_apport();
void s_shadowform();
void s_alert();
void s_regenerate();
void s_sanctify();
void s_clairvoyance();
void s_drain();
void s_polymorph();
void s_invulnerable();
void cast_spell();
void s_fear();
void s_wish();
void initspells();
void expandspellabbrevs();
int getspell();
int exapndspell();
char *spellid();

/* osite functions */
void l_temple();
void l_armorer();
void l_club();
void l_gym();
void sign_print();
void l_castle();
void l_healer();
void statue_random();
void l_statu_wake();
void l_casino();
void l_bank();
void l_sewer();
void l_tavern();
void l_alchemist();
void l_vault();
void l_oracle();
void l_commandant();
void l_countryside();
void l_dpw();
void l_library();
void l_pawn_shop();
void l_diner();
void l_condo();
void l_cemetary();
void l_garden();
void l_house();
void l_hovel();
void l_mansion();
void l_jail();
void l_adept();
void l_trifid();
void l_warning();
void l_brothel();
void l_crap();
void l_safe();
void l_cartographer();
void l_charity();
void l_mindstone();
void healforpay();
void cureforpay();
void gymtrain();
void buyfromstock();
void wake_statue();
void send_to_jail();

/* oscr functions */
void redraw();
void drawspot();
void drawvision();
void morewait();
void drawmonsters();
void locprint();
void mprint();
void title();
void sitclear();
void initgraf();
void xredraw();
void erase_monster();
void plotchar();
void show_screen();
void blankoutspot();
void show_license();
void phaseprint();
void menuclear();
void showcursor();
void mnumprint();
void menunumprint();
void display_pack();
void clearmsg();
void menuprint();
void print3();
void nprint3();
void print1();
void print2();
void nprint1();
void clearmsg1();
void clearmsg3();
void commanderror();
void nprint2();
void deathprint();
void stdmorewait();
void draw_explosion();
void dataprint();
void comwinprint();
void timeprint();
void erase_level();
void levelrefresh();
void display_options();
void display_option_slot();
void refreshmsg();
void menuaddch();
void drawscreen();
void maddch();
void putspot();
void checkclear();
void display_quit();
void showflags();
void spreadroomdark();
void display_win();
void drawplayer();
void displaystats();
void display_death();
void plotmon();
void display_bigwin();
void dobackspace();
void drawandrefreshspot();
void spreadroomlight();
void bufferprint();
void drawomega();
void screencheck();
void blotspot();
void plotspot();
void dodrawspot();
void display_inventory_slot();
void endgraf();
void menuspellprint();
void hide_line();
void display_possessions();
void clear_if_necessary();
void buffercycle();
void lightspot();
int parsenum();
int getnumber();
int litroom();
int move_slot();
int stillonblock();
char getspot();
char mgetc();
char lgetc();
char menugetc();
char *msgscanstring();
char mcigetc();
char ynq();
char ynq1();
char ynq2();

/* omove functions */
void p_movefunction();
void l_water();
void l_hedge();
void l_lava();
void l_lift();
void l_magic_pool();
void l_whirlwind();
void l_abyss();
void l_no_op();
void l_tactical_exit();
void l_rubble();
void l_raise_portcullis();
void l_protcullis();
void l_portcullis_trap();
void l_arena_exit();
void l_house_exit();
void l_drop_every_portcullis();
void l_earth_station();
void l_water_station();
void l_fire_station();
void l_air_station();
void l_void();
void stationcheck();
void l_whirlwind();
void l_voice1();
void l_voice2();
void l_voice3();
void l_void_station();
void l_chaos();
void l_tome1();
void l_tome2();
void l_enter_circle();
void l_escalator();
void l_fire();
void l_voidstone();
void l_chaostone();
void l_lawstone();
void l_balancestone();
void l_sacrificestone();

/* otrap functions */
void l_trap_dart();
void l_trap_pit();
void l_trap_door();
void l_trap_snare();
void l_trap_siren();
void l_trap_blade();
void l_trap_fire();
void l_trap_disintegrate();
void l_trap_teleport();
void l_trap_acid();
void l_trap_manadrain();
void l_trap_sleepgas();
void l_trap_abyss();

/* ohouse functions */
void load_house();
void make_dog();
void make_automaton();
void make_house_npc();
void make_mansion_npc();

/* omonf functions */
void m_normal_move();
void m_no_op();
void m_smart_move();
void m_simple_move();
void m_spirit_move();
void m_vanish();
void m_firebolt();
void m_talk_guard();
void m_random_move();
void m_sp_morgon();
void m_flutter_move();
void m_nbolt();
void m_sp_surprise();
void m_talk_druid();
void m_talk_stupid();
void m_talk_hungry();
void m_talk_silent();
void m_talk_greedy();
void m_talk_mp();
void m_follow_move();
void m_sp_mp();
void m_talk_evil();
void m_talk_prime();
void m_lball();
void m_fireball();
void m_talk_titter();
void m_talk_thief();
void m_sp_ng();
void m_sp_poison_cloud();
void m_huge_sounds();
void m_talk_robot();
void m_blind_strike();
void m_talk_im();
void m_confused_move();
void m_talk_beg();
void m_theif_f();
void m_talk_man();
void m_summon();
void m_animal_talk();
void m_move_animal();
void m_teleport();
void m_illusion();
void m_aggravate();
void m_talk_hint();
void m_sp_explode();
void m_sp_acid_cloud();
void m_sp_bogthing();
void m_talk_animal();
void m_sp_whirl();
void m_sp_escape();
void m_sp_ghost();
void m_sp_demonlover();
void m_talk_mimsy();
void m_talk_slithy();
void m_talk_burble();
void m_sp_spell();
void m_scaredy_move();
void m_talk_gf();
void m_talk_ef();
void m_sp_demon();
void m_sp_av();
void m_sp_seductor();
void m_talk_demonlover();
void m_talk_seductor();
void m_sp_lw();
void m_sp_eater();
void m_sp_dragonlord();
void m_talk_ninja();
void m_talk_assassin();
void m_sp_blackout();
void m_sp_leash();
void m_sp_were();
void m_sp_angel();
void m_sp_servent();
void m_sp_swarm();
void m_talk_servent();
void m_talk_horse();
void m_snowball();
void m_sp_prime();
void m_sp_mb();
void m_sp_mirrow();
void m_sp_raise();
void m_talk_scream();
void m_strike_sonic();
void m_talk_parrot();
void m_talk_heyna();
void m_talk_archmage();
void m_talk_merchant();
void m_sp_merchant();
void m_sp_court();
void m_sp_lair();
void m_sp_whistleblower();
char *mantype();

/* omon functions */
int m_statusp();
int m_immunityp();
int tacticalp();
void m_pulse();
void m_status_set();
void m_damage();
void monster_action();
void m_death();
void m_hit();
void m_miss();
void m_pickup();
void m_dropstuff();
void m_status_reset();
void monster_strike();
void tacmonster();
void freemlist();
void monster_talk();
void monster_melee();
void monster_move();
void monster_special();
void movemonster();
void make_log_npc();
void m_abyss();
void transcribe_monster_actions();
void m_movefunction();
void determin_npc_behavior();
void m_water();
void m_lava();
void m_altar();
void m_whirlwind();
void m_fire();
void make_hiscore_npc();
void m_trap_date();
void m_trap_pit();
void m_trap_door();
void m_trap_snare();
void m_trap_dart();
void m_trap_fire();
void m_trap_disintegrate();
void m_trap_teleport();
void m_trap_manadrain();
void m_trap_acid();
void m_trap_sleepgas();
void m_trap_obyss();
void strengthen_death();
void fix_phantom();
void destroy_order();
char random_loc();
int corpsevalue();
int monster_hit();

/* olev functions */
void populate_level();
void stock_level();
void wandercheck();
void make_site_monster();
void free_dungeon();
void tactical_map();
void make_country_monsters();
void make_site_treasure();
void make_specific_treasure();
int difficulty();
pmt m_create();
pmt make_creature();

/* osave functions */
int save_game();
int restore_game();
int signalsave();
void save_player();
void save_player_items();
void save_player_spells();
void save_player_item_knowledge();
void save_monsters();
void save_country();
void save_level();
void save_item();
void save_itemlist();
void restore_player();
void restore_player_items();
void restore_player_spells();
void restore_player_item_knowledge();
void restore_monsters();
void restore_level();
void restore_country();
pob restore_item();
pol restore_itemlist();

/* oitemf functions (1 to 3) */
void weapon_bare_hands();
void weapon_normal_hit();
void weapon_demonblade();
void weapon_lightsabre();
void weapon_mace_disrupt();
void weapon_tangle();
void weapon_vorpal();
void weapon_desecrate();
void weapon_firestar();
void weapon_defend();
void weapon_victrix();
void weapon_bolt();
void weapon_arrow();
void weapon_scythe();
void weapon_acidwhip();
void i_no_op();
void i_nothing();
void i_stargem();
void i_sceptre();
void i_deflect();
void i_spells();
void i_bless();
void i_enchant();
void i_acquire();
void i_jane_t();
void i_flux();
void i_breathing();
void i_invisible();
void i_perm_invisible();
void i_teleport();
void i_wish();
void i_clairvoyance();
void i_displace();
void i_id();
void i_augment();
void i_heal();
void i_objdet();
void i_mondet();
void i_sleep_self();
void i_neutralize_poison();
void i_speed();
void i_restore();
void i_azoth();
void i_regenerate();
void i_demonblade();
void i_firebolt();
void i_lbolt();
void i_missile();
void i_fireball();
void i_lball();
void i_snowball();
void i_sleep_other();
void i_summon();
void i_hide();
void i_disrupt();
void i_disintegrate();
void i_perm_vision();
void i_perm_burden();
void i_perm_fire_resist();
void i_charge();
void i_perm_poison_resist();
void i_perm_strength();
void i_perm_gaze_immune();
void i_perm_energy_resist();
void i_perm_speed();
void i_perm_breathing();
void i_perm_displace();
void i_perm_negimmune();
void i_mace_disrupt();
void i_warp();
void i_food();
void i_poison_food();
void i_stim();
void i_immune();
void i_lembas();
void i_pow();
void i_orbfire();
void i_orbwater();
void i_orbair();
void i_orbearth();
void i_orbdead();
void i_orbmastery();
void i_lightsabre();
void i_alert();
void i_knowledge();
void i_pick();
void i_perm_knowledge();
void i_fear_resist();
void i_perm_fear_resist();
void i_key();
void i_defend();
void i_victrix();
void i_apport();
void i_corpse();
void i_accuracy();
void i_perm_accuracy();
void i_hero();
void i_perm_hero();
void i_levitate();
void i_perm_regenerate();
void i_perm_levitate();
void i_perm_protection();
void i_perm_agility();
void i_truesight();
void i_perm_truesight();
void i_cure();
void i_dispel();
void i_pepper_food();
void i_enchantment();
void polymorph();
void i_crystal();
void i_klowynia();
void i_death();
void i_life();
void i_helm();
void i_antioch();
void i_illuminate();
void i_perm_illuminate();
void i_desecrate();
void i_trap();
void i_juggernaut();
void i_raise_portcullis();
void i_planes();
void i_fear();
void i_polymorph();
void i_symbol();
void i_perm_deflect();
void i_normal_armor();
void i_normal_weapon();
void i_normal_shield();

/* oitem functions */
void item_use();
pob create_object();
void make_shield();
void make_armor();
void make_weapon();
void make_scroll();
void make_potion();
void make_food();
void make_thing();
void make_ring();
void make_artifact();
void make_corpse();
void make_boots();
void make_cloak();
void make_stick();
void make_cash();
int itemplus();
int itemcharge();
int itemblessing();
int twohandedp();
char *scrollname();
char *potionname();
char *stickname();
char *ringname();
char *cloakname();
char *bootname();
char *grotname();

/* oinv functions */
void pickup_at();
void drop_at();
void givemonster();
void confrom_lost_object();
void conform_unused_object();
void gain_item();
void drop_money();
void add_to_pack();
void setplustr();
void setchargestr();
void setnumstr();
void p_drop_at();
void give_money();
void conform_lost_objects();
void itemlist_inventory();
void take_from_pack();
void merge_item();
void take_from_slot();
void inventory_control();
void item_inventory();
void drop_from_slot();
void put_to_pack();
void switch_to_slot();
void top_inventory_control();
void lose_all_items();
void dispose_lost_objects();
void pack_extra_items();
void fixpack();
int get_inventory_slot();
int getitem();
int objequal();
int slottable();
int item_useable();
int find_and_remove_item();
int getitem();
int badobjecT();
int cursed();
int get_money();
int get_item_number();
int find_item();

pob detach_money();
pob split_item();
char *itemid();
char *cashstr();

/* oguild functions */
void l_druid();
void l_theives_guild();
void l_college();
void l_merc_guild();
void l_sorcerors();
void l_altar();
void l_arena();
void l_order();

/* opriest functions */
void make_hp();
void hp_req_test();
void hp_req_print();
void increase_priest_rank();
void answer_prayer();
int check_sacrilege();

/* ocountry functions */
void load_country();
void load_dlair();
void load_speak();
void load_misle();
void load_temple();
void make_high_priest();
void random_temple_site();

/* ovillage functions */
void load_village();
void make_horse();
void make_sheep();
void make_guard();
void make_merchant();
void make_food_bin();
void assign_village_function();
void special_village_site();
