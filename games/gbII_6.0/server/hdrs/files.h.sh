#!/usr/bin/sh

case $CONFIG in
    '')
        if test ! -f config.sh; then
            ln ../config.sh . || \
                ln ../../config.sh . || \
                ln ../../../config.sh . || \
                (echo "Can't find config.h."; exit 1)
        fi
        . ./config.sh;
        ;;
esac

case "$0" in
    */*)
        cd `expr X$0 : 'X\(.*\)/'`
        ;;
esac

case "$d_sumlink" in
    *define*)
        sln='ln -s'
        ;;
    *)
        sln='ln'
        ;;
esac

echo "Extracting files.h (with variable substitutions)"

cat > files.h <<!GROK!THIS!
/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistributed it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) any
 * later verion.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * Files.h
 * this file was produced by running the files.h.sh script, which gets
 * its values from config.sh, which is generally produced by running
 * Configure.
 *
 * Feel free to modify any of this as the need arises. Note, however,
 * that running files.h.sh again will wipte out any changes you've
 * made. For a more permanent change edit config.h.sh.
 *
 * #ident  "@(#)files.h.sh      1.2 12/15/92 "
 */

#define PATH(file) "${gbpath}/"#file
#define BACK(file) "${gbbackup}/"#file
#define DATA(file) PATH(data/file)
#define NEWS(file) PATH(news/file)
#define MSGS(file) PATH(msgs/file)
#define LOG(file) PATH(logs/file)
#define MISC(file) PATH(misc/file)

#define PATHLEN 200 /* Length of file paths to game... */

#define CUTE_MESSAGE "\nThe Galactic News\n\n"

#define DECLARATION 0 /* News file index */
#define TRANSFER 1
#define COMBAT 2
#define ANNOUNCE 3

#define DATADIR Files[0]
#define HELPDIR Files[1]
#define EXAM_FL Files[2]
#define ENROLL_FL Files[3]
#define STARDATAFL Files[4]
#define SECTORDATAFL Files[5]
#define PLANETDATAFL Files[6]
#define RACEDATAFL Files[7]
#define BLOCKDATAFL Files[8]
#define SHIPDATAFL Files[9]
#define SHIPFREEDATAFL Files[10]
#define DUMMYFL Files[11]
#define PLAYERDATAFL Files[12]
#define MSGSDIR Files[13]
#define TELEGRAMFL Files[14]
#define POWFL Files[15]
#define NEWSDIR Files[16]
#define DECLARATIONFL Files[17]
#define TRANSFERFL Files[18]
#define COMBATFL Files[19]
#define ANNOUNCEFL Files[20]
#define COMMODDATAFL Files[21]
#define COMMODFREEDATAFL Files[22]
#define UPDATEFL Files[23]
#define SEGMENTFL Files[24]
#define ERRLOG Files[25]
#define USLOG Files[26]
#define UPLOG Files[27]
#define ERRLOGDIG Files[28]
#define USLOGDIG Files[29]
#define UPLOGDIG Files[30]
#define BACKUPSCRIPT Files[31]
#define BACKUPDIR Files[32]
#define BACKUPDATA Files[33]
#define WELCOME Files[34]
#define MOTD Files[35]
#define BUGREP Files[36]
#define SCHEDULEFL Files[37]
#define DISPATCHFL Files[38]

#define PLANETLIST PATH(misc/planet.list)
#define STARLIST PATH(misc/star.list)
#define GARBLEFILE PATH(misc/garble.dat)

#define NOGOFL PATH(nogo)
#define ACCESSFL PATH(misc/access.ctl)
#define NOVNFL PATH(novn)

extern const char *Files[];
/* END OF FILES.H */
!GROK!THIS!
