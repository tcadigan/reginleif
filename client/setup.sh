#!/bin/sh
#
# setup: A shell script to facilitate the compiling of the client.
#
# format: setup [-h] [-m] [-n]
#         -h causes script to not touch header file
#         -m causes script to not touch the Makefile
#         -n causes script to not touch the header and Makefile (no files)
#
# Written by Evan D. Koffler <evank@netcom.com>
#
# Copyright (c) 1992-1993
#
# See the COPYRIGHT file.

email=evank@netcom.com
author=evank
whoami=`whoami`

if [ "$author" = "$whoami" ]; then
    cdevel=TRUE
    defaultinstall='..'
    defaulthelp='../docs'
    defaultcompiler=gcc
    defaultcompflags='-g -W'
else
    devel=FALSE
    defaultinstall=/usr/local/bin/gbII
    defaulthelp=/usr/games/lib/gbII
    defaultcompiler=gcc
    defaultcompflags='-g -Wall'
fi

defaultx11='/usr/local/X11'

nomake=FALSE
noheader=FALSE

while [ $# -ne 0 ]; do
    case $1 in
        -n)
            nomake=TRUE
            noheader=TRUE
            ;;
        -m)
            nomake=TRUE
            ;;
        -h)
            noheader=TRUE
            ;;
        *)
            ;;
    esac
    shift
done

# Test for the files we need to have, that we are modifying with this
# script. (The Makefile and the header files) Otherwise there is no point to any
# of this.
if [ ! -r ../source/Makefile.default ]; then
    echo "No file: ../source/Makefile.default. Aborting."
    echo ""
    echo ""
    exit 1
fi

if [ ! -r ../source/gb.h.default ]; then
    echo "No file: ../source/gb.h.default. Aborting."
    echo ""
    echo ""
    exit 1
fi

cat - << END_OF_INTRO

 -- Welcome to the Galactic Bloodshed Client II Setup Facility --

 This program will ask you a few questions and then will set up the Makefile and
 gb.h for your specifications. this program should also take care of any machine
 specifics, as well as the options you do and do not want in the client.

 WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING

 The encryption routine included with the client is NOT secure. There exists
 several different cracking programs which do a very accurate job at draking
 crypt keys. I know. I have one. Consider yourself warned, and USE AT YOUR OWN
 RISK

END_OF_INTRO

# Determine if -n or \c is used to suppress newlines for echo (derived from
# Configure script)
echotmp=`(echo "hi there\c" ; echo " ")`

if echo "$echotmp" | grep c > /dev/null 2>&1 ; then
    n='-n'
    c=''
else
    n=''
    c='\c'
fi

rp="[Hit return to continue]"
echo $n "$rp $c"
read choice
echo ""

cat - << MACHINE_LIST

Machine Type:
  1) Linux (default)
  2) 4.3 BSD, Ultrix, SunOS
  3) System V
  4) HP-UX
  5) CTIX (just for you Tribbles)
  6) IRIX (Silicon Graphics)
  7) Cygwin
  If there are any I am missing please let me know what option it should be

MACHINE_LIST

rp="  Your choice ->"
echo $n "$rp $c"
read choice
echo ""

ctix=FALSE
irix=FALSE
sysv=FALSE
hpux=FALSE
linux=FALSE
cygwin=FALSE

termio=FALSE
index=FALSE

x11lib="$defaultx11/lib"
x11inc="$defaultx11/include"

case "$choice" in
    2) # 4.3 BSD, Ultrix, SunOS
        x11lib="/usr/openwin"
        x11inc="/usr/openwin"
        ;;
    3 | [sS][yY][vV]) # System V
        sysv=TRUE
        termio=TRUE
        ;;
    4 | [hH][pP]*[uU][xX]) # HP-UX
        sysv=TRUE
        termio=TRUE
        hpux=TRUE
        x11lib="/usr/lib/X11R5"
        x11inc="/usr/include/X11R5"
        ;;
    5) # CTIX
        sysv=TRUE
        termio=TRUE
        ctix=TRUE
        ;;
    6) # IRIX
        irix=TRUE
        sysv=TRUE
        termio=TRUE
        ;;
    7) # Cygwin under windows
        cygwin=TRUE
        linux=TRUE
        sysv=TRUE
        termio=TRUE
        defaultinstall='..'
        defaulthelp='../docs'
        x11lib="/usr/X11R6/lib"
        x11inc="/usr/X11R6/include"
        ;;
    *) # Linex, RedHat 9 tested
        linux=TRUE
        sysv=TRUE
        termio=TRUE
        x11lib="/usr/X11R6/lib"
        x11inc="/usr/X11R6/include"
        ;;
esac

