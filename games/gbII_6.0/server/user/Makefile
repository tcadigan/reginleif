PROG = gbuser
CC = clang
CFLAGS = -Wall
LDFLAGS = -lcurses -lcrypt

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

SRCS = analysis.c autoreport.c autoshoot.c bug.c build.c capital.c \
       capture.c chan.c cs.c csp_prof.c csp_orbit.c csp_dump.c \
       csp_map.c csp_survey.c csp_explore.c declare.c dissolve.c \
       dock.c enslave.c examine.c explore.c fire.c fleet.c fuel.c \
       help.c land.c launch.c load.c map.c mobiliz.c move.c \
       name.c orbit.c order.c power.c prof.c relation.c reserve.c \
       rst.c sche.c scrap.c shootblast.c survey.c tech.c tele.c \
       togg.c toxi.c vict.c zoom.c
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
