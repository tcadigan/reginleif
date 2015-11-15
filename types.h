#ifndef TYPES_H_
#define TYPES_H_

typedef unsigned char byteint;

#ifdef NO_SIGNED_CHARS
typedef short bytlint;

#else

typedef char bytlint;
#endif

typedef unsigned short wordint;
typedef short worlint;
typedef char atype[17];
typedef char btype[15];
typedef char ctype[27];
typedef char dtype[6];
typedef char etype[35];
typedef char ttype[80];
typedef char vtype[80];
typedef char stat_type[7];

typedef struct creature_type {
    ctype name;          /* Description of creature   */
    unsigned int cmove;  /* Bit field                 */
    unsigned int spells; /* Creature spells           */
    wordint cdefense;    /* Bit field                 */
    worlint sleep;       /* Inactive counter          */
    wordint mexp;        /* Experience value for kill */
    byteint aaf;         /* Area affect radius        */
    byteint ac;          /* AC                        */
    bytlint speed;       /* Movement speed            */
    char cchar;          /* Character representation  */
    dtype hd;            /* Creatures hit die         */
    etype damage;        /* Type attack and damage    */
    byteint level;       /* Level of creature         */
} creature_type;

typedef struct monster_type {
    worlint hp;     /* Hit points                    */
    worlint csleep; /* Inactive counter              */
    worlint cdis;   /* Current distance from player  */
    wordint mptr;   /* Pointer int creature          */
    wordint nptr;   /* Pointer to next block         */
    bytlint cspeed; /* Movement speed                */

    /* Note: FY and FX constrain dungeon size to 255 */
    byteint fy;     /* Y pointer into map            */
    byteint fx;     /* X pointer into map            */
    int stunned;    /* Rounds stunned                */
    int ml;         /* On if shown                   */
    int confused;   /* On if confused                */
} monster_type;

typedef struct treasure_type {
    ttype name;         /* Object name                */
    byteint tval;       /* Category number            */
    char tchar;         /* Character representation   */
    unsigned int flags; /* Special flags              */
    int p1;             /* Miscellaneous use variable */
    int cost;           /* Cost of item               */
    int subval;         /* Sub-category number        */
    wordint weight;     /* Weight                     */
    wordint number;     /* Number of items            */
    worlint tohit;      /* Plusses to hit             */
    worlint todam;      /* Plusses to damage          */
    worlint ac;         /* Normal AC                  */
    worlint toac;       /* Plusses to AC              */
    dtype damage;       /* Damage when hits           */
    bytlint level;      /* Level item found           */
} treasure_type;

typedef struct player_type {
    struct misc {
	vtype name;       /* Name of character     */
	vtype race;       /* Race of character     */
	vtype sex;        /* Sex of character      */
	vtype title;      /* Character's title     */
	vtype tclass;     /* Character's class     */
	int max_exp;      /* Max experience        */
	int exp;          /* Current experience    */
	int au;           /* Gold                  */
	wordint age;      /* Character's age       */
	wordint ht;       /* Height                */
	wordint wt;       /* Weight                */
	wordint lev;      /* Level                 */
	wordint max_lev;  /* Max level explored    */
	worlint srh;      /* Chance in search      */
	worlint fos;      /* Frequency of search   */
	worlint bth;      /* Base to hit           */
	worlint bthb;     /* Base to hit with bows */
	worlint mana;     /* Mana points           */
	worlint mhp;      /* Max hit points        */
	worlint ptohit;   /* Plusses to hit        */
	worlint ptodam;   /* Plusses to damage     */
	worlint pac;      /* Total AC              */
	worlint ptoac;    /* Magical AC            */
	worlint dis_th;   /* Display +ToHit        */
	worlint dis_td;   /* Display +ToDam        */
	worlint dis_ac;   /* Display +ToAC         */
	worlint dis_tac;  /* Display +ToTAC        */
	worlint disarm;   /* % to disarm           */
	worlint save;     /* Saving throw          */
	worlint sc;       /* Social class          */
	byteint pclass;   /* # of class            */
	byteint prace;    /* # of race             */
	byteint hitdie;   /* Char hit die          */
	bytlint stl;      /* Stealth factor        */
	double expfact;   /* Experience factor     */
	double cmana;     /* Current mana points   */
	double chp;       /* Current hit points    */
	vtype history[5]; /* History record        */
    } misc;
    
    struct stats {
	byteint str;   /* Max strength         */
	byteint cstr;  /* Current strength     */
	byteint dex;   /* Max dexterity        */
	byteint cdex;  /* Current dexterity    */
	byteint con;   /* Max constitution     */
	byteint ccon;  /* Current constitution */
	byteint intel; /* Max intelligence     */
	byteint cint;  /* Current intelligence */
	byteint wis;   /* Max wisdom           */
	byteint cwis;  /* Current wisdom       */
	byteint chr;   /* Max charisma         */
	byteint cchr;  /* Current charisma     */
    } stats;

    struct flags {
	unsigned int status; /* Status of player       */
	int rest;            /* Rest counter           */
	int blind;           /* Blindness counter      */
	int paralysis;       /* Paralysis counter      */
	int confused;        /* Confusion counter      */
	int food;            /* Food counter           */
	int food_digested;   /* Food per round         */
	int protection;      /* Protection from evil   */
	int speed;           /* Current speed adjust   */
	int fast;            /* Temporary speed change */
	int slow;            /* Temporary speed change */
	int afraid;          /* Fear                   */
	int poisoned;        /* Poisoned               */
	int image;           /* Hallucinate            */
	int protevil;        /* Protection VS evil     */
	int invuln;          /* Increases AC           */
	int hero;            /* Heroism                */
	int shero;           /* Super heroism          */
	int blessed;         /* Blessed                */
	int resist_heat;     /* Timed head resist      */
	int resist_cold;     /* Timed cold resist      */
	int detect_inv;      /* Timed see invisible    */
	int word_recall;     /* Time teleport level    */
	int see_infra;       /* See warm creatures     */
	int tim_infra;       /* Time infra. vision     */
	int see_inv;         /* Can see invisible      */
	int teleport;        /* Random teleportation   */
	int free_act;        /* Never paralyzed        */
	int slow_digest;     /* Lower food needs       */
	int aggravate;       /* Aggravate monsters     */
	int fire_resist;     /* Resistance to fire     */
	int cold_resist;     /* Resistance to cold     */
	int acid_resist;     /* Resistance to acid     */
	int regenerate;      /* Regenerate hit points  */
	int lght_resist;     /* Resistance to light    */
	int ffall;           /* No damage falling      */
	int sustain_str;     /* Keep strength          */
	int sustain_int;     /* Keep intelligence      */
	int sustain_wis;     /* Keep wisdom            */
	int sustain_con;     /* Keep constitution      */
	int sustain_dex;     /* Keep dexterity         */
	int sustain_chr;     /* keep charisma          */
	int confuse_monster; /* Glowing hands...       */
    } flags;
} player_type;