echo ""

if [ "$termio" = FALSE]; then
    rp='Use termio (as opposed to termcap) (y/n) [n]: '
    echo $n "$rp $c"
    read answer
    echo ""

    if [ -z "$answer" ]; then
        termio=FALSE
    elif [ "$answer" = "y" ]; then
        termio=TRUE
    else
        termio=FALSE
    fi
fi

if [ "$index" = FALSE ]; then
    rp='Use index (as opposed to strchr) (y/n) [n]: '
    echo $n "$rp $c"
    read answer
    echo ""

    if [ -z "$answer" ]; then
        index=FALSE
    elif [ "$answer" = "y" ]; then
        index=TRUE
    else
        index=FALSE
    fi
fi

rp='Do yo uwish to have XMAP (y/n) [n]: '
echo $n "$rp $c"
read answer
echo ""

if [ -z "$answer" ]; then
    xmap=FALSE
elif [ "$answer" = "y" ]; then
    xmap=TRUE
else
    xmap=FALSE
fi

if [ "$xmap" = TRUE ]; then
    echo "X11 seems to be configured as follows:"
    echo "  libraries: $x11lib"
    echo "   includes: $x11inc"
    echo ""

    rp="Is this correct (y/n) [y]: "
    echo $n "$rp $c"
    read answer
    echo ""

    if [ "$answer" = "n" ]; then
        rp="where is Xlib? [$defaultx11/lib]: "
        echo $n "$rp $c"
        read x11lib
        echo ""

        if [ -z "$x11lib" ]; then
            x11lib="$defaultx11/lib"
        fi

        dflt=`expr $x11lib" : '\(.*\)/[^/][^/]*`
        rp="Where is Xinclude? [$dflt/include]: "
        echo $n "$rp $c"
        read x11inc
        echo ""

        if [ -z "x11inc" ]; then
            x11inc="$deflt/include"
        fi
    fi

    xmaplib="s@^XLIB =.*@XLIB = -L$x11lib - -lX11@"
    xmapinc="s@^XINC =.*@XINC = -I$x11inc@"
else
    xmaplib="s@^XLIB =.*@XLIB =@"
    xmapinc="s@^XINC =.*@XINC =@"
fi

echo ""
echo "You may customize the client to include the features you desire"
echo "or you can compile the client with all options."
echo ""
echo ""
echo "Do you wish to compile the standard features. Which are"
echo "{ ARRAY, IMAP, OPTTECH, POPN, SMART_CLIENT } ON and"
rp="{ RESTRICTED_ACCESS, CLIENT_DEVEL } OFF (y/n) [y]: "
echo $n "$rp $c"
read answer
echo ""

imap=FALSE
opttech=FALSE
popn=FALSE
smartclient=FALSE
array=FALSE
restricted=TRUE

if [ -z "$answer" -o "$answer" = "y" -o "$answer" = "Y" ]; then
    imap=TRUE
    opttech=TRUE
    popn=TRUE
    smartclient=TRUE
    array=TRUE
    restricted=FALSE
fi

echo ""

if [ "$imap" = FALSE ]; then
    rp="Do you wish to have IMAP (y/n) [y]: "
    echo $n "$rp $c"
    read answer
    echo ""

    if [ -z "$answer" - o "$answer" = "y" -o "$answer" = "Y" ]; then
        imap=TRUE
    fi
fi

if [ "$opttech" = FALSE ]; then
    rp="Do you wish to have OPTTECH (y/n) [y]: "
    echo $n "$rp $c"
    read answer
    echo ""

    if [ -z "$answer" -o "$answer" = "y" -o "$answer" = "Y" ]; then
        opttech=TRUE
    fi
fi

if [ "$popn" = FALSE ]; then
    rp="Do you wish to have POPN (y/n) [y]: "
    echo $n "$rp $c"
    read answer
    echo ""

    if [ -z "$answer" -o "$answer" = "y" -o "$answer" = "Y" ]; then
        popn=TRUE
    fi
fi

if [ "$smartclient" = FALSE ]; then
    rp="Do you wish to compile with SMART_CLIENT (beta stage) [n]: "
    echo $n "$rp $c"
    read answer
    echo ""

    if [ "$answer" = "y" -o "$answer" = "Y" ]; then
        smartclient=TRUE
    else
        smartclient=FALSE
    fi
fi

if [ "$restricted" = TRUE ]; then
    rp="Use RESTRICTED_ACCESS (don't unless you know what it is) [n]: "
    echo $n "$rp $c"
    read answer
    echo ""

    if [ "$answer" = "y" -o "$answer" = "Y" ]; then
        restricted=TRUE
    else
        restricted=FALSE
    fi
fi

