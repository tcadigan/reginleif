extern int character_generated; /* Don't save score until char gen finished */
extern int character_saved; /* Prevents save on kill after save_char() */
extern int highscore_fd; /* High score file descriptor */
extern int player_max_exp; /* Max experience possible */
extern char norm_state[STATE_SIZE]; /* Contains seed # */
extern char randes_state[STATE_SIZE];
extern int randes_seed; /* For encoding colors */
extern char town_state[STATE_SIZE];
extern int town_seed; /* Seed for town generation */
extern int cur_height; /* Current dungeon size */
extern int cur_width; /* Current dungeon size */
extern int dun_level; /* Current dungeon level */
extern int missile_ctr; /* Counter for missiles */
extern int msg_flag; /* Set with first message */
extern int generate; /* Generate next level */
extern int death; /* True if died */
extern vtype died_from; /* What killed him */
extern int find_flag; /* Used in MORIA */
extern int reset_flag; /* Used in MORIA */
extern int stat_column; /* Column for stats */
extern unsigned int print_stat; /* Flag for stats */
extern int turn; /* Current turn of game */
extern in wizard1; /* Wizard flag */
extern int wizard2; /* Wizard flag */
extern int panic_save; /* This is true if playing from a panic save */
extern int used_line[23];
extern char password1[12];
extern char password2[12];
extern int wait_for_more;
extern int key_bindings;
extern char days[7][80];
extern int closing_flag; /* Used for closing */

/* Bit testing array */
extern unsigned int bit_array[32];

/* The following are calculated from max dungeon sizes */
extern int max_panel_rows;
extern int max_panel_cols;
extern int panel_row;
extern int panel_col;
extern int panel_row_min;
extern int panel_row_max;
extern int panel_col_min;
extern int panel_col_max;
extern int panel_col_prt;
extern int panel_row_prt;

/* The following are all floor definitions */
extern cave_type cave[MAX_HEIGHT][MAX_WIDTH];
extern cave_type blank_floor;
extern floor_type dopen_floor;
extern floor_type lopen_floor;
extern floor_type corr_floor1;
extern floor_type corr_floor2;
extern floor_type corr_floor3;
extern floor_type corr_floor4;
extern floor_type rock_wall1;
extern floor_type rock_wall2;
extern floor_type rock_wall3;
extern floor_type boundary_wall;

/* The following are player variables */
extern player_type py;
extern btype player_title[MAX_CLASS][MAX_PLAYER_LEVEL];
extern int player_exp[MAX_PLAYER_LEVEL];
extern double acc_exp; /* Accumulator for fractional experience */
extern dtype bare_hands;
extern int char_row;
extern int char_col;
extern int global_com_val;
extern race_type race[MAX_RACES];
extern background_type background[MAX_BACKGROUND];
extern double rgold_adj[MAX_RACES][MAX_RACES];

/*
 * SUN4 has a variables called class in the include file <math.h>
 * avoid a conflict by not defining my class in the file misc1.c
 */
#ifndef DONT_DEFINE_CLASS

extern class_type class[MAX_CLASS];

#endif

extern spell_type magic_spell[MAX_CLASS][31];
extern treasure_type mush;
extern byteint player_init[MAX_CLASS][5];
extern int total_winner;

/* The following are store definitions */
extern owner_type owners[MAX_OWNERS];
extern store_type store[MAX_STORES];
extern treasure_type store_door[MAX_STORES];
extern int store_choice[MAX_STORES][STORE_CHOICES];

int (*store_buy[MAX_STORES])();

/* The following are treasure arrays and variables */
extern treasure_type object_list[MAX_OBJECTS];
extern int object_ident[MAX_OBJECTS];
extern int t_level[MAX_OBJ_LEVEL];
extern treasure_type gold_list[MAX_GOLD];
extern treasure_type t_list[MAX_TALLOC];
extern treasure_type inventory[INVEN_ARRAY_SIZE];
extern treasure_type invenotry_init[INVEN_INIT_MAX];
extern treasure_type blank_treasure;
extern int inven_ctr; /* Total different objects */
extern int inven_weight; /* Current carried weight */
extern int equip_ctr; /* Current equipment counter */
extern int tcptr; /* Current treasure heap pointer */

/* The following are feature objects defined for dungeon */
extern treasure_type trap_lista[MAX_TRAPA];
extern treasure_type trap_listb[MAX_TRAPB];
extern treasure_type scare_monster; /* Special trap */
extern treasure_type rubble;
extern treasure_type door_list[3];
extern treasure_type up_stair;
extern treasure_type down_stair;

/* The following are creature arrays and variables */
extern creature_type c_list[MAX_CREATURES];
extern monster_type m_list[MAX_MALLOC];
extern int m_level[MAX_MONS_LEVEL];
extern monster_type blank_monster; /* Blank mosnter values */
extern int muptr; /* Current used monster pointer */
extern int mfptr; /* Current free monster pointer */
extern int mon_tot_mult; /* # of repro's of creature */

/* The following are arrays for descriptive pieces */
extern atype colors[MAX_COLORS];
extern atype mushrooms[MAX_MUSH];
extern atype woods[MAX_WOODS];
extern atype metals[MAX_METALS];
extern atype rocks[MAX_ROCKS];
extern atype amulets[MAX_AMULETS];
extern dtype syllables[MAX_SYLLABLES];

/* Function return values */
char *pad();
byteint de_statp();
byteint in_statp();
char *likert();
