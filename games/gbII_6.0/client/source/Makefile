PROG = gbII_client
CC = clang
CFLAGS = -Wall
LDFLAGS = -lm -lcurses -ltermcap # Should really move to terminfo

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

SRCS = action.c args.c arrays.c bind.c buffer.c command.c crypt.c csp.c gb.c \
       fuse.c help.c icomm.c imap.c key.c load.c map.c md5.c option.c popn.c \
       proc.c psmap.c socket.c status.c stmt.c str.c term.c util.c xmap.c
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
