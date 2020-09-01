# Hack or Quest Makefile

# Make Hack
GAME = hack
CAPGAME = HACK
GAMEDIR = ../tmp
CFLAGS = -O -g -Wall
LDFLAGS = -lcurses -ltermcap -lc -lm
CSOURCES = alloc.c hack.apply.c hack.bones.c hack.c hack.cmdlist.c hack.Decl.c \
           hack.do.c hack.dog.c hack.do_name.c hack.do_wear.c hack.eat.c \
           hack.end.c hack.engrave.c hack.fight.c hack.invent.c \
           hack.lev.c hack.main.c hack.makemon.c hack.mhitu.c hack.mkobj.c \
           hack.mon.c hack.monst.c hack.objnam.c hack.o_init.c hack.options.c \
           hack.pri.c hack.read.c hack.rip.c hack.rumors.c hack.save.c \
           hack.search.c hack.shk.c hack.stat.c hack.steal.c hack.termcap.c \
           hack.timeout.c hack.topl.c hack.track.c hack.trap.c hack.tty.c \
           hack.u_init.c hack.vault.c hack.version.c hack.whatis.c \
           hack.wield.c hack.worm.c hack.worn.c hack.zap.c makedefs.c mklev.c \
           mklv.makemaz.c mklv.shk.c mklv.shknam.c rnd.c savelev.c \

HSOURCES = alloc.h config.h date.h def.edog.h def.eshk.h def.func_tab.h \
           def.gen.h def.monst.h def.objclass.h def.objects.h def.obj.h \
           def.permonst.h def.trap.h def.wseg.h hack.apply.h hack.bones.h \
           hack.dog.h hack.do.h hack.do_name.h hack.do_wear.h hack.eat.h \
           hack.end.h hack.engrave.h hack.fight.h hack.h hack.invent.h \
           hack.lev.h hack.main.h hack.makemon.h hack.mfndpos.h hack.mhitu.h \
           hack.mkobj.h hack.mon.h hack.objnam.h hack.o_init.h hack.onames.h \
           hack.options.h hack.pri.h hack.read.h hack.rip.h hack.rumors.h \
           hack.save.h hack.search.h hack.shk.h hack.stat.h hack.steal.h \
           hack.termcap.h hack.timeout.h hack.topl.h hack.track.h hack.trap.h \
	   hack.tty.h hack.u_init.h hack.vault.h hack.version.h hack.whatis.h \
           hack.wield.h hack.worm.h hack.worn.h hack.zap.h mklev.h makedefs.h \
           mklev.h mklv.makemaz.h mklv.shk.h mklv.shknam.h rnd.h savelev.h \

SOURCES = $(CSOURCES) $(HSOURCES)

AUX = data help hh rumors hack.6 hack.sh

HOBJ = hack.apply.o hack.bones.o hack.o hack.cmdlist.o hack.do.o hack.dog.o \
       hack.do_name.o hack.do_wear.o hack.eat.o hack.end.o hack.engrave.o \
       hack.fight.o hack.invent.o hack.lev.o hack.main.o hack.makemon.o \
       hack.mhitu.o hack.mkobj.o hack.mon.o hack.objnam.o hack.o_init.o \
       hack.options.o hack.pri.o hack.read.o hack.rip.o hack.rumors.o \
       hack.save.o hack.search.o hack.shk.o hack.stat.o hack.steal.o \
       hack.termcap.o hack.timeout.o hack.topl.o hack.track.o hack.trap.o \
       hack.tty.o hack.u_init.o hack.vault.o hack.version.o hack.whatis.o \
       hack.wield.o hack.worm.o hack.worn.o hack.zap.o savelev.o \

GOBJ = alloc.o hack.Decl.o hack.monst.o rnd.o \

all: $(GAME) mklev
	@echo "Made Hack."

$(GAME): $(HOBJ) $(GOBJ) Makefile
	@echo "Loading ..."
	@$(CC) -o $(GAME) $(GOBJ) $(HOBJ) $(LDFLAGS)

makedefs.o: makedefs.c
	$(CC) -c $(CFLAGS) makedefs.c

makedefs: makedefs.o
	$(CC) -o makedefs makedefs.o $(LDFLAGS)

hack.onames.h: makedefs def.objects.h
	./makedefs > hack.onames.h

