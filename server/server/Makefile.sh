#!/usr/bin/sh

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
        cd `expr X$0 : 'X\(.*\)/'`
        ;;
esac

echo "Extracting server/Makefile (with variable substitutions)"
cat > Makefile <<!GROK!THIS!
#
# User level Makefile for the GB+ program
# This makefile will make the commands library routines
#
# Bug reports, patches, comments, suggestions should be send to:
#   John Deregon deragon@jethro.cs.nyu.edu
#
# Revision 1.8 17:36:23 jpd
#
# $Header: /var/cvs/gbp/GB+/server/Makefile.sh,v 1.4 2007/07/06 17:20:39 gbp Exp $

# Variables
#   Variables established by Configure
CC = $cc
CPP = -E
CCFLAGS = $ccflags $xencf
CHGRP = $chgrp
CHMOD = $chmod
CP = $cp
DEST = $bin
ECHO = $echo
LFALGS = $ldflags
LIB = -L../user -L.
LIBS2 = -lgbuser -lgbserv
LIBS = -ltermcap -lcurses -lm $LIBS
LINT = $lint
MV = $mv
OPTIMIZE = $optimize
RM = $rm -f
AR = $ar
RANLIB = $ranlib
TOUCH = $touch
PORT = $port
HOST = $host
MAKEDEP = ../misc/gbdepend

!GROK!THIS!

cat >> Makefile <<'!NO!SUBS!'
# Variables you may want to manually edit
# If you want debug loggin then you'll want to uncomment the following.
# DEBUG = -DDEBUG

# If you're on an ACSnet system (Australia then you'll want to uncomment the
# following
# DACSNET = -DACSNET

# Other general variables
BIN = ../bin
CFLAGS = $(CCFLAGS $(OPTIMIZE) -I$(INCLDIR) $(DEBUG) $(DACSNET)
INCLDIR = ../hdrs
LINTFLAGS = -I$(INCLDIR)
SHELL = /bin/sh

# Lists
SERVER_SRC = GB_server.c amoeba.c client.c dispatch.c doplanet.c dosector.c \
             doship.c dospace.c pod.c csp_who.c doturn.c first.c get4args.c \
             getplace.c lists.c log.c misc.c moveplanet.c moveship.c \
             schedule.c vn.c

ULIB_SRC = files.c perm.c rand.c max.c md5.c files_shl.c files_rw.c shlmisc.c

SERVER_OBJ = $(SERVER_SRC:.c=.0)
ULIB_OBJ = $(ULIB_SRC:.c=.o

# Standard targets
all: GB_SERVER ligbserv.a

# Dependencies and rules for compile C programs
GB_server: $& $(SERVER_OBJ) libgbserv.a ../user/libgbuser.a
	$(RM) $@
	$(CC) $(LFLAGS) -o $@ $(SERVER_OBJ) $(LIB) $(LIBS2) $(LIBS)

libgbserv.a: $(ULIB_OBJ)
	$(AR) cr $@ $(ULIB_OBJ)
	$(RANLIB) $@

install:
	cp GB_server $(BIN)

clean:
	$(RM) $(SERVER_OBJ) $(ULIB_OBJ) libgbserv.a GB_server

depend:
	$(MAKEDEP) -c$(CC) -f$(CPP) -- $(CFLAGS)
'!NO!SUBS!'
