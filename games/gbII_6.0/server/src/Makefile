GBII = gbII_server
ENROL = enrol
MAKEUNIV = makeuniv
RACEGEN = racegen
PSMAP = psmap
EXSHIP = exship
EXRACE = exrace
MAKELIST = makelist
MAKEINDEX = makeindex
GUARDIAN = guardian
SCOPE = scope
REPAIR = repair

CC = clang
CFLAGS = -Wall
LDFLAGS = -lcurses -lcrypt -lm

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

GBII_SRCS = GB_server.c amoeba.c analysis.c autoreport.c autoshoot.c bug.c \
            build.c capital.c capture.c chan.c client.c cs.c csp_dump.c \
            csp_explore.c csp_map.c csp_orbit.c csp_prof.c csp_survey.c \
            csp_who.c declare.c dispatch.c dissolve.c dock.c doplanet.c \
            dosector.c doship.c dospace.c doturn.c enslave.c examine.c \
            explore.c files.c files_rw.c files_shl.c fire.c first.c fuel.c \
            get4args.c getplace.c help.c land.c launch.c lists.c load.c log.c \
            map.c max.c md5.c misc.c mobiliz.c move.c moveplanet.c moveship.c \
            name.c orbit.c order.c perm.c pod.c prof.c rand.c relation.c \
            reserve.c rst.c sche.c schedule.c scrap.c shlmisc.c shootblast.c \
            survey.c tech.c tele.c togg.c toxi.c user_fleet.c user_power.c \
            vict.c vn.c zoom.c
GBII_OBJS = ${GBII_SRCS:.c=.o}

ENROL_SRCS = enrol.c files.c files_rw.c files_shl.c log.c max.c perm.c rand.c
ENROL_OBJS = ${ENROL_SRCS:.c=.o}

MAKEUNIV_SRCS = files.c makeplanet.c makestar.c makeuniv.c makewormhole.c \
                rand.c
MAKEUNIV_OBJS = ${MAKEUNIV_SRCS:.c=.o}

RACEGEN_SRCS = GB_enroll.c GB_racegen.c enroll.c files.c files_rw.c \
               files_shl.c log.c max.c perm.c rand.c
RACEGEN_OBJS = ${RACEGEN_SRCS:.c=.o}

PSMAP_SRCS = psmap.c
PSMAP_OBJS = ${PSMAP_SRCS:.c=.o}

EXSHIP_SRCS = exship.c files.c
EXSHIP_OBJS = ${EXSHIP_SRCS:.c=.o}

EXRACE_SRCS = exrace.c files.c
EXRACE_OBJS = ${EXRACE_SRCS:.c=.o}

MAKELIST_SRCS = makelist.c
MAKELIST_OBJS = ${MAKELIST_SRCS:.c=.o}

MAKEINDEX_SRCS = makeindex.c
MAKEINDEX_OBJS = ${MAKEINDEX_SRCS:.c=.o}

GUARDIAN_SRCS = GB_guardian.c files.c
GUARDIAN_OBJS = ${GUARDIAN_SRCS:.c=.o}

SCOPE_SRCS = scope.c files.c
SCOPE_OBJS = ${SCOPE_SRCS:.c=.o}

REPAIR_SRCS = repair.c files.c
REPAIR_OBJS = ${REPAIR_SRCS:.c=.o}

all: ${GBII} ${ENROL} ${MAKEUNIV} ${RACEGEN} ${PSMAP} ${EXSHIP} ${EXRACE} ${MAKELIST} ${MAKEINDEX} ${GUARDIAN} ${SCOPE} ${REPAIR}

${GBII}: depend ${GBII_OBJS}
	${CC} ${CFLAGS} ${GBII_SRCS} -o ${.TARGET} ${LDFLAGS}

${ENROL}: depend ${ENROL_OBJS}
	${CC} ${CFLAGS} ${ENROL_SRCS} -o ${.TARGET} ${LDFLAGS}

${MAKEUNIV}: depend ${MAKEUNIV_OBJS}
	${CC} ${CFLAGS} ${MAKEUNIV_SRCS} -o ${.TARGET} ${LDFLAGS}

${RACEGEN}: depend ${RACEGEN_OBJS}
	${CC} ${CFLAGS} ${RACEGEN_SRCS} -o ${.TARGET} ${LDFLAGS}

${PSMAP}: depend ${PSMAP_OBJS}
	${CC} ${CFLAGS} ${PSMAP_SRCS} -o ${.TARGET} ${LDFLAGS}

${EXSHIP}: depend ${EXSHIP_OBJS}
	${CC} ${CFLAGS} ${EXSHIP_SRCS} -o ${.TARGET} ${LDFLAGS}

${EXRACE}: depend ${EXRACE_OBJS}
	${CC} ${CFLAGS} ${EXRACE_SRCS} -o ${.TARGET} ${LDFLAGS}

${MAKELIST}: depend ${MAKELIST_OBJS}
	${CC} ${CFLAGS} ${MAKELIST_SRCS} -o ${.TARGET} ${LDFLAGS}

${MAKEINDEX}: depend ${MAKEINDEX_OBJS}
	${CC} ${CFLAGS} ${MAKEINDEX_SRCS} -o ${.TARGET} ${LDFLAGS}

${GUARDIAN}: depend ${GUARDIAN_OBJS}
	${CC} ${CFLAGS} ${GUARDIAN_SRCS} -o ${.TARGET} ${LDFLAGS}

${SCOPE}: depend ${SCOPE_OBJS}
	${CC} ${CFLAGS} ${SCOPE_SRCS} -o ${.TARGET} ${LDFLAGS}

${REPAIR}: depend ${REPAIR_OBJS}
	${CC} ${CFLAGS} ${REPAIR_SRCS} -o ${.TARGET} ${LDFLAGS}

clean:
	rm -rf *~ *.o .depend ${GBII} ${ENROL} ${MAKEUNIV} ${RACEGEN} ${PSMAP} ${EXSHIP} ${EXRACE} ${MAKELIST} ${MAKEINDEX} ${GUARDIAN} ${SCOPE} ${REPAIR}

depend: ${GBII_SRCS} ${ENROL_SRCS} ${MAKEUNIV_SRCS} ${RACEGEN_SRCS} ${PSMAP_SRCS} ${EXSHIP_SRCS} ${EXRACE_SRCS} ${MAKELIST_SRCS} ${MAKEINDEX_SRCS} ${GUARDIAN_SRCS} ${SCOPE_SRCS} ${REPAIR_SRCS}
	${CC} ${CFLAGS} -MM ${GBII_SRCS} ${MAKEUNIV_SRCS} ${RACEGEN_SRCS} ${PSMAP_SRCS} ${EXSHIP_SRCS} ${EXRACE_SRCS} ${MAKELIST_SRCS} ${MAKEINDEX_SRCS} ${GUARDIAN_SRCS} ${SCOPE_SRCS} ${REPAIR_SRCS} > .depend

-include .depend

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}
