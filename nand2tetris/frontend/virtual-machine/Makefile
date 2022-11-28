PROG = VM
CC = clang90
BEAR = bear -a

.ifdef DISABLE_BEAR
BEAR =
.endif

SRCS = code-writer.c main.c parser.c
OBJS = ${SRCS:.c=.o}

all: ${PROG}

${PROG}: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} ${OBJS} -o ${PROG}

clean:
	rm -rf *~ *.o .depend ${PROG}

depend: ${SRCS}
	rm -rf .depend
	${CC} ${CFLAGS} -MM ${SRCS} > .depend

-include .depend

SUFFIXES: .c .o

.c:
	${BEAR} ${CC} ${CFLAGS} ${.IMPSRC} -o ${.TARGET}

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC}