if [ "$array" = FALSE ]; then
    echo "Use ARRAYS which has the nova and mirror array in an orbit"
    rp="output. It is more cosmetic than anything. [y]: "
    echo $n "$rp $c"
    read answer
    echo ""

    if [ "$answer" = "n" -o "$answer" = "N" ]; then
        array=FALSE
    else
        array=TRUE
    fi
fi

if [ "$cdevel" = FALSE ]; then
    echo "Enable code segments under development, this may make your"
    rp="client unstable. Use this only if you're developing. [n]: "
    echo $n "$rp $c"
    read answer
    echo ""

    if [ "$answer" = "y" -o "$answer" = "Y" ]; then
        cdevel=TRUE
    else
        cdevel=FALSE
    fi
fi

echo ""
echo ""
echo ""
rp="What compiler to use? [$defaultcompiler]: "
echo $n "$rp $c"
read temp
echo ""

if [ -z "$temp" ]; then
    temp=$defaultcompiler
fi

compiler="s@^CC =.*@CC = `echo $temp`@"

echo ""
echo ""
echo ""
rp="What compiler flags (-g is debugging, -O is otimize) [$defaultcompflags]: "
echo $n "$rp $c"
read temp
echo ""

if [ -z "$temp" ]; then
    temp="$defaultcompflags"
fi

compflag="s@^CFLAGS =.*@CFLAGS = `echo $temp` \${XINC}@"

echo ""
echo ""
echo ""
rp="Install gbII where? [$defaultinstall]: "
echo $n "$rp $c"
read installpath
echo ""

if [ -z "$installpath" ]; then
    installpath=$defaultinstall
fi

installs="s@^INSTALLNAME = .*@INSTALLNAME = `echo $installpath`@"

rp="Put Help files where? [$defaulthelp]: "
echo $n "$rp $c"
read helppath
echo ""

if [ -z "$helppath" ]; then
    helppath=$defaulthelp
fi

help=$helppath
helpdirs="s@^HELPDIR =.*@HELPDIR = `echo $help`@"
helpc="s@^#define HELP_CLIENT.*@#define HELP_CLIENT \"$help/Help\"@"
helps="s@^#define HELP_SERVER.*@#define HELP_SERVER \"$help/Help.server\"@"

if [ "$cygwin" = "TRUE" ]; then
    gbrcs="s@^#define DEFAULT_GBRC_PATH.*@#define DEFAULT_GBRC_PATH \"gbrc.txt\"@"
fi

echo ""
echo ""
echo "Okay..."
echo "Setting up your options."
echo "Client will be installed in " $installpath
echo "Help files located in " $helppath
echo "And options of:"

# this sets up the sed strings and then does them all!

if [ "$termio" = "TRUE" ]; then
    echo $n "TERMIO $c"
    termios='s@^#define TERMIO@#define TERMIO@'
else
    termios='s@^#define TERMIO@/* #define TERMIO */@'
fi

if [ "sysv" = "TRUE" ]; then
    if [ "$irix" = "TRUE" -o "$hpux" = "TRUE" -o "$linux" = "TRUE" ]; then
        makelib='s/^LIBS =.*/LIBS = -ltermcap -lm/'
    else
        makelib='s/^LIBS =.*/LIBS = -ltermcap -lm -lsocket/'
    fi

    echo $n "SYSV $c"
    sysvs='s@^#define SYSV@#define SYSV@'
else
    sysvs='s@^#define SYSV@/* #define SYSV */@'
    makelib='s/^LIBS =.*/LIBS = -ltermcap -lm/'
fi

if [ "$ctix" = "TRUE" ]; then
    echo $n "CTIX $c"
    ctixs='s@^#define CTIX@#define CTIX@'
else
    ctixs='s@^#define CTIX@/* #define CTIX */@'
fi

if [ "$cygwin" = "TRUE" ]; then
    echo $n "CYGWIN $c"
    cygwins='s@^#define CYGWIN@#define CYGWIN@'
else
    cygwins='s@^#define CYGWIN@/* #define CYGWIN */@'
fi

if [ "$index" = "TRUE" ]; then
    echo $n "INDEX $c"
    indexs='s@^#define USE_INDEX@#define USE_INDEX@'
else
    indexs='s@^#define USE_INDEX@/* #define USE_INDEX */@'
fi

if [ "$xmap" = "TRUE" ]; then
    echo $n "XMAP $c"
    xmaps='s@^#define XMAP@#define XMAP@'
else
    xmaps='s@^#define XMAP@/* #define XMAP */@'
fi

if [ "$imap" = "TRUE" ]; then
    echo $n "IMAP $c"
    imaps='s@^#define IMAP@#define IMAP@'
