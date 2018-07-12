PROG = gbII_client
CFLAGS = -O -g -Wall
LDFLAGS = -ltermcap -lm -lcurses

SRCS = action.c args.c arrays.c bind.c buffer.c command.c crypt.c csp.c gb.c \
       fuse.c help.c icomm.c imap.c key.c load.c map.c md5.c option.c popn.c \
       psmap.c proc.c socket.c status.c stmt.c str.c term.c util.c xmap.c

OBJS = action.o args.o arrays.o bind.o buffer.o command.o crypt.o csp.o gb.o \
       fuse.o help.o icomm.o imap.o key.o load.o map.o md5.o option.o popn.o \
       psmap.o proc.o socket.o status.o stmt.o str.o term.o util.o xmap.o

HDRS = action.h ansi.h args.h arrays.h bind.h buffer.h command.h crypt.h \
       csparr.h csp.h csp_types.h fuse.h gb.h help.h icomm.h imap.h key.h \
       load.h map.h option.h popn.h proc.h socket.h status.h stmt.h str.h \
       term.h types.h vars.h widget.h xmap.h

all: ${PROG}

$(PROG): $(OBJS) $(HDRS)
	$(CC) -o $(PROG) $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(OBJS) $(PROG)

action.o: action.h args.h gb.h key.h option.h str.h
args.o: args.h gb.h option.h str.h types.h
arrays.o: arrays.h
bind.o: bind.h gb.h icomm.h imap.h key.h str.h
buffer.o: buffer.h gb.h option.h str.h
command.o: command.h action.h args.h bind.h crypt.h csp.h fuse.h gb.h help.h icomm.h key.h load.h option.h popn.h proc.h psmap.h socket.h status.h stmt.h str.h str.h util.h xmap.h
crypt.o: crypt.h str.h
csp.o: csp.h ansi.h args.h arrays.h csparr.h gb.h icomm.h imap.h key.h map.h option.h popn.h socket.h status.h str.h term.h types.h xmap.h
gb.o: gb.h args.h bind.h command.h crypt.h csp.h icomm.h imap.h key.h load.h option.h proc.h socket.h status.h stmt.h str.h types.h util.h xmap.h
fuse.o: fuse.h csp.h gb.h key.h socket.h str.h types.h
help.o: help.h str.h types.h
icomm.o: icomm.h csp.h gb.h str.h types.h
imap.o: imap.h ansi.h csp.h gb.h key.h str.h socket.h status.h term.h
key.o: key.h args.h bind.h command.h crypt.h gb.h imap.h option.h proc.h socket.h status.h str.h term.h types.h util.h
load.o: load.h gb.h key.h option.h str.h types.h util.h
map.o: map.h ansi.h arrays.h gb.h key.h option.h str.h types.h util.h
option.o: option.h args.h gb.h str.h term.h types.h vars.h
popn.o: popn.h csp.h gb.h str.h term.h types.h vars.h
proc.o: proc.h gb.h str.h types.h vars.h
psmap.o: psmap.h csp.h gb.h types.h vars.h
socket.o: socket.h action.h args.h csp.h gb.h str.h term.h types.h vars.h xmap.h
status.o: status.h csp_types.h gb.h str.h term.h types.h vars.h
stmt.o: stmt.h args.h gb.h str.h types.h vars.h
str.o: str.h action.h ansi.h gb.h term.h types.h vars.h
term.o: term.h gb.h str.h types.h vars.h
util.o: util.h args.h gb.h str.h types.h vars.h
xmap.o: xmap.h arrays.h csp.h gb.h types.h vars.h widget.h

arrays.h: gb.h
buffer.h: types.h
command.h: types.h
crypt.h: types.h
csp.h: types.h
csparr.h: psmap.h
gb.h: ansi.h types.h
imap.h: types.h
map.h: gb.h
option.h: term.h types.h
stmt.h: types.h
types.h: csp_types.h
util.h: types.h
vars.h: types.h
xmap.h: widget.h
