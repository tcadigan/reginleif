/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * #ident  "@(#)races.h 1.9 11/5/93 "
 *
 * $Header: /var/cvs/gbp/GB+/hdrs/races.h,v 1.3 2007/07/06 16:55:26 gbp Exp $
 */
#ifndef RACES_H_
#define RACES_H_

typedef struct {
    char invisible;
    char standby;
    char color; /* 1 if you are using a color client */
    char double_digits;
    char inverse;
    char geography;
    char autoload;
    char gag;
    char highlight; /* Which race to highlight */
    char compat;
    /* char autopurge; /\* Add this in for next run *\/ */
} toggletype;

struct race {
    int Playernm;
    char name[RNAMESIZE]; /* Racial name. */
    char password[RNAMESIZE];
    char info[PERSONALSIZE]; /* Personal information */
    char motto[MOTTOSIZE]; /* For a cute message */
    unsigned char absorb; /* Does this race absorb enemies in combat? */
    unsigned char collective_iq; /* Does this race have collective IQ? */
    unsigned char pods; /* Can this race use pods? */
    unsigned char fighters; /* Fight rating of this race */
    unsigned char IQ;
    unsigned char IQ_limit; /* Asymptotic IQ for collective IQ races */
    unsigned char number_sexes;
    /*
     * Chance that this race will increase the fertility of its sectors by 1
     * each update.
     */
    unsigned char fertilize;
    double adventurism;
    double birthrate;
    double mass;
    double metabolism;
    double likes[WASTER + 1]; /* Sector condition compats. */
    short conditions[OTHER + 1]; /* Atmosphere/temperature this race likes. */
    unsigned char likesbest; /* 100% compat sector condition for this race. */
    char dissolved; /* Player has quit. */
    char God; /* Planer is a God race. */
    char Guest; /* Player is a guest race. */
    char Metamorph; /* Player is a morph; (for printing). */
    char monitor; /* God is monitoring this race. */
    char translate[MAXPLAYERS]; /* Translation mod for each player */
    unsigned long atwar[2]; /* War 64 bits */
    unsigned long allied[2]; /* Allies 64 bits */
    long morale; /* Race's morale level */
     /*
      * Keep track of war status against another player - for short reports
      */
    unsigned int points[MAXPLAYERS];
    unsigned short Gov_ship; /* Shipnumber of government ship. */
    unsigned short controlled_planets; /* Number of planets under control. */
    unsigned short victory_turns;
    unsigned short turn;
    double tech;
    unsigned char discoveries[80]; /* Tech discoveries. */
    unsigned long victory_score; /* Number of victory points. */
    unsigned long votes;
    unsigned long planet_points; /* For the determination of global APs */
    char governors;

    struct gov {
        char name[RNAMESIZE];
        char password[RNAMESIZE];
        unsigned char rank;
        unsigned char active;
        unsigned char deflevel;
        unsigned char defplanetnum; /* Current default */
        unsigned char homelevel;
        unsigned char homesystem;
        unsigned char homeplanetnum; /* Home place */
        unsigned long newspos[4]; /* News file pointers */
        toggletype toggel;
        unsigned long money;
        unsigned long income;
        unsigned long maintain;
        unsigned long cost_tech;
        unsigned long cost_market;
        unsigned long profit_market;
        unsigned long channel[2]; /* The channel we are communicating on */
        time_t last_login; /* Last login for the governor */
        char last_ip[16];

        struct {
            unsigned int csp:1;
            unsigned int empty:5;
            unsigned int csp_client_vers;
        } CSP_client_info;
    } governor[MAXGOVERNORS + 1];

    /* Fleet struct -mfw */
    struct flt {
        char name[FLEET_NAMESIZE + 1]; /* Name of this fleet */
        unsigned short flagship; /* First ship in the fleet list */
        unsigned short admiral; /* Governor in control of this fleet */
    } fleet[MAXFLEETS + 1];
};

/* Vote bits */
#define VOTE_UPDATE_GO 0x01 /* On - Go, Off - Wait */

/* Special discoveries */
#define D_HYPER_DRIVE 0 /* Hyperspace capable */
#define D_LASER 1 /* Can construct/operate combat lasers */
#define D_CEW 2 /* Can construct/operate cews */

#ifdef USE_VN
#define D_VN 3 /* Can construct Von Neumann machines */
#endif

#define D_TRACTOR_BEAM 4 /* Tractor/repulsor beam */
#define D_TRANSPORTER 5 /* Tractor beam (local) */
#define D_AVPM 6 /* AVPM transporter */
#define D_CLOAK 7 /* Cloaking device */
#define D_WORMHOLE 8 /* Wormhole */
#define D_CRYSTAL 9 /* Crystal power */
#define D_ATMOS 10 /* Atmospheric processor */

#define Hyper_drive(r) ((r)->discoveries[D_HYPER_DRIVE])
#define Crystal(r) ((r)->discoveries[D_CRYSTAL])
#define Atmos(r) ((r)->discoveries[D_ATMOS])
#define Laser(r) ((r)->discoveries[D_LASER])
#define Wormhole(r) ((r)->discoveries[D_WORMHOLE])

#ifdef USE_VN
#define Vn(r) ((r)->discoveries[D_VN])

#else

#define Vn(r) (0)
#endif

#define Cew(r) ((r)->discoveries[D_CEW])
#define Cloak(r) ((r)->discoveries[D_CLOAK])
#define Avpm(r) ((r)->discoveries[D_AVPM])
#define Tractor_beam(r) ((r)->discoveries[D_TRACTOR_BEAM])
#define Transporter(r) ((r)->discoveries[D_TRANSPORTER])

#define TECH_ATMOS 80.0
#define TECH_HYPER_DRIVE 50.0
#define TECH_LASER 100.0
#define TECH_CEW 150.0

#ifdef USE_VN
#define TECH_VN 120.0
#endif

#define TECH_TRACTOR_BEAM 999.0
#define TECH_TRANSPORTER 999.0
#define TECH_AVPM 200.0 /* Was 250: Changed by Gardan 20.12.1996 */
#define TECH_CLOAK 170.0
#define TECH_WORMHOLE 100.0
#define TECH_CRYSTAL 80.0 /* Was 50: changed by Gardan 20.12.1996 */

struct block {
    int Playernum;
    char name[RNAMESIZE];
    char motto[MOTTOSIZE];
    unsigned long invite[2];
    unsigned long pledge[2];
    unsigned long atwar[2];
    unsigned long allied[2];
    unsigned short next;
    unsigned short systems_owned;
    unsigned long VPs;
    unsigned long money;
    unsigned long dummy[2];
};

struct power_blocks {
    char time[128];
    unsigned long members[MAXPLAYERS];
    unsigned long troops[MAXPLAYERS]; /* Total troops */
    unsigned long popn[MAXPLAYERS]; /* Total population */
    unsigned long resource[MAXPLAYERS]; /* Total resource in stock */
    unsigned long fuel[MAXPLAYERS];
    unsigned long destruct[MAXPLAYERS]; /* Total dest in stock */
    unsigned short ships_owned[MAXPLAYERS];
    unsigned short systems_owned[MAXPLAYERS];
    unsigned long sectors_owned[MAXPLAYERS];
    unsigned long money[MAXPLAYERS];
    unsigned short VPs[MAXPLAYERS];
    unsigned long planets_owned[MAXPLAYERS];
};

typedef struct race racetype;
typedef struct block blocktype;

extern struct block Blocks[MAXPLAYERS];
extern struct power_blocks Power_blocks;
extern racetype *races[MAXPLAYERS];

#endif /* RACES_H_ */
