PLAYER = player
ROGOMATIC = rogomatic
DATESUB = datesub
GENE = gene
HISTPLOT = histplot
RGMPLOT = rgmplot

CC = clang
CFLAGS = -Wall
LDFLAGS = -lm -lcurses -ltermcap # Should really move to terminfo

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

PLAYER_SRCS = arms.c command.c database.c debug.c explore.c io.c learn.c ltm.c \
              main.c mess.c monsters.c pack.c rand.c replay.c rooms.c \
              scorefile.c search.c stats.c strategy.c survival.c tactics.c \
              things.c titlepage.c utility.c worth.c
PLAYER_OBJS = ${PLAYER_SRCS:.c=.o}

DATESUB_SRCS = datesub.c
DATESUB_OBJS = ${DATESUB_SRCS:.c=.o}

GENE_SRCS = gene.c learn.c rand.c stats.c utility.c
GENE_OBJS = ${GENE_SRCS:.c=.o}

HISTPLOT_SRCS = histplot.c utility.c
HISTPLOT_OBJS = ${HISTPLOT_SRCS:.c=.o}

RGMPLOT_SRCS = rgmplot.c utility.c
RGMPLOT_OBJS = ${RGMPLOT_SRCS:.c=.o}

ROGOMATIC_SRCS = findscore.c scorefile.c setup.c utility.c
ROGOMATIC_OBJS = ${ROGOMATIC_SRCS:.c=.o}

all: ${ROGOMATIC} ${DATESUB} ${GENE} ${HISTPLOT} ${PLAYER} ${RGMPLOT}

${ROGOMATIC}: depend ${ROGOMATIC_OBJS}
	${CC} ${CFLAGS} ${ROGOMATIC_OBJS} -o ${.TARGET} ${LDFLAGS}

${DATESUB}: depend ${DATESUB_OBJS}
	${CC} ${CFLAGS} ${DATESUB_OBJS} -o ${.TARGET} ${LDFLAGS}

${GENE}: depend ${GENE_OBJS}
	${CC} ${CFLAGS} ${GENE_OBJS} -o ${.TARGET} ${LDFLAGS}

${HISTPLOT}: depend ${HISTPLOT_OBJS}
	${CC} ${CFLAGS} ${HISTPLOT_OBJS} -o ${.TARGET} ${LDFLAGS}

${PLAYER}: depend ${PLAYER_OBJS}
	${CC} ${CFLAGS} ${PLAYER_OBJS} -o ${.TARGET} ${LDFLAGS}

${RGMPLOT}: depend ${RGMPLOT_OBJS}
	${CC} ${CFLAGS} ${RGMPLOT_OBJS} -o ${.TARGET} ${LDFLAGS}

clean:
	rm -rf *~ *.o .depend ${ROGOMATIC} ${DATESUB} ${GENE} ${HISTPLOT} ${PLAYER} ${RGMPLOT} datesub.c

depend: ${ROGOMATIC_SRCS} ${DATESUB_SRCS} ${GENE_SRCS} ${HISTPLOT_SRCS} ${PLAYER_SRCS} ${RGMPLOT_SRCS}
	rm -rf .depend
	${CC} ${CFLAGS} -MM ${ROGOMATIC_SRCS} ${DATESUB_SRCS} ${GENE_SRCS} ${HISTPLOT_SRCS} ${PLAYER_SRCS} ${RGMPLOT_SRCS} > .depend

-include .depend

datesub.c: datesub.l
	lex datesub.l
	mv lex.yy.c datesub.c

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}