mklv.mkobj.o: hack.mkobj.c mklev.h hack.onames.h
	rm -f mklv.mkobj.c
	ln hack.mkobj.c mklv.mkobj.c
	$(CC) -c $(CFLAGS) -DMKLEV mklv.mkobj.c
	rm -f mklv.mkobj.c

mklv.makemon.o: hack.makemon.c mklev.h
	rm -f mklv.makemon.c
	ln hack.makemon.c mklv.makemon.c
	$(CC) -c $(CFLAGS) -DMKLEV mklv.makemon.c
	rm -f mklv.makemon.c

mklv.o_init.o: hack.o_init.c def.objects.h hack.onames.h
	rm -f mklv.o_init.c
	ln hack.o_init.c mklv.o_init.c
	$(CC) -c $(CFLAGS) -DMKLEV mklv.o_init.c
	rm -f mklv.o_init.c

mklv.savelev.o: savelev.c
	rm -rf mklv.savelev.o
	ln savelev.c mklv.savelev.c
	$(CC) -c $(CFLAGS) -DMKLEV mklv.savelev.c
	rm -f mklv.savelev.c

mklev.o: mklev.c
	$(CC) -c $(CFLAGS) -DMKLEV mklev.c

MKLOBJ = $(GOBJ) mklv.makemon.o mklv.mkobj.o mklv.o_init.o \
         mklev.o mklv.makemaz.o mklv.savelev.o mklv.shk.o mklv.shknam.o \

mklev: $(MKLOBJ)
	$(CC) -o mklev $(MKLOBJ) $(LDFLAGS)

clean:
	rm -rf $(MKLOBJ) $(HOBJ) makedefs.o $(GAME) mklev makedefs

lint: lint_h lint_m
	@echo "Lint done."

lint_h:
# lint cannot have -p here because (i) capitals are meaningful:
# [Ww]izard, (ii) identifiers may coincide in the first six places:
# doweararm() versus dowearring().
# _flsbuf comes from <stdio.h>, a bug in the system libraries.
	lint -axbh -DLINT $(HACKCSRC) | sed '/_flsbuf/d' > olint_h
	cat olint_h | sed '/never used/d;warning/d'

lint_m:
	lint -axbh -DLINT -DMKLEV mklev.c hack.makemon.c hack.monst.c \
        hack.o_init.c hack.mkobj.c mklv.shk.c mklv.makemaz.c alloc.c \
        rnd.c

print:
	print *.h Makefile $(AUX)
	print mklev.c mklev.*.c rnd.c alloc.c hack.c
	print hack.Decl.c Hack.magic.C hack.[a-m]*.c
	print hack.[n-z]*.c
	print show.c makedefs.c

diff:
	@- for i in $(SOURCES) $(AUX) ; do \
	    echo ; echo diff $D/$$i $$i ; \
        diff $D/$$i $$i ; done

# The distribution directory also contains the empty files
# perm and record, and the informative files Porting and Bugs.
distribution: Makefile
	@- for i in READ_ME $(SOURCES) $(AUX) Makefile date.h hack.onames.h ; do \
        cmp -s $$i $D/$$d || \
        ( echo cp $$i $D ; cp $$i $D ) ; done

install:
	rm -f /usr/games/$(CAPGAME)
	cp $(GAME) /usr/games/$(CAPGAME)
        # chmod 0710 /usr/games/$(CAPGAME)
	chmod 7411 /usr/games/$(CAPGAME)
	rm -f $(GAMEDIR)/mklev
        # chmod 0750 $(GAMEDIR)/mklev
	chmod 0751 $(GAMEDIR)/mklev
	rm -f $(GAMEDIR)/bones*

# For the moment we are lazy and disregard /usr/include files because
# the sources contain them conditionally. Perhaps we should use cpp.
# -e 's,<\(.*\)>,"/usr/include/\1",'
depend:
	for i in ${CSOURCES}; do \
        ( /bin/grep '^#[ ]*include' $$i | sed -n \
        -e '/<.*>/d' \
        -e 's/[^"]*"\([^"]*\)".*/\1/' \
        -e H -e '$$g' -e '$$s/\n/ /g' \
        -e '$$s/.*/'$$i':&/' -e '$$s/\.c:/.o:/p' \
        >> makedep); done
	for i in ${HSOURCES}; do \
        ( /bin/grep '^#[ ]*include' $$i | sed -n \
        -e '/<.*>/d' \
        -e 's/[^"]*"\([^"]*\)".*/\1/' \
        -e H -e '$$g' -e '$$s/\n/ /g' \
        -e '$$s/.*/'$$i':&\
		touch '$$i/p \
		>> makedep); done
	@echo ':/^# DO NOT DELETE THIS LINE/' > eddep
	@echo ':.+2,$$d' >> eddep
	@echo ':$$r makedep' >> eddep
	@echo ':wq' >> eddep
	@echo '' >> eddep
	@cp Makefile Makefile.bak
	vi -s eddep Makefile
	rm eddep makedep
	@echo '' >> Makefile
	@echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	@echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	@echo '# see make depend above' >> Makefile
	- diff Makefile Makefile.bak
	@rm -f Makefile.bak

