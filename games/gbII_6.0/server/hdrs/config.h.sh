#!/usr/bin/sh

case $CONFIG in
    '')
        if test ! -f config.sh; then
            ln ../config.sh . || \
                ln ../../config.sh . || \
                ln ../../../config.sh . || \
                (echo "Can't find config.sh."; exit 1)
            echo "Using config.sh from above..."
        fi
        . ./config.sh
        ;;
esac

echo "Extracting config.h (with variable substitutions)"

sed -e 's/^#undef[      ]*\(.*\)/\/\* #undef \1\*\//g' <<!GROK!THIS! > config.h
#ifndef CONFIG_H_INC
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
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * config.h
 * This file was produced by running the config.h.sh script, which gets its
 * values from config.sh, which is generally produced by running Configure.
 *
 * Feel free to modify any of this as the need arises. Note, however, that
 * running config.h.sh again will wipe out any changes you've made. For a more
 * permanent change edit config.sh and rerun config.h.sh.
 */

/* CONFIG.H FILE FOR GB+ Version 1 JPD 12.10.92 */

/*
 * BIN:
 *   This symbol holds the name of the directory in which the user wants to put
 *   publicly executable images for the package in question. It is most often a
 *   local directory such as /usr/local/bin.
 */
#define BIN "$bin"

/*
 * BYTEORDER:
 *   This symbol contains an encoding of the order of bytes in a long. Usual
 *   values (in octal) are 01234, 04321, 02143, 03412...
 */
#define BYTEORDER 0x$byteorder

/*
 * These are old BSD defines, linux had NBBY but not NBPW, I thought while I was
 * at it I'd figure them out and set them here. Note that while NBBY will likely
 * never change, NBPW can vary due to architecture and compiler. On an older
 * Ultrix machine NBPW was 2, but on my Linux box it's 4. So I've modified the
 * Configure string to figure them out and set the values here. -mfw
 */
#ifndef NBBY
/* Number of Bits per Byte */
#define NBBY $nbby
#endif

#ifndef NBPW
/* Number of Bytes per Word */
#define NBPW $nbpw
#endif

/*
 * CPPSTDIN:
 *   This symbol contains the first part of the string which will invoke the C
 *   preprocessor on the standard input and produce to standard output. Typical
 *   value of "cc -E" or "/lib/cpp"/
 */
#define CPPSTDIN "$cppstdin"

/*
 * CPPMINUS:
 *   This symbol contains the second part of the string which will invoke the C
 *   preprocessor on the standard input and produce to standard output. This
 *   symbol will have the value "-" if CPPSTDIN needs a minus to specify
 *   standard input, otherwise the value is "".
 */
#define CPPMINUS "$cppminus"

/*
 * GETOPT:
 *   This symbol, if defined, indicates that the getopt() routine exists.
 */
#d_getopt GETOPT

/*
 * HOSTNAME:
 *   This symbol contains the name of the host the program is going to be run
 *   on. The domain is not kept with the hostname, but must be gotten from
 *   MYDOMAIN. The dot comes with MYDOMAIN, and need not be supplied by the
 *   program.
 */
#define HOSTNAME "$hostname"

/*
 * MYDOMAIN:
 *   This symbol contains the domain of the host the program is going to run
 *   on. The domain must be appended to HOSTNAME to form a complete host
 *   name. The dot comes with MYDOMAIN, and need not be supplied by the
 *   program. If the host name is derived from PHOSTNAME, the domain may or may
 *   not already be there, and the program should check.
 */
#define MYDOMAIN "$mydomain"

/*
 * GB_HOST:
 *   The fully qualified hostname and domain name of the GB server
 */
#define GB_HOST "$host"

/*
 * GB_PORT:
 *   This symbol contains the port number that GB will run on.
 */
#define GB_PORT $port

/*
 * GODADDR:
 *   Email address of the game's admin (god)
 */
#define GODADDR "$godaddr"

#define RACEGEN_PORT $raceport

/*
 * CHAP Authentication
 */
#$d_define CHAP_AUTH

/*
 * COMBAT UPDATE:
 *   Combat is not allowed until after this update
 */
#define COMBAT_UPDATE $d_combat_update

/*
 * THRESHLOADING:
 *   Use the threshloading code?
 */
#$d_threshloading THRESHLOADING

/*
 * AUTOSCRAP:
 *   Use the autoscrap code?
 */
#$d_autoscrap AUTOSCRAP

/*
 * SUPERPODS:
 *   Use superpods (cost 9r Success 75%)
 */
#$d_superpod SUPER_PODS 1
#$d_normalpod NORMAL_PODS 1

#ifndef SUPER_PODS
#define SUPER_PODS 0
#endif

#ifndef NORMAL_PODS
#define NORMAL_PODS 0
#endif

/*
 * SPORE_SUCCESS_RATE:
 *   Chance of spore survival. (Was in tweakables.h before)
 */
