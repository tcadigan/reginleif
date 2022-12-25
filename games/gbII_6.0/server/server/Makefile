PROG = gbII_server
GBSERV = gbserv
CC = clang
CFLAGS = -Wall

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

SRCS = amoeba.c client.c csp_who.c dispatch.c doplanet.c dosector.c doship.c \
       dospace.c doturn.c first.c GB_server.c get4args.c getplace.c lists.c \
       log.c misc.c moveplanet.c moveship.c pod.c schedule.c vn.c
OBJS = ${SRCS:.c=.o}

GBSERV_SRCS = files.c files_rw.c files_shl.c max.c md5.c perm.c rand.c shlmisc.c
GBSERV_OBJS = ${GBSERV_SRCS:.c=.o}

all: ${PROG} ${GBSERV}

${PROG}: depend ${OBJS}
	${CC} ${CFLAGS} ${SRCS} -o ${.TARGET} ${LDFLAGS}

${GBSERV}: depend ${GBSERV_OBJS}
	${CC} ${CFLAGS} ${GBSERV_SRCS} -o ${.TARGET} ${LDFLAGS}

clean:
	rm -rf *~ *.o .depend ${PROG} ${GBSERV}

depend: ${SRCS} ${GBSERV_SRCS}
	${CC} ${CFLAGS} -MM ${SRCS} ${GBSERV_SRCS} > .depend

-include .depend

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}