# DO NOT DELETE THIS LINE

alloc.o: alloc.h hack.pri.h
hack.apply.o: hack.apply.h def.edog.h hack.h hack.apply.h hack.do.h hack.do_name.h hack.invent.h hack.mon.h hack.pri.h hack.topl.h hack.tty.h hack.wield.h rnd.h
hack.bones.o: hack.bones.h hack.h hack.dog.h hack.lev.h hack.makemon.h hack.mkobj.h hack.pri.h hack.stat.h hack.steal.h hack.topl.h rnd.h savelev.h
hack.o: hack.h def.trap.h hack.do_name.h hack.do_wear.h hack.eat.h hack.end.h hack.engrave.h hack.fight.h hack.invent.h hack.mkobj.h hack.mon.h hack.objnam.h hack.pri.h hack.search.h hack.shk.h hack.topl.h hack.trap.h hack.wield.h hack.worm.h rnd.h
hack.cmdlist.o: config.h def.func_tab.h hack.apply.h hack.do.h hack.do_name.h hack.do_wear.h hack.eat.h hack.end.h hack.engrave.h hack.options.h hack.invent.h hack.read.h hack.save.h hack.search.h hack.shk.h hack.topl.h hack.version.h hack.whatis.h hack.wield.h hack.zap.h
hack.Decl.o: def.gen.h def.monst.h def.obj.h hack.h mklev.h
hack.dog.o: hack.dog.h alloc.h def.edog.h hack.h hack.do_name.h hack.fight.h hack.invent.h hack.makemon.h hack.mfndpos.h hack.mon.h hack.objnam.h hack.onames.h hack.pri.h hack.shk.h hack.steal.h hack.topl.h hack.track.h hack.trap.h rnd.h
hack.do_name.o: hack.do_name.h alloc.h hack.h hack.invent.h hack.mon.h hack.objnam.h hack.pri.h hack.shk.h hack.termcap.h hack.topl.h hack.tty.h hack.vault.h hack.worn.h
hack.do_wear.o: hack.do_wear.h hack.h hack.do.h hack.invent.h hack.main.h hack.mon.h hack.objnam.h hack.topl.h hack.trap.h hack.tty.h hack.wield.h hack.worn.h rnd.h
hack.eat.o: hack.eat.h hack.h hack.do.h hack.end.h hack.invent.h hack.objnam.h hack.pri.h hack.rumors.h hack.topl.h rnd.h
hack.end.o: hack.end.h alloc.h hack.h hack.bones.h hack.do_name.h hack.invent.h hack.main.h hack.objnam.h hack.pri.h hack.rip.h hack.shk.h hack.termcap.h hack.topl.h hack.tty.h
hack.engrave.o: hack.engrave.h alloc.h hack.h hack.invent.h hack.lev.h hack.main.h hack.objnam.h hack.topl.h hack.tty.h rnd.h savelev.h
hack.fight.o: hack.fight.h hack.h hack.do_name.h hack.end.h hack.invent.h hack.mkobj.h hack.mon.h hack.objnam.h hack.pri.h hack.search.h hack.shk.h hack.topl.h hack.worn.h hack.zap.h rnd.h
hack.invent.o: hack.invent.h alloc.h hack.h hack.do.h hack.mkobj.h hack.objnam.h hack.pri.h hack.shk.h hack.termcap.h hack.topl.h hack.tty.h hack.wield.h hack.worn.h def.wseg.h
hack.main.o: hack.main.h hack.h hack.dog.h hack.do.h hack.do_wear.h hack.eat.h hack.end.h hack.engrave.h hack.lev.h hack.makemon.h hack.mon.h hack.o_init.h hack.pri.h hack.save.h hack.search.h hack.shk.h hack.stat.h hack.termcap.h hack.timeout.h hack.topl.h hack.track.h hack.trap.h hack.tty.h hack.u_init.h hack.vault.h rnd.h
hack.makemon.o: hack.makemon.h alloc.h hack.invent.h hack.mon.h hack.worm.h rnd.h hack.h mklev.h hack.mkobj.h hack.h hack.pri.h
hack.mhitu.o: hack.mhitu.h hack.h hack.do_name.h hack.end.h hack.fight.h hack.makemon.h hack.mon.h hack.steal.h hack.topl.h hack.worm.h hack.zap.h rnd.h
hack.mkobj.o: hack.mkobj.h alloc.h hack.invent.h hack.onames.h hack.o_init.h rnd.h mklev.h hack.h
hack.mon.o: hack.mon.h hack.h hack.dog.h hack.do_name.h hack.end.h hack.engrave.h hack.fight.h hack.invent.h hack.makemon.h hack.mfndpos.h hack.mhitu.h hack.mkobj.h hack.pri.h hack.shk.h hack.steal.h hack.topl.h hack.track.h hack.trap.h hack.vault.h hack.worm.h hack.zap.h rnd.h
hack.monst.o: mklev.h def.eshk.h
hack.objnam.o: hack.objnam.h hack.h hack.end.h hack.invent.h hack.mkobj.h hack.o_init.h hack.topl.h rnd.h
hack.o_init.o: hack.o_init.h alloc.h config.h def.objects.h hack.lev.h hack.main.h hack.onames.h hack.pri.h savelev.h rnd.h
hack.options.o: hack.options.h config.h hack.topl.h hack.tty.h hack.h
hack.pri.o: hack.pri.h hack.h hack.end.h hack.invent.h hack.termcap.h hack.topl.h hack.worm.h def.wseg.h
hack.read.o: hack.read.h hack.h hack.do.h hack.dog.h hack.do_name.h hack.do_wear.h hack.end.h hack.invent.h hack.main.h hack.makemon.h hack.mkobj.h hack.mon.h hack.pri.h hack.trap.h hack.topl.h hack.tty.h hack.wield.h hack.worn.h rnd.h
hack.rumors.o: hack.rumors.h alloc.h config.h hack.topl.h mklev.h rnd.h
hack.save.o: hack.save.h alloc.h hack.h hack.end.h hack.lev.h hack.main.h hack.o_init.h hack.pri.h hack.topl.h hack.tty.h hack.worm.h hack.worn.h savelev.h
hack.search.o: hack.search.h def.trap.h hack.h hack.do.h hack.invent.h hack.makemon.h hack.mon.h hack.pri.h hack.topl.h rnd.h
hack.shk.o: hack.shk.h alloc.h def.eshk.h hack.h hack.dog.h hack.do_name.h hack.fight.h hack.invent.h hack.main.h hack.mfndpos.h hack.mon.h hack.objnam.h hack.pri.h hack.topl.h hack.tty.h rnd.h
hack.stat.o: hack.stat.h hack.main.h hack.topl.h mklev.h
hack.steal.o: hack.steal.h hack.h hack.do_name.h hack.do_wear.h hack.invent.h hack.main.h hack.mkobj.h hack.mon.h hack.objnam.h hack.pri.h hack.topl.h hack.wield.h hack.worn.h rnd.h
hack.termcap.o: hack.termcap.h alloc.h config.h hack.main.h hack.topl.h hack.tty.h
hack.timeout.o: hack.timeout.h hack.h hack.end.h hack.pri.h hack.topl.h
hack.topl.o: hack.topl.h alloc.h hack.h hack.end.h hack.mkobj.h hack.pri.h hack.termcap.h hack.tty.h
hack.track.o: hack.track.h hack.h
hack.trap.o: hack.trap.h def.trap.h hack.h hack.do.h hack.dog.h hack.do_name.h hack.end.h hack.engrave.h hack.fight.h hack.invent.h hack.main.h hack.makemon.h hack.mkobj.h hack.mon.h hack.pri.h hack.shk.h hack.topl.h rnd.h
hack.tty.o: hack.tty.h hack.h hack.mkobj.h hack.termcap.h hack.topl.h
hack.u_init.o: hack.u_init.h hack.h hack.do_wear.h hack.eat.h hack.invent.h hack.main.h hack.mkobj.h hack.tty.h hack.wield.h hack.worn.h rnd.h
hack.vault.o: hack.vault.h hack.h hack.dog.h hack.invent.h hack.main.h hack.makemon.h hack.mon.h hack.pri.h hack.topl.h hack.trap.h hack.tty.h
hack.version.o: hack.version.h date.h hack.topl.h
hack.whatis.o: hack.whatis.h hack.h hack.main.h hack.pri.h hack.topl.h hack.tty.h
hack.wield.o: hack.wield.h hack.h hack.do.h hack.invent.h hack.objnam.h hack.topl.h hack.worn.h rnd.h
hack.worm.o: hack.worm.h alloc.h hack.h hack.fight.h hack.mon.h hack.pri.h hack.topl.h rnd.h
hack.worn.o: hack.worn.h hack.h hack.main.h hack.topl.h
hack.zap.o: hack.zap.h hack.h hack.do.h hack.do_name.h hack.fight.h hack.invent.h hack.main.h hack.makemon.h hack.mon.h hack.objnam.h hack.pri.h hack.read.h hack.search.h hack.topl.h hack.tty.h rnd.h
mklev.o: mklev.h alloc.h def.trap.h hack.makemon.h hack.mkobj.h hack.o_init.h mklev.h mklv.makemaz.h mklv.shk.h rnd.h savelev.h
mklv.makemaz.o: mklv.makemaz.h hack.makemon.h hack.mkobj.h rnd.h
mklv.shk.o: mklv.shk.h def.eshk.h hack.makemon.h hack.mkobj.h mklev.h mklv.shknam.h rnd.h
mklv.shknam.o: mklv.shknam.h mklev.h
savelev.o: savelev.h mklev.h def.wseg.h hack.h hack.engrave.h
def.eshk.h: mklev.h
	touch def.eshk.h
