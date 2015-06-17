# Makefile: Rog-O-Matic XIV (CMU) Thu Jul 3 15:23:02 1986 - mlm
# Copyright (C) 2015 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
#
BINARIES= rogomatic player rgmplot datesub histplot gene
BINDIR= ./.rogomatic/bin
PUBDIR= ./.rogomatic/src
CCFLAGS= -g -Wall
LDFLAGS= -lm -lcurses -ltermcap
OBJS= arms.o command.o database.o debug.o explore.o io.o learn.o \
	  ltm.o main.o mess.o monsters.o pack.o rand.o replay.o rooms.o \
	  scorefile.o search.o stats.o strategy.o survival.o tactics.o \
	  things.o titlepage.o utility.o worth.o
SRCS= arms.c command.c database.c debug.c explore.c io.c learn.c \
	  ltm.c main.c mess.c monsters.c pack.c rand.c replay.c rooms.c \
      scorefile.c search.c stats.c strategy.c survival.c tactics.c \
	  things.c titlepage.c utility.c worth.c
HDRS= types.h globals.h install.h termtokens.h utility.h scorefile.h \
	  setup.h findscore.h command.h monsters.h pack.h database.h \
	  io.h things.h survival.h mess.h explore.h
OTHERS= setup.c fidscore.c datesub.l histplot.c rgmplot.c gene.c \
	    rplot Bugreport

#
# The following commands are declared:
#
all: $(BINARIES)
	echo -n "" > /dev/tty

#
# General makefile stuff:
#
arms.o: arms.h debug.h globals.h things.h types.h utility.h
	$(CC) -c $(CCFLAGS) arms.c
command.o: arms.h database.h debug.h globals.h io.h monsters.h pack.h things.h types.h utility.h command.h
	$(CC) -c $(CCFLAGS) command.c
database.o: database.h debug.h globals.h types.h
	$(CC) -c $(CCFLAGS) database.c
datesub.c: datesub.l
	lex datesub.l
	mv lex.yy.c datesub.c
datesub.o: datesub.c
	$(CC) -c $(CCFLAGS) datesub.c
datesub: datesub.o
	$(CC) $(LDFLAGS) -o datesub datesub.o
debug.o: database.h globals.h install.h io.h mess.h monsters.h pack.h survival.h things.h types.h debug.h
	$(CC) -c $(CCFLAGS) debug.c
explore.o: explore.h command.h debug.h globals.h io.h monsters.h rooms.h search.h survival.h things.h types.h
	$(CC) -c $(CCFLAGS) explore.c
findscore.o: install.h findscore.h utility.h
	$(CC) -c $(CCFLAGS) findscore.c
gene: gene.c rand.o learn.o stats.o utility.o types.h install.h
	$(CC) $(CCFLAGS) -o gene gene.c rand.o learn.o stats.o utility.o $(LDFLAGS)
histplot: histplot.o utility.o
	$(CC) $(LDFLAGS) -o histplot histplot.o utility.o
histplot.o:
	$(CC) -c histplot.c
io.o: types.h globals.h install.h termtokens.h
	$(CC) -c $(CCFLAGS) io.c
mess.o: types.h globals.h
	$(CC) -c $(CCFLAGS) mess.c
learn.o: types.h install.h
	$(CC) -c $(CCFLAGS) learn.c
ltm.o: types.h globals.h install.h
	$(CC) -c $(CCFLAGS) ltm.c
main.o: install.h termtokens.h types.h globals.h
	$(CC) -c $(CCFLAGS) main.c
monsters.o: types.h globals.h
	$(CC) -c $(CCFLAGS) monsters.c
pack.o:type.h globals.h
	$(CC) -c $(CCFLAGS) pack.c
player: $(OBJS)
	$(CC) $(CCFLAGS) -o player $(OBJS) $(LDFLAGS)
	size player
rand.o: rand.c
	$(CC) -c $(CCFLAGS) rand.c
replay.o: types.h globals.h
	$(CC) -c $(CCFLAGS) replay.c
rgmplot.o: rgmplot.c
	$(CC) -c $(CCFLAGS) rgmplot.c
rgmplot: rgmplot.o utility.o
	$(CC) $(CCFLAGS) -o rgmplot rgmplot.o utility.o
rogomatic: setup.o findscore.o scorefile.o utility.o
	$(CC) $(CCFLAGS) -o rogomatic setup.o findscore.o scorefile.o utility.o
	size rogomatic
rooms.o: types.h globals.h
	$(CC) -c $(CCFLAGS) rooms.c
scorefile.o: types.h globals.h install.h utility.h scorefile.h
	$(CC) -c $(CCFLAGS) scorefile.c
search.o: types.h globals.h
	$(CC) -c $(CCFLAGS) search.c
setup.o: install.h utility.h scorefile.h setup.h findscore.h
	$(CC) -c $(CCFLAGS) setup.c
stats.o: types.h
	$(CC) -c $(CCFLAGS) stats.c
strategy.o: types.h globals.h install.h
	$(CC) -c $(CCFLAGS) strategy.c
survival.o: types.h globals.h
	$(CC) -c $(CCFLAGS) survival.c
tactics.o: types.h globals.h install.h
	$(CC) -c $(CCFLAGS) tactics.c
testfind: testfind.o findscore.o utility.o
	$(CC) $(LDFLAGS) -o testfind testfind.o findscore.o utility.o
things.o: types.h globals.h
	$(CC) -c $(CCFLAGS) things.c
titlepage.o: titlepage.c
	$(CC) -c $(CCFLAGS) titlepage.c
titler.o: titler.c
	$(CC) -c titler.c
utility.o: install.h utility.h
	$(CC) -c $(CCFLAGS) utility.c
worth.o: types.h globals.h
	$(CC) -c $(CCFLAGS) worth.c

#
# Miscellaneous useful pseudo-makes
#
backup:
	rm -f backup.tar.gz
	tar czvf backup.tar.gz *.c *.h *.l rogomatic.6 makefile
	chmod ugo-w backup.tar.gz
clean:
	rm -f *.o datesub.c
	rm -rf $(BINARIES)
install:
	strip $(BINARIES)
	rm -f $(BINDIR)/player
	ln player $(BINDIR)/player
	rm -f $(BINDIR)/rogomatic
	ln rogomatic $(BINDIR)/rogomatic
titler: titler.c
	$(CC) -o titler titler.c -lcurses -ltermcap
anim: anim.c utility.o
	$(CC) -o anim anim.c utility.o -lcurses -ltermcap
index: $(SRCS)
	ctags -c $(SRCS) > index
print: $(SRCS)
	@echo $? > printit
dist: $(SRCS) $(HDRS) $(OTHERS) makefile rogomatic.6 README
	rm -f $(PUBDIR)/*
	cp $(SRCS) $(HDRS) $(OTHERS) makefile rogomatic.6 README $(PUBDIR)
	chmod 0444 $(PUBDIR)/*
	du $(PUBDIR)

genetest: genetest.o learn.o rand.o stats.o utility.o types.h
	$(CC) -g -o genetest genetest.o learn.o rand.o stats.o utility.o -lm

gplot: gplot.c
	$(CC) -c gplot gplot.c -lm
