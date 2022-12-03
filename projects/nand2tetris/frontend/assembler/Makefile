PROG = assembler
CC = clang
CFLAGS = -Wall
BEAR = bear --append --

.ifdef DISABLE_BEAR
BEAR = asdf
.endif

SRCS = code.c main.c parser.c symbol-table.c
OBJS = ${SRCS:.c=.o}

all: ${PROG}

${PROG}: depend ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} ${OBJS} -o ${.TARGET}

clean:
	rm -rf *~ *.o .depend ${PROG}

depend: ${SRCS}
	rm -rf .depend
	${CC} ${CFLAGS} -MM ${SRCS} > .depend

-include .depend

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}
