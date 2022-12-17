PROG = moria
CC = clang
CFLAGS = -Wall
LDFLAGS = -lcurses -lm -ltermcap # Should really move to terminfo

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

SRCS = create.c creature.c death.c desc.c dungeon.c eat.c files.c generate.c \
       help.c io.c magic.c main.c misc1.c misc2.c monsters.c moria1.c moria2.c \
       potions.c prayer.c save.c scrolls.c sets.c signals.c spells.c staffs.c \
       store1.c store2.c treasure1.c treasure2.c variables.c wands.c wizard.c
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
