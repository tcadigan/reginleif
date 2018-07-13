PROG = gbII_client
CFLAGS = -O -g -Wall
LDFLAGS = -ltermcap -lm -lcurses

SRCS = action.c args.c arrays.c bind.c buffer.c command.c crypt.c csp.c gb.c \
       fuse.c help.c icomm.c imap.c key.c load.c map.c md5.c option.c popn.c \
       proc.c psmap.c socket.c status.c stmt.c str.c term.c util.c xmap.c

OBJS = action.o args.o arrays.o bind.o buffer.o command.o crypt.o csp.o gb.o \
       fuse.o help.o icomm.o imap.o key.o load.o map.o md5.o option.o popn.o \
       proc.o psmap.o socket.o status.o stmt.o str.o term.o util.o xmap.o

HDRS = action.h ansi.h args.h arrays.h bind.h buffer.h command.h crypt.h \
       csparr.h csp.h csp_types.h fuse.h gb.h help.h icomm.h imap.h key.h \
       load.h map.h md5.h option.h popn.h proc.h psmap.h socket.h status.h \
       stmt.h str.h term.h types.h widget.h xmap.h

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
command.o: command.h action.h args.h bind.h crypt.h csp.h fuse.h gb.h help.h icomm.h key.h load.h option.h popn.h proc.h psmap.h socket.h status.h stmt.h str.h util.h xmap.h
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
md5.o: md5.h
option.o: option.h args.h bind.h key.h socket.h status.h str.h
popn.o: popn.h str.h types.h
proc.o: proc.h crypt.h gb.h str.h types.h util.h
psmap.o: psmap.h csp.h gb.h key.h load.h proc.h str.h socket.h
socket.o: socket.h action.h args.h buffer.h command.h crypt.h csp.h gb.h icomm.h imap.h key.h map.h proc.h option.h status.h str.h types.h util.h
status.o: status.h gb.h key.h option.h str.h term.h types.h
stmt.o: stmt.h args.h gb.h key.h str.h util.h
str.o: str.h action.h bind.h gb.h key.h option.h proc.h socket.h types.h
term.o: term.h bind.h gb.h option.h str.h types.h
util.o: util.h args.h gb.h key.h socket.h str.h types.h
xmap.o: xmap.h arrays.h csp.h gb.h types.h widget.h

arrays.h: gb.h
buffer.h: types.h
command.h: types.h
crypt.h: types.h
csp.h: types.h
csparr.h: psmap.h
gb.h: ansi.h types.h
imap.h: types.h
map.h: gb.h
option.h: gb.h term.h types.h
stmt.h: types.h
types.h: csp_types.h
util.h: types.h
xmap.h: widget.h
