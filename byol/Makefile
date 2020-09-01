PROG = lispy
CC = clang90
BEAR = bear -a

.ifdef DISABLE_BEAR
BEAR =
.endif

SRCS = lispy.c mpc.c
OBJS = ${SRCS:.c=.o}

all: ${PROG}

${PROG}: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} ${OBJS} -o ${PROG}

clean:
	rm -rf *~ *.o ${PROG}

SUFFIXES: .c .o

.c:
	${BEAR} ${CC} ${CFLAGS} ${.IMPSRC} -o ${.TARGET}

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC}
