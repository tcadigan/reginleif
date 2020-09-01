HDRS = armor.h chase.h command.h daemon.h daemons.h fight.h \
       init.h io.h list.h machdep.h main.h misc.h monsters.h move.h \
       newlevel.h options.h pack.h passages.h potions.h rings.h rip.h \
       rogue.h rooms.h save.h scrolls.h sticks.h things.h weapons.h \
       wizard.h debug.h
OBJS = vers.o armor.o chase.o command.o daemon.o daemons.o fight.o \
       init.o io.o list.o main.o misc.o monsters.o move.o newlevel.o \
       options.o pack.o passages.o potions.o rings.o rip.o rooms.o \
       save.o scrolls.o sticks.o things.o weapons.o wizard.o debug.o
CFILES = vers.c armor.c chase.c command.c daemon.c daemons.c fight.c \
         init.c io.c list.c main.c misc.c monsters.c move.c newlevel.c \
         options.c pack.c passages.c potions.c rings.c rip.c rooms.c \
         save.c scrolls.c sticks.c things.c weapons.c wizard.c debug.c
CFLAGS = -O -g -Wall -Werror
LINUX_LIB = -lncurses -lcrypt -lbsd
MAC_LIB = -lncurses
MISC = Makefile TODO
ARCH = $(shell uname -s)

a.out: $(HDRS) $(OBJS)
ifeq ($(ARCH), Linux)
	$(CC) $(CFLAGS) $(OBJS) $(LINUX_LIB)
else
	$(CC) $(CFLAGS) $(OBJS) $(MAC_LIB)
endif

rogue: a.out
	cp a.out rogue
	strip rogue

main.o rip.o: machdep.h

clean:
	rm -f $(OBJS) rogue a.out

count:
	wc -l $(HDRS) $(CFILES)

cfiles: $(CFILES)
