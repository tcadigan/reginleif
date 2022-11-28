# Makefile for gbII. client for Galactic Bloodshed.
#
# Written by Evan D. Koffler <evank@netcom.com>
#
# Copyright (c) 1990-1993
#
# See COPYRIGHT file

FILENAME=gbII
MV = mv -f
RM = rm -f

# Main level files
MAIN = Makefile setup.sh

# Source code files
CFILES = action.c args.c bind.c buffer.c command.c crypt.c csp.c gb.c fuse.c \
         help.c icomm.c imap.c key.c load.c map.c option.c popn.c proc.c \
         psmap.c socket.c status.c stmt.c str.c term.c xmap.c md5.c \
         bind.h command.h csp.h csp_types.h csparr.h option.h str.h \
         term.h types.h vars.h xmap.h

SRCFILES = gb.h.default Makefile.default

# Documentation files
DOCS = BUGREPORT COPYRIGHT README.XMAP Help Help.server History \
       TODO TODO.XMAP CHANGES CHANGES.XMAP client.protocol \
       sample.gbrc sample.macros README

SCRIPTS = atmos.sh domes.sh formers.sh plows.sh vicstat.sh weaps.sh

# Object files
COBJS = action.o args.o bind.o buffer.o command.o crypt.o csp.o gb.o fuse.o \
	help.o icomm.o impa.o key.o load.o map.o option.o popn.o psmap.o \
        proc.o socket.o status.o stmt.o str.o term.o util.o xmap.o md5.o

# Primary target of make
all: source/Makefile Makefile
	(cd source; make)
	mv ./source/${FILENAME} .
	@echo ""
	@echo ""
	@echo ${FILENAME} located in `pwd`
	@echo ""
	@echo ""

# The shell
install: source/${FILENAME}
	cd source; make install

source/${FILENAME}:
	cd source; make ${FILENAME}

static: source/Makefile Makefile
	(cd source; make static)
	mv ./source/${FILENAME} .

# Extras to make my life easier
tar:
	@echo Moving source/gb.h and source/Makefile and gb out of the way
	@echo ""
	chmod a+rx ./setup.sh
	-@${MV} ./source/Makefile ..
	-@${MV} ./source/Makefile~ ..
	-@${MV} ./source/gb.h ..
	-@${MV} ./source/gb.h~ ..
	-@${MV} ./gb ..
	(cd ..; tar -cfFF - gbII | compress > gbII.client.tar.Z)
	ls -l ../gb.client.tar.Z
	@echo Moving source/gb.h and source/Makefile and gb back in.
	-@${MV} ../Makefile ./source/.
	-@${MV} ../Makefile~ ./source/.
	-@${MV} ../gb.h ./source/.
	-@${MV} ../gb.h~ ./source/.
	-@${MV} ../gb .

shar:
	@echo Moving source/gb.h and source/Makefile out of the way
	@echo ""
	-@${MV} ./source/Makefile ..
	-@${MV} ./source/Makefile~ ..
	-@${MV} ./source/gb.h ..
	-@${MV} ./source/gb.h~ ..
	-@${MV} ./gb ..
	(cd ..; shar gbII | compress > gbII.client.shar.Z)
	ls -l ../gbII.client.shar.Z
	@echo Moving source/gb.h and source/Makefile back in.
	@echo ""
	-@${MV} ../Makefile ./source/.
	-@${MV} ../Makefile~ ./source/.
	-@${MV} ../gb.h ./source/.
	-@${MV} ../gb.h~ ./source/.
	-@${MV} ../gb .

post:
	@echo Moving source/gb.h and source/Makefile out of the way
	@echo ""
	-@${MV} ./source/Makefile ..
	-@${MV} ./source/Makefile~ ..
	-@${MV} ./source/gb.h ..
	-@${MV} ./source/gb.h~ ..
	-@${MV} ./gb ..
	(cd ..; tar -cfFF gbII.client.tar gbII)
	ls -l ../gbII.client.tar
	(cd ..; shar gbII > gbII.client.shar)
	ls -l ../gbII.client.shar
	@echo ""
	@echo moving source/gb.h and source/Makefile back in.
	-@${MV} ../Makefile ./source/.
	-@${MV} ../Makefile~ ./source/.
	-@${MV} ../gb.h ./source/.
	-@${MV} ../gb.h~ ./source/.
	-@${MV} ../gb .

clean:
	(cd source; ${RM} ${COBJS} gbII gb.h~ Makefile~)
	${RM} tags gbII

clear: clean
	(cd source; ${RM} gb.h Makefile)

ultra:
	(cd source; ${RM} ${COBJS} ${CFILES} ${SRCFILES} Makefile \
         Makefile~ gb.h gb.h~ gbz bgII)
	(cd docs; ${RM} ${DOCS})
	(cd scripts; ${RM} ${SCRIPTS})
	${RM} ${MAIN}

count:
	wc -l ${MAIN}
	(cd docs; wc -l ${DOCS})
	(cd source; wc -l ${CFILES} ${SRCFILES})
	(cd scripts; wc -l ${SCRIPTS})

ctags:
	ctags source/*.[hc]
