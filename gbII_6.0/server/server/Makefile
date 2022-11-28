PROG = gbII_server
CFLAGS = -O -g -Wall

SRCS = GB_server.c amoeba.c client.c dispatch.c doplanet.c dosector.c doship.c \
       dospace.c pod.c csp_who.c doturn.c first.c get4args.c getplace.c \
       misc.c moveplanet.c moveship.c schedule.c vn.c

OBJS = GB_server.o amoeba.o client.o dispatch.o doplanet.o dosector.o doship.o \
       dospace.o pod.o csp_who.o doturn.o first.o get4args.o getplace.o \
       misc.o moveplanet.o moveship.o schedule.o vn.o

HDRS = amoeba.h apcosts.h buffers.h client.h command.h config.h csp_dispatch.h \
       csp.h csp_types.h csp_who.h debug.h dispatch.h doplanet.h dosector.h \
       doship.h dospace.h doturn.h files.h files_rw.h files_shl.h first.h \
       fleet.h game_info.h GB_server.h get4args.h getpagesize.h getplace.h \
       idx.h lists.h log.h max.h md5.h misc.h moveplanet.h moveship.h orders.h \
       perm.h pod.h power.h racegen.h races.h rand.h ranks.h schedule.h \
       shipdata.h ships.h shlmisc.h tweakables.h vars.h vn.h

ULIB_SRCS = files.c perm.c rand.c max.c md5.c files_shl.c files_rw.c shlmisc.c

ULIB_OBJS = files.o perm.o rand.o max.o md5.o files_shl.o files_rw.o shlmisc.o

ULIB_HDRS = files.h perm.h rand.h max.h md5.h files_shl.h files_rw.h shlmisc.h

all: ${PROG}

$(PROG): $(OBJS) $(HDRS)
	$(CC) -o $(PROG) $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(OBJS) $(PROG)

GB_server.o: GB_server.h buffers.h command.h csp.h csp_types.h debug.h doturn.h game_info.h power.h races.h ranks.h ships.h tweakables.h vars.h
amoeba.o: amoeba.h bugger.h config.h debug.h power.h races.h ships.h tweakables.h vars.h
client.o: client.h buffers.h csp.h csp_dispatch.h csp_types.h debug.h power.h races.h ships.h vars.h
csp_who.o: csp_who.h buffers.h config.h csp.h csp_types.h debug.h power.h races.h ships.h tweakables.h vars.h
dispatch.o: dispatch.h buffers.h files.h power.h races.h ranks.h ships.h vars.h
doplanet.o: doplanet.h buffers.h doturn.h power.h races.h ships.h tweakables.h vars.h
dosector.o: dosector.h doturn.h power.h races.h ships.h vars.h
doship.o: doship.h buffers.h debug.h doturn.h power.h races.h ships.h vars.h
dospace.o: dospace.h buffers.h doturn.h power.h races.h ships.h tweakables.h vars.h
doturn.o: doturn.h power.h races.h ships.h tweakables.h vars.h
files.o: files.h
files_rw.o: files_rw.h
files_shl.o: files_shl.h
first.o: first.h buffers.h doturn.h power.h races.h ships.h vars.h
get4args.o: get4args.h
getplace.o: getplace.h buffers.h power.h races.h ships.h vars.h
lists.o: lists.h
log.o: log.h
max.o: max.h
md5.o: md5.h
misc.o: misc.h buffers.h debug.h files.h power.h races.h ships.h vars.h
moveplanet.o: moveplanet.h doturn.h power.h races.h ships.h vars.h
moveship.o: moveship.h buffers.h doturn.h power.h races.h ships.h vars.h
perm.o: perm.h
pod.o: pod.h buffers.h doturn.h power.h races.h ships.h vars.h
rand.o: rand.h
schedule.o: schedule.h files.h power.h races.h ships.h vars.h
shlmisc.o: shlmisc.h
vn.o: vn.h buffers.h debug.h doturn.h power.h races.h ships.h vars.h

buffers.h: tweakables.h