typedef struct spell_type {
    ctype sname;
    byteint slevel;
    byteint smana;
    wordint sexp;
    byteint sfail;
    int learned;
} spell_type;

typedef struct spl_rec {
    int splnum;
    int splchn;
} spl_rec;

typedef spl_rec spl_typ[22];

typedef struct race_type {
    vtype trace;         /* Type of race               */
    bytlint str_adj;
    bytlint int_adj;
    bytlint wis_adj;
    bytlint dex_adj;
    bytlint con_adj;
    bytlint chr_adj;
    wordint b_age;       /* Base age of character      */
    wordint m_age;       /* Maximum age for character  */
    wordint m_b_ht;      /* Base height of males       */
    wordint m_m_ht;      /* Mod base height for males  */
    wordint m_b_wt;      /* Base weight for males      */
    wordint m_m_wt;      /* Mod weight for males       */
    wordint f_b_ht;      /* Base height for males      */
    wordint f_m_ht;      /* Mod height for females     */
    wordint f_b_wt;      /* Base weight for females    */
    wordint f_m_wt;      /* Mod weight for females     */
    double b_exp;        /* Base experience factor     */
    bytlint b_dis;       /* Base chance to disarm      */
    bytlint srh;         /* Base chance for search     */
    bytlint stl;         /* Stealth of character       */
    bytlint fos;         /* Frequency of auto search   */
    bytlint bth;         /* Adj base chance to hit     */
    bytlint bthb;        /* Adj base to hit with bows  */
    bytlint bsav;        /* Race base for saving throw */
    bytlint bhitdie;     /* Base hit points for race   */
    bytlint infra;       /* See infra-red              */
    unsigned int tclass; /* Bit field for class types  */
} race_type;

typedef struct class_type {
    vtype title;      /* Type of class                  */
    double m_exp;     /* Class experience factor        */
    bytlint adj_hd;   /* Adjust hit points              */
    bytlint mdis;     /* Modifier disarming traps       */
    bytlint msrh;     /* Modifier to searching          */
    bytlint mstl;     /* Modifier to stealth            */
    bytlint mfos;     /* Modifier to freq-of-search     */
    bytlint mbth;     /* Modifier to base to hit        */
    bytlint mbthb;    /* Modifier to base to hit - bows */
    bytlint msav;     /* Class modifier to save         */
    bytlint madj_str; /* Class modifier to strength     */
    bytlint madj_int; /* Class modifier to intelligence */
    bytlint madj_wis; /* Class modifier to wisdom       */
    bytlint madj_dex; /* Class modifier to dexterity    */
    bytlint madj_con; /* Class modifier to constitution */
    bytlint madj_chr; /* Class modifier to charisma     */
    int pspell;       /* Class use priest spells        */
    int mspell;       /* Class use mage spells          */
} class_type;

typedef struct background_type {
    vtype info;    /* History information         */
    byteint roll;  /* Die roll needed for history */
    byteint chart; /* Table number                */
    bytlint next;  /* Pointer to next table       */
    bytlint bonus; /* Bonus to the Social Class   */
} background_type;

typedef struct floor_type {
    int ftval;
    int ftopen;
} floor_type;

typedef struct cave_type {
    byteint cptr;
    byteint tptr;
    int fval;
    int fopen;
    int fm; /* Used for hidden objects */
    int pl;
    int tl;
} cave_type;

typedef struct owner_type {
    vtype owner_name;
    worlint max_cost;
    double max_inflate;
    double min_inflate;
    double haggle_per;
    byteint owner_race;
    byteint insult_max;
} owner_type;

typedef struct inven_record {
    int scost;
    treasure_type sitem;
} inven_record;

typedef struct store_type {
    worlint store_open;
    short insult_cur;
    byteint owner;
    byteint store_ctr;
    inven_record store_inven[STORE_INVEN_MAX];
} store_type;

typedef struct high_scores {
    long int points;
    wordint lev;
    wordint max_lev;
    worlint mhp;
    double chp;
    int uid;
    int dun_level;
    char sex;
    vtype name;
    vtype died_from;
    byteint pclass;
    byteint prace;
} high_scores;
    
#endif