def.gen.h: config.h
	touch def.gen.h
def.monst.h: config.h mklev.h
	touch def.monst.h
def.objects.h: def.objclass.h
	touch def.objects.h
def.obj.h: config.h
	touch def.obj.h
hack.apply.h: def.monst.h def.obj.h
	touch hack.apply.h
hack.dog.h: config.h def.monst.h def.obj.h
	touch hack.dog.h
hack.do.h: config.h def.obj.h
	touch hack.do.h
hack.do_name.h: def.monst.h mklev.h
	touch hack.do_name.h
hack.do_wear.h: def.obj.h
	touch hack.do_wear.h
hack.eat.h: def.obj.h
	touch hack.eat.h
hack.end.h: def.monst.h
	touch hack.end.h
hack.engrave.h: config.h
	touch hack.engrave.h
hack.fight.h: config.h def.monst.h def.obj.h
	touch hack.fight.h
hack.h: config.h def.monst.h def.obj.h mklev.h hack.onames.h
	touch hack.h
hack.invent.h: def.gen.h def.monst.h def.obj.h
	touch hack.invent.h
hack.makemon.h: config.h def.monst.h def.permonst.h mklev.h
	touch hack.makemon.h
hack.mhitu.h: def.monst.h
	touch hack.mhitu.h
