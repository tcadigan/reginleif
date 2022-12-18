PROG = rogue
CC = clang
CFLAGS = -Wall
LDFLAGS = -lcurses -lcrypt

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

SRCS = vers.c armor.c chase.c command.c daemon.c daemons.c fight.c \
       init.c io.c list.c main.c misc.c monsters.c move.c newlevel.c \
       options.c pack.c passages.c potions.c rings.c rip.c rooms.c \
       save.c scrolls.c sticks.c things.c weapons.c wizard.c debug.c
OBJS = ${SRCS:.c=.o}

all: ${PROG}

${PROG}: depend ${OBJS}
	${CC} ${CFLAGS} ${SRCS} -o ${.TARGET} ${LDFLAGS}

clean:
	rm -rf *~ *.o .depend ${PROG}

depend: ${SRCS}
	${CC} ${CFLAGS} -MM ${SRCS} > .depend

-include .depend

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}

# LINUX_LIB = -lncurses -lcrypt -lbsd
