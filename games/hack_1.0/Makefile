# Hack or Quest Makefile

HACK = hack
MAKEDEFS = makedefs
MKLEV = mklev
CC = clang
CFLAGS = -Wall
LDFLAGS = -lm -lcurses -ltermcap # Should really move to terminfo

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

HACK_SRCS = alloc.c hack.apply.c hack.bones.c hack.c hack.cmdlist.c hack.Decl.c \
            hack.do.c hack.dog.c hack.do_name.c hack.do_wear.c hack.eat.c \
            hack.end.c hack.engrave.c hack.fight.c hack.invent.c \
            hack.lev.c hack.main.c hack.makemon.c hack.mhitu.c hack.mkobj.c \
            hack.mon.c hack.monst.c hack.objnam.c hack.o_init.c hack.options.c \
            hack.pri.c hack.read.c hack.rip.c hack.rumors.c hack.save.c \
            hack.search.c hack.shk.c hack.stat.c hack.steal.c hack.termcap.c \
            hack.timeout.c hack.topl.c hack.track.c hack.trap.c hack.tty.c \
            hack.u_init.c hack.vault.c hack.version.c hack.whatis.c \
            hack.wield.c hack.worm.c hack.worn.c hack.zap.c rnd.c savelev.c
HACK_OBJS = ${HACK_SRCS:.c=.o}

MAKEDEFS_SRCS = makedefs.c
MAKEDEFS_OBJS = ${MAKEDEFS_SRCS:.c=.o}

MKLEV_SRCS = alloc.c hack.monst.c rnd.c mklv.makemon.c mklv.mkobj.c \
             mklv.o_init.c mklev.c mklv.makemaz.c mklv.savelev.c mklv.shk.c \
             mklv.shknam.c
MKLEV_OBJS = ${MKLEV_SRCS:.c=.o}

all: ${HACK} ${MAKEDEFS} ${MKLEV}

${HACK}: second-depend ${HACK_OBJS} ${MKLEV}
	${CC} ${CFLAGS} ${HACK_SRCS} -o ${.TARGET} ${LDFLAGS}

${MAKEDEFS}: initial-depend ${MAKEDEFS_OBJS}
	${CC} ${CFLAGS} ${MAKEDEFS_SRCS} -o ${.TARGET} ${LDFLAGS}

${MKLEV}: second-depend ${MKLEV_OBJS}
	echo ${MKLEV_SRCS}
	${CC} ${CFLAGS} ${MKLEV_SRCS} -o ${.TARGET} ${LDFLAGS}

clean:
	rm -rf *~ *.o .depend ${HACK} ${MAKEDEFS} ${MKLEV} hack.onames.h

depend: initial-depend second-depend

initial-depend: ${MAKEDEFS_SRCS}
	rm -rf .depend
	${CC} ${CFLAGS} -MM ${MAKEDEFS_SRCS} > .depend

second-depend: hack.onames.h ${HACK_SRCS} ${MKLEV_SRCS}
	rm -rf .depend
	${CC} ${CFLAGS} -MM ${HACK_SRCS} ${MKLEV_SRCS} >> .depend

-include .depend

hack.onames.h: ${MAKEDEFS} def.objects.h
	./makedefs > hack.onames.h

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}