hack.mkobj.h: def.obj.h
	touch hack.mkobj.h
hack.mon.h: def.monst.h def.permonst.h mklev.h
	touch hack.mon.h
hack.objnam.h: def.obj.h
	touch hack.objnam.h
hack.pri.h: config.h def.monst.h def.obj.h
	touch hack.pri.h
hack.read.h: config.h def.obj.h
	touch hack.read.h
hack.save.h: def.monst.h def.obj.h
	touch hack.save.h
hack.search.h: def.monst.h
	touch hack.search.h
hack.shk.h: def.eshk.h def.obj.h
	touch hack.shk.h
hack.steal.h: def.monst.h def.obj.h
	touch hack.steal.h
hack.track.h: mklev.h
	touch hack.track.h
hack.trap.h: def.gen.h def.monst.h
	touch hack.trap.h
hack.tty.h: config.h
	touch hack.tty.h
hack.u_init.h: config.h
	touch hack.u_init.h
hack.wield.h: def.obj.h
	touch hack.wield.h
hack.worm.h: config.h def.monst.h def.wseg.h
	touch hack.worm.h
hack.worn.h: def.obj.h
	touch hack.worn.h
hack.zap.h: config.h def.monst.h
	touch hack.zap.h
mklv.makemaz.h: mklev.h
	touch mklv.makemaz.h
savelev.h: def.gen.h def.monst.h def.obj.h
	touch savelev.h

# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
