PROG = lispy
CC = clang
CFLAGS = -Wall

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

SRCS = lispy.c mpc.c
OBJS = ${SRCS:.c=.o}

all: ${PROG}

${PROG}: depend ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${.TARGET}

clean:
	rm -rf *~ *.o .depend ${PROG}

depend: ${SRCS}
	${CC} ${CFLAGS} -MM ${SRCS} > .depend

-include .depend

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}
