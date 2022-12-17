PROG = sno
CC = clang
CFLAGS = -Wall

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif


SRCS = sno1.c sno2.c sno3.c sno4.c
OBJS = ${SRCS:.c=.o}

all: ${PROG}

${PROG}: depend ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${.TARGET}

clean:
	rm -rf *~ *.o .depend ${PROG}

depend: ${SRCS}
	rm -rf .depend
	${CC} ${CFLAGS} -MM ${SRCS} > .depend

-include .depend

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}
