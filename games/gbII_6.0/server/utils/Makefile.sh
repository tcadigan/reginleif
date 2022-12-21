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

echo "Extracting utils/Makefile (with variable substitutions"
cat >Makefile <<!GROK!THIS
#
# util level Makefile for the GB+ program
# This Makefile will make the command library routines
#
# Bug reports, patches, comments, suggestions should be sent ot:
#   John Deragon deragon@jethro.cs.nyu.edu
#
# Revision 1.0 92/12/10 jpd
#
# Variables
#   Variables estabilished by Configure
#
CC = $cc
CPP = -E
CCFLAGS = $ccflags $xencf
CHGRP = $chgrp
CHMOD = $chmod
CP = $cp
DEST = $bin
ECHO = $echo
LFLAGS = $ldflags
LIB =
LIBS2 =
LIBS = -lcurses -ltermcap -lm $libs
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

!GROK!THIS

cat >>Makefile <<'!NO!SUBS!'

# Variables you may want to manually edit:
#   If you want debug logging then you'll want to uncomment the following.
##DEBUG = -DDEBUG

#   If you're on an ACSnet system (Australia) then you'll want to uncomment the
#   following.
##DACSNET = -DACSNET

# Other general variables
BIN = ../bin
CFLAGS = $(CCFLAGS) $(OPTIMIZE) -I$(INCLDIR) $(DEBUG) $(DACSNET)
INCLDIR = ../hdrs
LINTFLAGS = -I$(INCLDIR)
SHELL = /bin/sh

# Lists
ENROL_SRC = enrol.c
ENROL_OBJ = $(ENROL_SRC:.c=.o)

MAKEUNIV_SRC = makeuniv.c makestar.c makeplanet.c makewormhole.c
MAKEUNIV_OBJ = $(MAKEUNIV_SRC:.c=.0)

RACEGEN_SRC = GB_racegen.c enroll.c GB_enroll.c
RACEGEN_OBJ = $(RACEGEN_SRC:.c=.o)

PSMAP_SRC = psmap.c
PSMAP_OBJ = $(PSMAP_SRC:.c=.o)

EXSHIP_SRC = exship.c
EXSHIP_OBJ = $(EXSHIP_SRC:.c=.o)

EXRACE_SRC = exrace.c
EXRACE_OBJ = $(EXRACE_SRC:.c=.o)

MAKELIST_SRC = makelist.c
MAKELIST_OBJ = $(MAKELIST_SRC:.c=.o)

MAKEINDEX_SRC = makeindex.c
MAKEINDEX_OBJ = $(MAKEINDEX_SRC:.c=.o)

GUARDIAN_SRC = GB_guardian.c
GUARDIAN_OBJ = $(GUARDIAN_SRC:.c=.o)

# Standard targets
all: makeuniv GB_racegen psmap exship exrace enrol makelist makeindex GB_guardian

# Dependencies and rules for compiling C programs
enrol: $& $(ENROL_OBJ) ../server/libgbserv.a
	$(CC) $(LFLAGS) -o $@ $(ENROL_OBJ) $(LIBS) -L../server -lgbserv

makeuniv: $& $(MAKEUNIV_OBJ) ../server/libgbserv.a
	$(CC) $(LFLAGS) -o $@ $(MAKEUNIV_OBJ) $(LIBS) - L../server -lgbserv

psmap: $& $(PSMAP_OBJ)
	$(CC) $(LFLAGS) -o $@ $(PSMAP_OBJ)

exship: $& $(EXSHIP_OBJ)
	$(CC) $(LFLAGS) -o $@ $(EXSHIP_OBJ)

exrace: $& $(EXRACE_OBJ)
	$(CC) $(LFLAGS) -o $@ $(EXRACE_OBJ)

makelist: $& $(MAKELIST_OBJ)
	$(CC) $(LFLAGS) -o $@ $(MAKELIST_OBJ)

makeindex: $& $(MAKEINDEX_OBJ)
	$(CC) $(LFLAGS) -o $@ $(MAKEINDEX_OBJ)

GB_guardian: $& $(GUARDIAN_OBJ)
	$(CC) $(LFLAGS) -o $@ $(GUARDIAN_OBJ)

GB_racegen.o: GB_racegen.c
	$(CC) $(CFLAGS) -DPRIV -o $@ GB_racegen.c -c

GB_racegen: $(RACEGEN_OBJ) ../server/libgbserv.a
	$(CC) -o GB_racegen $(RACEGEN_OBJ) -lm $(LIBS) -L../server -lgbserv

clean:
	rm -f $(ENROL_OBJ) $(MAKEUNIV_OBJ) $(RACEGEN_OBJ)
	rm -f $(PSMAP_OBJ $(EXSHIP_OBJ) $(EXRACE_OBJ) $(MAKEINDEX_OBJ)
	rm -f $(MAKELIST_OBJ) $(GUARDIAN_OBJ)
	rm -f exship exrace psmap enrol makeuniv makelist makeindex
	rm -f GB_racegen GB_guardian

install:
	cp makeuniv $(BIN)
	cp GB_racegen $(BIN)
	cp psmap $(BIN)
	cp exship $(BIN)
	cp exrace $(BIN)
	cp enrol $(BIN)
	cp makelist $(BIN)
	cp makeindex $(BIN)
	cp GB_guardian $(BIN)

depend:
	$(MAKEDEP) -c$(CC) -f$(CPP) -- $(CFLAGS)
'!NO!SUBS!'
