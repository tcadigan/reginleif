/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
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
 * The GNU General Public License is contains in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * #ident  "%W% %G% %Q%"
 * $Header: /var/cvs/gbp/GB+/hdrs/ranks.h,v 1.3 2007/07/06 16:55:26 gbp Exp $
 *
 * ranks.h
 *
 * Created: Thu Feb 18 04:22:18 EST 1993
 * Author: J. Deragon (deragon@jethro.nyu,edu)
 *
 * Version: %I% %U%
 *
 * File with all the ranks for the various commands. (Replaces numerous *.p
 * files)
 */
#ifndef RANKS_H_
#define RANKS_H_

/*
 * Rank the guest race is given by default keeping this PRIVATE prevents a lot of
 * headaches in the long run.
 */
#define GUEST_RANK PRIVATE

/* Ranks for user/build.c */
#define RANK_ROUTE GENERAL
#define RANK_BUILD CAPTAIN
#define RANK_UPGRADE CAPTAIN
#define RANK_MODIFY CAPTAIN
#define RANK_SELL GENERAL
#define RANK_BID GENERAL

/* Ranks for user/move.c */
#define RANK_ARM CAPTAIN
#define RANK_MOVE CAPTAIN
#define RANK_WALK GENERAL

/* Ranks for user/scrap.c */
#define RANK_SCRAP GENERAL

/* Ranks for user/toxi.c */
#define RANK_TOXI CAPTAIN

/* Ranks for user/tech.c */
#define RANK_TECH GENERAL

/* Ranks for user/dissolve.c */
#define RANK_DISSOLVE LEADER /* I wouldn't change this. --jpd-- */

/* Ranks for user/survey.c */
#define RANK_REPAIR CAPTAIN
#define RANK_SURVEY NOVICE

/* Ranks for user/sche.c */
#define RANK_SCHED NOVICE

/* Ranks for user/relation.c */
#define RANK_RELATION NOVICE

/* Ranks for user/prof.c */
#define RANK_WHOIS NOVICE
#define RANK_TREASURY NOVICE
#define RANK_PROFILE NOVICE

/* Ranks for user/name.c */
#define RANK_PERSONAL NOVICE
#define RANK_INSURG GENERAL
#define RANK_PAY GENERAL
#define RANK_GIVE GENERAL
#define RANK_PAGE PRIVATE
#define RANK_MOTTO GENERAL
#define RANK_NSHIP PRIVATE
#define RANK_NCLASS PRIVATE
#define RANK_NBLOCK GENERAL
#define RANK_NRACE GENERAL
#define RANK_ANNOUNCE NOVICE

/* Ranks for user/power.c */
#define RANK_BLOCK PRIVATE
#define RANK_POWER PRIVATE

/* Ranks for user/map.c */
#define RANK_MAP NOVICE

/* Ranks for user/mobilize.c */
#define RANK_MOBILIZE GENERAL
#define RANK_TAX GENERAL

/* Ranks for user/anal.c */
#define RANK_ANAL PRIVATE

/* Ranks for user/autoreport.c */
#define RANK_AUTOREP GENERAL

/* Ranks for user/capital.c */
#define RANK_CAPITAL GENERAL

/* Ranks for user/cs.c */
#define RANK_CS NOVICE

/* Ranks for user/declare.c */
#define RANK_INVITE GENERAL
#define RANK_DECLARE GENERAL
#define RANK_PLEDGE GENERAL
#define RANK_VOTE GENERAL

/* Ranks for user/enslave.c */
#define RANK_ENSLAVE CAPTAIN

/* Ranks for user/examine.c */
#define RANK_EXAMINE PRIVATE

/* Ranks for user/explore.c */
#define RANK_COLONY PRIVATE
#define RANK_DIST PRIVATE
#define RANK_EXPLORE PRIVATE
#define RANK_TECHS CAPTAIN

/* Ranks for user/fuel.c */
#define RANK_FUEL CAPTAIN

/* Ranks for user/dock.c */
#define RANK_DOCK CAPTAIN

/* Ranks for user/land.c */
#define RANK_LAND CAPTAIN

/* Ranks for user/launch.h */
#define RANK_LAUNCH CAPTAIN

/* Ranks for user/load.c */
#define RANK_LOAD CAPTAIN
#define RANK_JETT CAPTAIN
#define RANK_DUMP GENERAL
#define RANK_TRANSFER CAPTAIN
#define RANK_MOUNT CAPTAIN

/* Ranks for user/capture.c */
#define RANK_CAPTURE CAPTAIN

/* Ranks for user/fire.c */
#define RANK_FIRE CAPTAIN
#define RANK_BOMBARD CAPTAIN
#define RANK_DEFEND CAPTAIN
#define RANK_DETONATE CAPTAIN

/* Ranks for user/orbit.c */
#define RANK_ORBIT NOVICE

/* Ranks for user/zoom.c */
#define RANK_ZOOM NOVICE

/* Ranks for user/rst.c */
#define RANK_REPORT CAPTAIN
#define RANK_TACT CAPTAIN
#define RANK_REPUNIV CAPTAIN /* I wouldn't change this either --jpd-- */
#define RANK_REPPLAN PRIVATE
#define RANK_REPSTAR NOVICE

/* Ranks for user/vict.c */
#define RANK_VICT NOVICE

/* Ranks for user/shlmisc.c */
#define RANK_ALLOCATE CAPTAIN
#define RANK_GRANT_SHIP CAPTAIN
#define RANK_GRANT_MON GENERAL

#endif /* RANKS_H_ */
