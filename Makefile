#       @(#)Makefile  5.9 (Berkeley) 7/25/90

# EXTRA
#       Incorporates code to gather additional performance statistics
# SYSV
#       Use system III/V (instead of V7) type ioctl calls
# BSD
#       Use BSD specific features (mostly timer and signal stuff)
# BSD41
#       Use BSD4.1 to avoid some 4.2 dependencies (must be used with BSD above;
#       do not mix with SYSV)
# HIDEBYLINK
#       If defined, the program attempts to hide from ps
# DOCHECKPOINTS
#       If not defined, checkpoint files are periodically written by the Larn
#       process (no forking) if enabled in the .larnopts description
#       file. checkpointing is handy on an unreliable system, but takes
#       CPU. Inclusion of DOCHECKPOINTS will cause fork()-ing to perform the
#       checkpoints (again if enabled in the .larnopts file). This usually
#       avoids pasues in Larn while checkpoints is being done (on large
#       machines).
# VER
#       This is the version of the software, example: 12
# SUBVER
#       This is the revision of the software, example: 1
# FLUSHNO=#
#       Set the input queue excess flushing threshold (default 5)
# NOVARARGS
#       Define for systems that don't have varargs (a default varargs will be
#       used).
# MACRORND
#       Define to use macro version of rnd() and rund() (fast and big)
# UIDSCORE
#       Define to use user id's to manage scoreboard. Leaving this out will
#       cause player id's from the file ".playerids" to be used
#       instead. (.playerids is created upon demand). Only one entry per id #
#       is allowed in each scoreboard (winning and non-winning).
# VT100
#       Compile for using vt100 family of terminals. Omission of this define
#       will cause Larn to use termcap, but it will be MUCH slower due to an
#       extra layer of output interpretation. Also, only VT100 mode allows 2
#       different standout modes, inverse video, and bold video. And only in
#       VT100 mode is the scrolling region of the terminal used (much nicer
#       than insert/delete line sequences to simulate it, if VT100 is omitted).
# NONAP
#       This causes napms() to return immediately instead of delaying n
#       milliseconds. This define may be needed on some systems if the nap
#       stuff does not work correctly (possible hang). nap() is primarily used
#       to delay for effect when casting missile type spells.
# NOLOG
#       Turn off logging.

PROG = larn
CFLAGS = -O -g -Wall -DBSD -DVER=12 -DSUBVER=0 -DNONAP
LDFLAGS = -ltermcap -lcurses
HIDEGAME = hidegame

SRCS = bill.c config.c create.c data.c diag.c display.c fortune.c global.c \
       help.c io.c main.c monster.c moreobj.c movem.c nap.c object.c regen.c \
       savelev.c scores.c signal.c store.c tok.c \

OBJS = bill.o config.o create.o data.o diag.o display.o fortune.o \
       global.o help.o io.o main.o monster.o moreobj.o movem.o nap.o \
       object.o regen.o savelev.o scores.o signal.o store.o tok.o \

HDRS = bill.h create.h diag.h display.h fortune.h global.h header.h help.h \
       io.h main.h monster.h moreobj.h movem.h nap.h object.h pathnames.h \
       regen.h savelev.h scores.h signal.h store.h tok.h

all: $(PROG)

$(PROG): $(OBJS) $(HDRS)
	$(CC) -o $(PROG) $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(OBJS) $(PROG)

beforeinstall:
	(cd ${.CURDIR}/datfiles; install -c o ${BINOWN} -g ${BINGRP} -m 444 \
             larnmaze larnopts lforturne larn.help \
             ${DESTDIR}/usr/share/games/larn)

bill.o: bill.h header.h io.h
config.o: header.h pathnames.h
create.o: create.h global.h header.h io.h monster.h savelev.h scores.h
data.o: header.h
diag.o: diag.h global.h header.h io.h nap.h savelev.h scores.h store.h
display.o: display.h create.h global.h header.h io.h monster.h
fortune.o: fortune.h
global.o: global.h display.h header.h io.h main.h monster.h nap.h scores.h
help.o: help.h display.h io.h
io.o: io.h header.h scores.h
main.o: main.h create.h diag.h display.h fortune.h global.h header.h help.h io.h monster.h movem.h object.h regen.h scores.h signal.h tok.h
monster.o: monster.h create.h display.h global.h header.h io.h main.h nap.h object.h scores.h tok.h
moreobj.o: moreobj.h display.h global.h header.h io.h main.h monster.h object.h scores.h
movem.o: movem.h create.h display.h header.h io.h movem.h monster.h
nap.o: nap.h io.h
object.o: object.h create.h display.h fortune.h global.h io.h main.h monster.h moreobj.h nap.h regen.h scores.h store.h
regen.o: regen.h display.h global.h header.h io.h
savelev.o: savelev.h header.h
scores.o: scores.h bill.h header.h io.h main.h
signal.o: signal.h diag.h display.h global.h header.h io.h scores.h
store.o: store.h display.h global.h header.h io.h main.h nap.h object.h scores.h
tok.o: tok.h diag.h display.h header.h io.h scores.h
