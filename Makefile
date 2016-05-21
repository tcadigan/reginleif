GAME = Omega
CFLAGS = -O -g -Wall
LDFLAGS = -lcurses

SRCS = oabyss.c oaux1.c oaux2.c oaux3.c o.c ochar.c ocity.c ocom1.c ocom2.c \
       ocom3.c ocountry.c oeffect1.c oeffect2.c oeffect3.c oenv.c oetc.c \
       ofile.c ogen1.c ogen2.c oguild1.c oguild2.c ohouse.c oinv.c oitem.c \
       oitemf1.c oitemf2.c oitemf3.c olev.c ommelee.c ommove.c omon.c omove.c \
       omovef.c omspec.c omstrike.c omtalk.c opriest.c osave.c oscr.c osite1.c \
       osite2.c ospell.c otime.c otrap.c outil.c ovillage.c 

OBJS = oabyss.o oaux1.o oaux2.o oaux3.o o.o ochar.o ocity.o ocom1.o ocom2.o \
       ocom3.o ocountry.o oeffect1.o oeffect2.o oeffect3.o oenv.o oetc.o \
       ofile.o ogen1.o ogen2.o oguild1.o oguild2.o ohouse.o oinv.o oitem.o \
       oitemf1.o oitemf2.o otiemf3.o olev.o ommelee.o ommove.o omon.o omove.o \
       omovef.o omspec.o omstrike.o omtalk.o opriest.o osave.o oscr.o osite1.o \
       osite2.o ospell.o otime.o otrap.o outil.o ovillage.o

HDRS = oabyss.h oaux1.h oaux2.h oaux3.h ochar.h ocity.h ocom3.h ocountry.h \
       odate.h odefs.h oeffect1.h oeffect3.h oenv.h oetc.h ofile.h ogen1.h \
       ogen2.h oglob.h ohouse.h oiinit.h oinv.h oitemf2.h oitem.h olev.h \
       ominit.h ommelee.h ommove.h omon.h oscr.h osite2.h outil.h ovillage.h

all: $(GAME) crypt decrypt

$(GAME): $(OBJS) $(HDRS) Makefile
	$(CC) -o $(GAME) $(OBJS) $(LDFLAGS)

CRYPT_SRC = crypt.c
CRYPT_OBJ = crypt.o

crypt.o: crypt.c
	$(CC) -c $(CFLAGS) crypt.c

crypt: $(CRYPT_OBJ)
	$(CC) -o crypt $(CRYPT_OBJ)

DECRYPT_SRC = decrypt.c
DECRYPT_OBJ = decrypt.o

decrypt.o: decrypt.c
	$(CC) -c $(CFLAGS) decrypt.c

decrypt: $(DECRYPT_OBJ)
	$(CC) -o decrypt $(DECRYPT_OBJ)

clean:
	rm -rf crypt decrypt $(DECRYPT_OBJ) $(CRYPT_OBJ) $(OBJS) $(GAME)

oabyss.o: oabyss.h ogen1.h oglob.h outil.h
oaux1.o: oaux1.h oaux2.h oaux3.h oeffect1.h oeffect3.h oetc.h ogen1.h oglob.h oinv.h olev.h ommove.h omon.h oscr.h outil.h
oaux2.o: oaux2.h oabyss.h oaux1.h ochar.h ocity.h ocom3.h ocountry.h oeffect3.h oenv.h ofile.h ogen1.h ogen2.h oglob.h ohouse.h oinv.h oitem.h oitemf2.h olev.h ommelee.h omon.h oscr.h ovillage.h outil.h
oaux3.o: oaux3.h oaux1.h oaux2.h ochar.h ocity.h ocom2.h ocom3.h oeffect1.h oeffect2.h oeffect3.h oetc.h oglob.h oinv.h oitem.h olev.h omon.h omove.h oscr.h osite1.h outil.h
o.o: o.h oaux2.h ochar.h ocity.h ocom1.h ocom2.h ocountry.h ofile.h ogen1.h oglob.h oitem.h otime.h osave.h oscr.h ospell.h outil.h
ochar.o: ochar.h oaux1.h oaux2.h ocom2.h ofile.h oglob.h oscr.h outil.h
ocity.o: ocity.h ogen1.h oglob.h olev.h omon.h outil.h
ocom1.o: ocom1.h oaux1.h oaux3.h ocom2.h ocom3.h oeffect1.h ofile.h oglob.h oinv.h omovef.h oscr.h outil.h
ocom2.o: ocom2.h oaux1.h oaux2.h oaux3.h oeffect1.h oeffect3.h ogen1.h oglob.h oinv.h oitem.h olev.h omon.h omovef.h osave.h oscr.h osite1.h ospell.h outil.h
ocom3.o: ocom3.h oaux1.h oaux2.h oaux3.h ocom2.h odate.h ofile.h ogen1.h oglob.h oinv.h oitem.h omon.h omovef.h oscr.h osite2.h outil.h
ocountry.o: ocountry.h ogen1.h oglob.h olev.h omon.h oscr.h outil.h
oeffect1.o: oeffect1.h oaux1.h ochar.h oeffect3.h oglob.h oitem.h oinv.h omon.h oscr.h outil.h
oeffect2.o: oeffect2.h oaux1.h oaux2.h oeffect1.h oeffect3.h ogen1.h oglob.h omovef.h oscr.h outil.h
oeffect3.o: oeffect3.h oaux1.h oaux2.h oaux3.h oeffect1.h ogen1.h oglob.h oinv.h oitem.h olev.h omon.h oscr.h ospell.h outil.h
oenv.o: oenv.h ogen1.h oglob.h olev.h omon.h oscr.h outil.h
oetc.o: oetc.h oglob.h oscr.h outil.h
ofile.o: ofile.h oaux1.h ochar.h oglob.h oscr.h outil.h
ogen1.o: ogen1.h oaux1.h ogen2.h oglob.h olev.h oscr.h outil.h
ogen2.o: ogen2.h ogen1.h oglob.h olev.h oscr.h outil.h
oguild1.o: oglob.h
oguild2.o: oglob.h
ohouse.o: ohouse.h oglob.h
oinv.o: oinv.h oglob.h
oitem.o: oitem.h oglob.h
oitemf1.o: oglob.h
oitemf2.o: oitemf2.h oglob.h
oitemf3.o: oglob.h
olev.o: olev.h oglob.h
ommelee.o: ommelee.h oglob.h
ommove.o: ommove.h oglob.h
omon.o: omon.h oglob.h
omove.o: omove.h oglob.h
omovef.o: omovef.h oglob.h
omspec.o: oglob.h
omstrike.o: oglob.h
omtalk.o: oglob.h
opriest.o: oglob.h
osave.o: osave.h oglob.h
oscr.o: oscr.h oglob.h
osite1.o: osite1.h oglob.h
osite2.o: osite2.h oglob.h
ospell.o: oglob.h
otime.o: otime.h oglob.h
otrap.o: oglob.h
outil.o: outil.h oglob.h 
ovillage.o: ovillage.h oglob.h
oaux1.h: odefs.h
oaux2.h: odefs.h
oglob.h: odefs.h
ogen1.h: odefs.h
oinv.h: odefs.h
oitemf2.h: odefs.h
oitem.h: odefs.h
ommelee.h: odefs.h
ommove.h: odefs.h
omon.h: odefs.h
oscr.h: odefs.h
