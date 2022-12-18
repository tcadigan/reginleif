OMEGA = omega
CRYPT = crypt
DECRYPT = decrypt
CC= clang
CFLAGS = -Wall
LDFLAGS = -lcurses

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

OMEGA_SRCS = oabyss.c oaux1.c oaux2.c oaux3.c o.c ochar.c ocity.c ocom1.c ocom2.c \
             ocom3.c ocountry.c oeffect1.c oeffect2.c oeffect3.c oenv.c oetc.c \
             ofile.c ogen1.c ogen2.c oguild1.c oguild2.c ohouse.c oinv.c oitem.c \
             oitemf1.c oitemf2.c oitemf3.c olev.c ommelee.c ommove.c omon.c omove.c \
             omovef.c omspec.c omstrike.c omtalk.c opriest.c osave.c oscr.c osite1.c \
             osite2.c ospell.c otime.c otrap.c outil.c ovillage.c
OMEGA_OBJS = ${OMEGA_SRCS:.c=.o}

CRYPT_SRCS = crypt.c
CRYPT_OBJS = ${CRYPT_SRC:.c=.o}

DECRYPT_SRCS = decrypt.c
DECRYPT_OBJS = ${DECRYPT_SRC:.c=.o}

all: ${OMEGA} ${CRYPT} ${DECRYPT}

${OMEGA}: depend ${OMEGA_OBJS}
	${CC} ${CFLAGS} ${OMEGA_SRCS} -o ${.TARGET} ${LDFLAGS}

${CRYPT}: depend ${CRYPT_OBJS}
	${CC} ${CFLAGS} ${CRYPT_SRCS} -o ${.TARGET} ${LDFLAGS}

${DECRYPT}: depend ${DECRYPT_OBJS}
	${CC} ${CFLAGS} ${DECRYPT_SRCS} -o ${.TARGET} ${LDFLAGS}

clean:
	rm -rf *~ *.o .depend ${OMEGA} ${CRYPT} ${DECRYPT}

depend: ${OMEGA_SRCS} ${CRYPT_SRCS} ${DECRYPT_SRCS}
	${CC} ${CFLAGS} -MM ${OMEGA_SRCS} ${CRYPT_SRCS} ${DECRYPT_SRCS} >> .depend

-include .depend

.c.o:
	${BEAR} ${CC} ${CFLAGS} -c ${.IMPSRC} -o ${.TARGET}
