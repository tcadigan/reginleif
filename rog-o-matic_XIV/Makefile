# Makefile: Rog-O-Matic XIV (CMU) Thu Jul 3 15:23:02 1986 - mlm
# Copyright (C) 2015 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
#
BINARIES= rogomatic player rgmplot datesub histplot gene
BINDIR= ./.rogomatic/bin
PUBDIR= ./.rogomatic/src
CFLAGS= -g -Wall
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
	  io.h things.h survival.h mess.h explore.h arms.h debug.h \
	  ltm.h rooms.h seach.h stats.h termtokens.h learn.h rand.h \
	  main.h replay.h strategy.h tactics.h worth.h rgmplot.h titlepage.h \
	  histplot.h
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
arms.o: arms.c arms.h debug.h globals.h things.h types.h utility.h
	$(CC) -c $(CFLAGS) arms.c
command.o: command.c arms.h database.h debug.h globals.h io.h monsters.h pack.h things.h types.h utility.h command.h
	$(CC) -c $(CFLAGS) command.c
database.o: database.c database.h debug.h globals.h io.h types.h
	$(CC) -c $(CFLAGS) database.c
datesub.c: datesub.l
	lex datesub.l
	mv lex.yy.c datesub.c
datesub.o: datesub.c
	$(CC) -c $(CFLAGS) datesub.c
datesub: datesub.o
	$(CC) $(CFLAGS) -o datesub datesub.o $(LDFLAGS)
	size datesub
debug.o: debug.c debug.h database.h globals.h install.h io.h mess.h monsters.h pack.h survival.h things.h types.h utility.h
	$(CC) -c $(CFLAGS) debug.c
explore.o: explore.c explore.h command.h debug.h globals.h io.h monsters.h rooms.h search.h survival.h things.h types.h
	$(CC) -c $(CFLAGS) explore.c
findscore.o: findscore.c findscore.h install.h utility.h
	$(CC) -c $(CFLAGS) findscore.c
gene.o: gene.c install.h learn.h types.h utility.h
	$(CC) -c $(CFLAGS) gene.c
gene: gene.o learn.o utility.o stats.o rand.o
	$(CC) $(CFLAGS) -o gene gene.o learn.o utility.o stats.o rand.o $(LDFLAGS)
	size gene
histplot.o: histplot.c histplot.h utility.h
	$(CC) -c $(CFLAGS) histplot.c
histplot: histplot.o utility.o
	$(CC) $(CFLAGS) -o histplot histplot.o utility.o $(LDFLAGS)
	size histplot
io.o: io.c io.h arms.h command.h debug.h globals.h install.h ltm.h mess.h monsters.h pack.h rooms.h scorefile.h search.h stats.h termtokens.h things.h types.h utility.h
	$(CC) -c $(CFLAGS) io.c
mess.o: mess.c mess.h arms.h database.h debug.h globals.h io.h ltm.h monsters.h pack.h rooms.h search.h stats.h tactics.h things.h types.h utility.h
	$(CC) -c $(CFLAGS) mess.c
learn.o: learn.c learn.h rand.h stats.h types.h utility.h
	$(CC) -c $(CFLAGS) learn.c
ltm.o: ltm.c ltm.h debug.h globals.h install.h io.h monsters.h stats.h types.h utility.h
	$(CC) -c $(CFLAGS) ltm.c
main.o: main.c main.h arms.h command.h database.h debug.h explore.h globals.h install.h io.h learn.h ltm.h mess.h monsters.h pack.h replay.h rooms.h search.h strategy.h survival.h termtokens.h things.h types.h utility.h
	$(CC) -c $(CFLAGS) main.c
monsters.o: monsters.c monsters.h debug.h globals.h io.h ltm.h types.h utility.h
	$(CC) -c $(CFLAGS) monsters.c
pack.o: pack.c pack.h command.h database.h debug.h globals.h io.h things.h types.h utility.h worth.h
	$(CC) -c $(CFLAGS) pack.c
player: $(OBJS)
	$(CC) $(CFLAGS) -o player $(OBJS) $(LDFLAGS)
	size player
rand.o: rand.c
	$(CC) -c $(CFLAGS) rand.c
replay.o: replay.c types.h globals.h io.h types.h
	$(CC) -c $(CFLAGS) replay.c
rgmplot.o: rgmplot.c rgmplot.h utility.h
	$(CC) -c $(CFLAGS) rgmplot.c
rgmplot: rgmplot.o utility.o
	$(CC) $(CFLAGS) -o rgmplot rgmplot.o utility.o
	size rgmplot
rogomatic: setup.o findscore.o scorefile.o utility.o
	$(CC) $(CFLAGS) -o rogomatic setup.o findscore.o scorefile.o utility.o
	size rogomatic
rooms.o: rooms.c rooms.h command.h debug.h explore.h globals.h io.h monsters.h search.h things.h types.h
	$(CC) -c $(CFLAGS) rooms.c
scorefile.o: scorefile.c types.h globals.h install.h utility.h scorefile.h
	$(CC) -c $(CFLAGS) scorefile.c
search.o: search.c search.h command.h debug.h globals.h io.h things.h types.h
	$(CC) -c $(CFLAGS) search.c
setup.o: setup.c setup.h findscore.h install.h scorefile.h utility.h
	$(CC) -c $(CFLAGS) setup.c
stats.o: stats.c stats.h types.h
	$(CC) -c $(CFLAGS) stats.c
strategy.o: strategy.c strategy.h arms.h command.h debug.h explore.h globals.h install.h io.h monsters.h rooms.h search.h survival.h tactics.h things.h types.h worth.h
	$(CC) -c $() strategy.c
survival.o: survival.c survival.h debug.h globals.h io.h search.h rooms.h tactics.h types.h
	$(CC) -c $(CFLAGS) survival.c
tactics.o: tactics.c tactics.h arms.h command.h database.h debug.h explore.h globals.h io.h install.h rooms.h search.h things.h titlepage.h types.h utility.h
	$(CC) -c $(CFLAGS) tactics.c
things.o: things.c things.h arms.h command.h database.h debug.h globals.h io.h types.h utility.h
	$(CC) -c $(CFLAGS) things.c
titlepage.o: titlepage.c titlepage.h globals.h types.h
	$(CC) -c $(CFLAGS) titlepage.c
utility.o: utility.c utility.h install.h
	$(CC) -c $(CFLAGS) utility.c
worth.o: worth.c worth.h arms.h globals.h things.h types.h utility.h
	$(CC) -c $(CFLAGS) worth.c

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
index: $(SRCS)
	ctags -c $(SRCS) > index
print: $(SRCS)
	@echo $? > printit
dist: $(SRCS) $(HDRS) $(OTHERS) makefile rogomatic.6 README
	rm -f $(PUBDIR)/*
	cp $(SRCS) $(HDRS) $(OTHERS) makefile rogomatic.6 README $(PUBDIR)
	chmod 0444 $(PUBDIR)/*
	du $(PUBDIR)
