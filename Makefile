# BINDIR is the directory where the moria binary will be put
# LIBDIR is where the other files (scoreboard, news, hours) will be put
# LIBDIR must be the same directory defined in config.h
BINDIR = /usr/public/
LIBDIR = /usr/public/moriadir/

# If you use Gnu C, must add -fwritable-strings option to CFLAGS
CFLAGS = -O -g -Wall
LDFLAGS = -lcurses -ltermcap -lm

SRCS = create.c creature.c death.c desc.c dungeon.c eat.c files.c generate.c \
       help.c io.c magic.c main.c misc1.c misc2.c monsters.c moria1.c moria2.c \
       potions.c prayer.c save.c scrolls.c sets.c signals.c spells.c staffs.c \
       store1.c store2.c treasure1.c treasure2.c variables.c wands.c wizard.c \

OBJS = create.o creature.o death.o desc.o dungeon.o eat.o files.o generate.o \
       help.o io.o magic.o main.o misc1.o misc2.o monsters.o moria1.o moria2.o \
       potions.o prayer.o save.o scrolls.o sets.o signals.o spells.o staffs.o \
       store1.o store2.o treasure1.o treasure2.o variables.o wands.o wizard.o \

HDRS = config.h constants.h desc.h externs.h io.h misc1.h misc2.h moria1.h \
       moria2.h types.h

moria: $(OBJS) $(HDRS)
	$(CC) -o moria $(CFLAGS) $(OBJS) $(LDFLAGS)

lintout: $(SRCS)
	lint $(SRCS) $(LDFLAGS) > lintout

lintout2: $(SRCS)
	lint -bach $(SRCS) $(LDFLAGS) > lintout

tags: $(SRCS)
	ctags -x $(SRCS) > TAGS

clean:
	rm -rf $(OBJS) moria

# You must define BINDIR and LIBDIR before installing
install:
	cp moria $(BINDIR)
	cp Moria_hours $(LIBDIR)
	cp Moria_news $(LIBDIR)
	cp Highscores $(LIBDIR)
	chmod 4511 $(BINDIR)/moria
	chmod 644 $(LIBDIR)/Highscores
	chmod 644 $(LIBDIR)/Moria_news
	chmod 444 $(LIBDIR)/Moria_hours
	chmod 555 $(LIBDIR)

create.o: config.h constants.h externs.h io.h misc1.h misc2.h types.h
creature.o: config.h constants.h desc.h externs.h io.h misc1.h misc2.h moria1.h moria2.h spells.h types.h
death.o: config.h constants.h externs.h types.h
desc.o: config.h constants.h externs.h types.h
dungeon.o: config.h constants.h externs.h types.h
eat.o: config.h constants.h externs.h types.h
files.o: config.h constants.h externs.h types.h
generate.o: config.h constants.h externs.h types.h
help.o: config.h constants.h externs.h types.h
io.o: config.h constants.h externs.h types.h
magic.o: config.h constants.h externs.h types.h
main.o: config.h constants.h externs.h types.h
misc1.o: config.h constants.h externs.h types.h
misc2.o: config.h constants.h externs.h types.h
monsters.o: config.h constants.h types.h
moria1.o: config.h constants.h externs.h types.h
moria2.o: config.h constants.h externs.h types.h
potions.o: config.h constants.h externs.h types.h
prayer.o: config.h constants.h externs.h types.h
save.o: config.h constants.h externs.h types.h
scrolls.o: config.h constants.h externs.h types.h
sets.o: config.h constants.h
signals.o: config.h constants.h externs.h types.h
spells.o: config.h constants.h externs.h types.h
staffs.o: config.h constants.h externs.h types.h
store1.o: config.h constants.h externs.h types.h
store2.o: config.h constants.h externs.h types.h
treasure1.o: config.h constants.h types.h
treasure2.o: config.h constants.h types.h
wands.o: config.h constants.h externs.h types.h
wizard.o: config.h constants.h externs.h types.h
moria1.h: types.h
	touch moria1.h
