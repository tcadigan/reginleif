#!/bin/sh
#
# Galactic Bloodshed backup script.
# this is called via the @backup command.
#
# args: $1 is the data directory
#       $2 is the backup destination
#
# 02/06/05 Changed to do complete backups of Data, News, Tele directories -mfw

GAMEDIR=$1
DATADIR=$2
NEWSDIR=$3
MSGSDIR=$4
BACKDIR=$5

DATE=`date +%m.%d-%H%M`

# If there is no data directory then exit
if [ ! -d $DATADIR ]; then
    exit 1;
fi

# If there is no game directory (dunno how this would happen) then exit
if [ ! -d $GAMEDIR ]; then
    exit 1;
fi

# If there is no backup directory then exit
if [ ! -d $BACKDIR ]; then
    exit 1;
fi

cd $GAMEDIR > /dev/null 2>&1
R1=$?

tar -cf $BACKDIR/backup.$DATE.tar `basename $DATADIR` > /dev/null 2>&1
R2=$?

tar -rf $BACKDIR/backup.$DATE.tar `basename $NEWSDIR` > /dev/null 2>&1
R3=$?

tar -rf $BACKDIR/backup.$DATE.tar `basename $MSGSDIR` > /dev/null 2>&1
R4=$?

compress $BACKDIR/backup.$DATE.tar > /dev/null 2>&1
R5=$?

# Sum the results of the backup commands
RA=`echo "$R1 + $R2 + $R3 + $R4 + $R5" | bc`

# IF it's greater than 0 we return with an error
if [ "$RA" -gt "0" ]; then
    exit 1;
fi

# Otherwise, all is well, exit normally
exit 0