#define SPORE_SUCCESS_RATE $spore_success_rate
#define NORMAL_SUCCESS_RATE $normal_succesS_rate

/*
 * USE_VN:
 *   Use Von Neumann Machines?
 */
#$d_vn USE_VN

/*
 * USE_AMOEBA:
 *   Use the Dreaded Space Amoeba?
 */
#$d_amoeba USE_AMOEBA

/*
 * COLLECTIVE_MONEY
 *   Have all the governor's money pooled into one total for the race
 */
#$d_money COLLECTIVE_MONEY

/*
 * SHOW_COWARDS:
 *   Have "And # comwards." show up at the bottom of "who"
 */
#$d_cowards SHOW_COWARDS

/*
 * DISSOLVE:
 *   If you want to allow players to dissolve mid-game.
 *   CAUTION: THIS CAN CORRUPT THE DATABASE
 */
#$d_dissolve DISSOLVE

/*
 * STARTING_INVENTORY:
 *   If you want races to start with res/def/fuel and money
 */
#$d_starting_inventory STARTING_INVENTORY

/*
 * DEFAULT_CHAT:
 *   Select either NO_CHAT, TRANS_CHAT, or FREE_CHAT (see vars.h)
 */
#define DEFAULT_CHAT $d_garble_chat

/*
 * TAX_PROTEST:
 *   With this defined, population may protest due to high taxes, morale can be
 *   affected as well.
 */
#$d_tax_protest TAX_PROTEST 1

/*
 * MULTIPLE_COMM_CHANNELS:
 *   Gives GB 3 distinct comm channels. Mainly this is used to move traffic off
 *   general broadcast.
 */
#$d_comm_channels MULTIPLE_COMM_CHANNELS

/*
 * SAVE_LOGFILES:
 *   If you want the server to compress and save log files
 */
#$d_save_logfiles SAVE_LOGFILES

/*
 * MAX_LOGSIZE:
 *   Size in bytes of the log when it truncates
 */
#define MAX_LOGSIZE $maxlogsize

/*
 * I_TIME:
 *   This symbol is defined if the program should include <time.h>
 */
#$i_time I_TIME

/*
 * I_SYSTIME:
 *   this symbol is defined if the program should include <sys/time.h>
 */
#i_systime I_SYSTIME

/*
 * I_SYSTIMEKERNEL:
 *   This symbol is defined if the program should include <sys/time.h> with
 *   KERNEL defined.
 */
#$d_systimekernel SYSTIMEKERNEL

/*
 * CONFIGURE_DATE:
 *   This symbol contains the last data that configure was run for GB -v output
 */
#define CONFIGURE_DATE "$c_date"
#define VERS "$c_date"

/*
 * XENIX:
 *   this symbol, if defined, indicates this is a Xenix system, for knocking out
 *   the far keyword in selected places.
 */
#$d_xenix XENIX

/*
 * BSD:
 *   This symbol, if defined, indicates this is a BSD type system
 */
#$d_bsd BSD

/* #define MAILPROG "/usr/bin/mailx" */
#define MAILPROG "/bin/mail"

/*
 * ------------------------------------------------
 * Below are the variables not changed by configure
 *   You should not modify these unless you know
 *             what you are doing!!
 * ------------------------------------------------
 */

/*
 * MAX_OUTPUT:
 *   ???
 */
#define MAX_OUTPUT 32768

/*
 * QUIT_COMMAND:
 * WHO_COMMAND:
 * HELP_COMMAND:
 * EMULATE_COMMAND:
 */
#define QUIT_COMMAND "quit"
#define WHO_COMMAND "who"
#define HELP_COMMAND "help"
#define EMULATE_COMMAND "emulate"

/*
 * LEAVE_MESSAGE:
 */
#define LEAVE_MESSAGE "\n*** Thank you for playing Galactic Bloodshed ***\n"

/*
 * DEBUG:
 *   Uncomment if you want to use memory debugging
 */
/* #define DEBUG */

/*
 * MARKET:
 *   Comment out if you don't want the market
 */
#define MARKET

/*
 * VICTORY:
 *   If you want to use victory conditions
 */
/* #define VICTORY */

/*
 * DEFENSE:
 *   If you want to allow planetary defense
 */
#define DEFENSE

/*
 * VOTING:
 *   If you want to allow player voting
 */
#define VOTING

/*
 * ACCESS_CHECK:
 *   If you want to check address authorization
 */
#define ACCESS_CHECK

/*
 * NOMADS:
 *   If you want to allow min # of sexes to always colonize a sector safely
 */
/* #define NOMADS */

/*
 * MONITOR:
 *   Allows deity to monitor messages etc. (deity can set with 'toggle monitor'
 *   option. I use it to watch battles in progress.
 */
#define MONITOR

/*
 * POD_TERRAFORM:
 *   If pods will terraform sectors they infect
 */
/* #define POD_TERRAFORM */

#define CONFIG_H_INC
#endf
/* END OF CONFIG.H */
!GROK!THIS!
