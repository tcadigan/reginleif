PROG = lispy
CC = clang
CFLAGS = -Wall

.ifdef DISABLE_BEAR
BEAR = bear --append --
.endif

SRCS = lispy.c mpc.c
OBJS = ${SRCS:.c=.o}

all: ${PROG}

${PROG}: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} ${OBJS} -o ${.TARGET:=}

clean:
	rm -rf *~ *.o ${PROG}

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}
