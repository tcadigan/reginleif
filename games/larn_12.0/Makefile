#       @(#)Makefile  5.9 (Berkeley) 7/25/90

PROG = larn
CC = clang
CFLAGS = -Wall
LDFLAGS = -lcurses -ltermcap # Should really move to terminfo

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

SRCS = bill.c config.c create.c data.c diag.c display.c fortune.c global.c \
       help.c io.c main.c monster.c moreobj.c movem.c nap.c object.c regen.c \
       savelev.c scores.c signal.c store.c tok.c
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
