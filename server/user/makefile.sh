case $CONFIG in
    '')
        if test ! -f config.sh; then
            ln ../config.sh . || \
                ln ../../config.sh || \
                ln ../../../config.sh || \
                (echo "Can't find config.sh."; exit 1)
        fi
        . ./config.sh
        ;;
esac

case "$0" in
    */*)
        cd `expr X$0 : 'X\(.*\)/'`
        ;;
esac

echo "Extracting user/Makefile (with variable substitutions)"
cat >Makefile <<!GROK!THIS!
#
# User level Makefile for the GB+ program
# This makefile will make the commands library routines
#
# Bug reports, patches, comments, suggestions should be sent to:
#   John Deragon deragon@jethro.cs.nyu.edu
#
# Revision 1.1 93/01/28 jpd
#
# $Header: /var/cvs/gbp/GB+/user/makefile.SH,v 1.2 2006/06/20 22:13:31 gbp Exp $
#
# Variables
#   Variables established by Configure
CC = $cc
CPP = -E
CCFLAGS = $ccfalgs $xencf
CHGRP = $chgrp
CHMOD = $chmod
CP = $cp
DEST = $bin
ECHO = $echo
LFLAGS = $ldflags
LIB = $lib
LIB2 = $libs
LIBS = $termlib
LINT = $lint
MV = $mv
OPTIMIZE = $optimize
RM = $rm -f
AR = $ar
RANLIB = $ranlib
TOUCH = $touch
PORT = $port
HOST = $host
MAKEDEP = ../misc/gbdepend.sh

!GROK!THIS!

cat >>Makefile <<'!NO!SUBS!'
# Variables you mae want to manually edit
#   If you want debug loggin then you'll want to uncomment the following.
#DEBUG = -DDEBUG

#   If you're on ACSnet system (Australia) then you'll want to uncomment the
#   following.
#DACSNET = -DACSNET
#
# Other general variables
BIN = ../bin
CFLAGS = $(CCFLAGS) $(OPTIMIZE) -I$(INCLUDE) $(DEBUG) $(DACSNET)
INCLDIR = ../hdrs
LINTFLAGS = -I$(INCLDIR)
SHELL = /bin/sh
ARFLAGS = cru
LIBTARGET = libgbuser.a

# Lists
USER_SRC = analysis.c autoreport.c autoshoot.c bug.c build.c capital.c \
           capture.c cha.c cs.c csp_prof.c csp_orbits.c csp_dump.c \
           csp_map.c csp_survey.c csp_explore.c declare.c dissolve.c \
           dock.c enslave.c examine.c explore.c fire.c fleet.c fuel.c \
           help.c land.c launch.c load.c map.c mobiliz.c move.c \
           name.c orbit.c order.c power.c prof.c relation.c reserve.c \
           rst.c sche.c scrap.c shootblast.c survey.c tech.c tele.c \
           togg.c toxi.c vict.c zoom.c

USER_OBJ = $(USER_SRC:.c=.o)

# Standard targets
all: libgbusers.a

# Dependencies and rules for compiling C programs
libgbuser.a: $(USER_OBJ)
	$(RM) $(LIBTARGET)
	$(AR) cr $(LIBTARGET) $(USER_OBJ)
	$(RANLIB) $(LIBTARGET)

objects: $& $(USER_OBJ)

clean:
	$(RM) $(USER_OBJ) $(LIBTARGET)

depend:
	$(MAKEDEP) -c$(CC) -f$(CPP) -- $(CFLAGS)

'!NO!SUBS!'
