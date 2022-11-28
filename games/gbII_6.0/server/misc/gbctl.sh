#!/bin/sh

case $CONFIG in
    '')
        if test ! -f config.sh; then
            ln ../config.sh . || \
                ln ../../config.sh . || \
                ln ../../../config.sh . || \
                (echo "Can't find config.sh."; exit 1)
        fi

        . ./config.sh
        ;;
esac

case "$0" in
    */*)
        cd `expr X$0 : 'X\(.*\)/`
        ;;
esac

case "$d_symlink" in
    *define*)
        sln='ln -s'
        ;;
    *)
        sln='ln'
        ;;
esac

echo "Extracting gbctl (with variable substitutions)"
case > gbctl.sh <<EOF
#!/bin/sh -
#
# Description: Galactic Bloodshed is a multi-player game of space conquest
#
# Author: Michael Wilkinson <mike@towerravens.com> 10/07/03
#         Developed under Solaris 2.8 and RedHat Fedora Core 4
#
# gbctl: This shell script takes care of starting and stopping the
#        GB, guardian, and racegen daemons.
#
# chkconfig: - 95 30
# processname: gbctl

# Path to the GB directory
GBPATH=${gbpath}

# Set this to '0' for no core files or 'unlimited' to dump core files
CORELIMIT=${corelimit}

# Minimum UID/GID we'll start as
UIDMIN=100
DIGMIN=100

# If started as root this is the user we'll start the daemons as
RUNAS="gbp"

# Names of the daemons we're running
GBDAEMON="GB_server"
GUARDDAEMON="GB_guardian"
RACEDAEMON="GB_racegen"

GBPID="$GBPATH/logs/GB_server.pid"
GUARDPID="$GBPATH/logs/GB_guardian.pid"
RACEPID="$GBPATH/logs/GB_racegen.pid"

# Change to the GB directory
cd $GBPATH

# Add the GB bin path to out PATH variable
PATH="$GBPATH/bin:$PATH"
export PATH

# Set the ulimit for core dumps to 'unlimited' to allow core files
# or set it to '0' to disallow core files
ulimit -c $CORELIMIT

# This script's program name
PROG=`basename $0`

# List of daemons we want to control
DAEMONLIST="$GBDAEMON $GUARDDAEMON $RACEDAEMON"

# List of pid files (guardian pid file must be first)
PIDLIST=`ls -t $GBPATH/logs/*.pid 2>/dev/null | sed -s 's/\n//g'`

# Check if the daemons are there, if not, exit.
for SCRIPT in $DAEMONLIST
do
    if [ ! -f "$GBPATH/bin/$SCRIPT" ]
    then
        echo "$SCRIPT not faound in $GBPATH/bin ... exiting $PROG"
        exit 1
    fi
done

gbstart()
{
    # Make sure this isn't going to run as a privileged user/group
    MYUID=`id | cut -f3 -d'=' | cut -f1 -d'('`
    MYGID=`id | cut -f3 -d'=' | cut -f1 -d'('`

    if [ "$MYUID" -lt "$UIDMIN" -o "$MYGID" -lt "$GIDMIN" ]
    then
        # If we're running as root, change the UID
        if [ "$MYUID" -eq 0 ]
        then
            IMROOT=1
        else
            echo "Cannot run with a UID/GID less than $UIDMIN/$GIDMIN"
            exit
        fi
    fi

    # Check for existing processes and pid files
    for PIDFILE in $PIDLIST
    do
        if [ -r $PIDFILE ]
        then
            PID=`cat $PIDFILE`
            if kill -0 $PID > /dev/null 2>&1
            then
                PIDNAME=`ps -p $PID | grep -v PID | awk '{print $4}'`
                echo "Process $PIDNAME ($PID) seems to be running"
                exit 1
            else
                rm -f $PIDFILE
            fi
        fi
    done

    # Start the Daemons
    for DAEMON in `echo $DAEMONLIST | sed -e "s/$GBDAEMON//g"`
    do
        printf "%-60s" "Starting "`basename $DAEMON`": "

        if [ "$DAEMON" = "$RACEDAEMON" ]
        then
            if [ -n "$IMROOT" ]
            then
                su $RUNAS -c "$DAEMON -s > /dev/nu'' 2>&1"
            else
                $DAEMON -s > /dev/null 2>&1
            fi
        else
            if [ -n "$IMROOT" ]
            then
                su $RUNAS -c "$DAEMON > $GBPATH/logs/$DAEMON.err 2>&1 &"
            else
                $DAEMON > $GBPATH/logs/$DAEMON.err 2>&1 &
            fi
        fi

        RETVAL=$?

        if [ "$RETVAL" -eq "0" ]
        then
            echo "[  OK  ]"
        else
            echo "[FAILED]"
            return 1
        fi
    done
}

gbstop()
{
    for PIDFILE in $PIDLIST
    do
        # Stop daemon
        if test -f "$PIDFILE"
        then
            PID=`cat $PIDFILE`
            PIDNAME=`ps -p $PID | grep -v PID | awk '{print $4}'`
            printf "%-60s" "Shutting down $PIDNAME pid ($PID): "
            kill $PID > /dev/null 2>&1

            RETVAL = $?

            if [ "$RETVAL" -eq "0" ]
            then
                echo "[  OK  ]"
                rm -f $PIDFILE > /dev/null 2>&1
            else
                echo "[FAILED]"
            fi
        fi
    done
}

gbstatus()
{
    for PIDFILE in $PIDLIST
    do
        if [ -f "$PIDFILE" ]
        then
            THISPID=`cat $PIDFILE`
            if [ "$THISPID" -gt "0" ]
            then
                ALLPIDS="$THATPID THISPID"
                THATPID="ALLPIDS"
            fi
        fi
    done

    if [ "$ALLPIDS" ]
    then
        for PID in $ALLPIDS
        do
            THISPID=`ps -p $PID | grep -v PID | awk '{print $1}'`
            THISNAM=`ps -p $PID | grep -v PID | awk '{print $4}'`

            if [ -n "$THISPID" ]
            then
                echo "$THISNAM (pid $THISPID) is running..."
            fi
        done
    fi
}

# See how we were called
case "$1" in
    start)
        gbstart
        ;;
    stop)
        gbstop
        ;;
    restart)
        gbstop
        gbstart
        ;;
    status)
        gbstatus
        ;;
    *)
        echo "Usage: $0 {start|stop|restart|status}"
        exit 1
esac

exit 0
EOF