else
    imaps='s@^#define IMAP@/* #define IMAP */@'
fi

if [ "$array" = "TRUE" ]; then
    echo $n "ARRAY $c"
    arrays='s@^#define ARRAY@#define ARRAY@'
else
    arrays='s@^#define ARRAY@/* #define ARRAY */@'
fi

if [ "$opttech" = "TRUE" ]; then
    echo $n "OPTTECH $c"
    opttechs='s@^#define OPTTECH@#define OPTTECH@'
else
    opttechs='s@^#define OPTTECH@/* #define OPTTECH */@'
fi

if [ "$popn" = "TRUE" ]; then
    echo $n "POPN $c"
    popns='s@^#define POPN@#define POPN@'
else
    popns='s@^#define POPN@/* #define POPN */@'
fi

if [ "$restricted" = "TRUE" ]; then
    echo $n "RESTRICTED_ACCESS $c"
    restricts='s@^#define RESTRICTED_ACCESS@#define RESTRICTED_ACCESS@'
else
    restricts='s@^#define RESTRICTED_ACCESS@/* #define RESTRICTED_ACCESS */@'
fi

if [ "$smartclient" = "TRUE" ]; then
    echo $n "SMART_CLIENT $c"
    sclients='s@^#define SMART_CLIENT@#define SMART_CLIENT@'
else
    sclients='s@^#define SMART_CLIENT@/* #define SMART_CLIENT */@'
fi

if [ "$cdevel" = "TRUE" ]; then
    sdevel='s@^#define CLIENT_DEVEL@#define CLIENT_DEVEL@'
else
    sdevel='s@^#define CLIENT_DEVEL@/* #define CLIENT_DEVEL */@'
fi

echo ""
echo ""

# Move files to prepare for us setting them up
if [ -f ../source/Makefile ] && [ "$nomake" = FALSE ]; then
    echo "Moving old ../source/Makefile to ../source/Makefile~"
    mv ../source/Makefile ../source/Makefile~
fi

if [ -f ../source/gb.h ] && [ "$noheader" = FALSE ]; then
    echo "Moving old ../source/gb.h to ../source/gb.h~"
    mv ../source/gb.h ../source/gb.h~
fi

echo ""
echo ""

if [ "$nomake" = FALSE]; then
    echo "Preparing ../source/Makefile..."
    sed -e "$makelib
            $compiler
            $compflags
            $installs
            $helpdirs
            $xmaplib
            $xmapinc" ../source/Makefile.default > ../source/Makefile
fi

echo "Done with ../source/Makefile."

if [ "$noheader" = FALSE ]; then
    echo "Preparing ../source/gb.h..."
    sed -e "$termios
            $sysvs
            $ctixs
            $cygwins
            $indexs
            $arrays
            $xmaps
            $imaps
            $opttechs
            $popns
            $restricts
            $helpc
            $helps
            $gbrcs
            $sclients
            $sdevel" ../source/gb.h.default > ../source/gb.h
fi

echo "Done with ../source/gb.h."

echo ""
echo ""
rp='Do you wish to compile the client now (y/n) [y]: '
echo $n "$rp $c"
read answer
echo ""

if [ -z "$answer" ]; then
    compile=TRUE
else
    if [ "$answer" = y ]; then
        compile=TRUE
    else
        compile=FALSE
    fi
fi

if [ "$compile" = TRUE ]; then
    echo ""
    echo ""
    echo "Making the client. Please wait..."
    echo ""

    # Exit status is 0 if okay, 1 if not
    make

    if [ $? -ne 0 ]; then
        makestatus=FALSE
        echo ""
        echo ""
        echo "A problem hsa occurred in making the client."
        echo "Please report any problems or fixes to the author."
        echo "Email address is: $email"
        echo ""
        echo ""
        echo $n "[Hit return to continue]$c"
        read answer
        echo ""
    else
        makestatus=TRUE
    fi
fi

echo ""
echo ""
echo ""

if [ "$makestatus" = TRUE ]; then
    rp="Install the client and help files? [y]: "
    echo $n "$rp $c"
    read answer
    echo ""

    if [ -z "$answer" -o "$answer" = "y" -o "$answer" = "Y" ]; then
        echo "Installing the client and help files. Please wait..."
        make install
    fi
fi

cat - << END_COMMENT

 -- Thank you for using the Galactic Bloodshed Client II Setup Facility --

 It is suggested you look in the directory docs.
 Help documentation, sample default/init file (sample.gbrc), and
 examples of macros (sample.macros) are located in that directory.

 Any suggestions for improvements, contact Evan Koffler:
 Email: $email
 HAP/GB Games: Keeshans

END_COMMENT
