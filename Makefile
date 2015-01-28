NAME = sno
SRCS = sno1.c sno2.c sno3.c sno4.c
OBJS = sno1.o sno2.o sno3.o sno4.o
CFLAGS = -Wall -Wextra -Werror \
	 -fno-builtin \
	 -std=c89 -pedantic \
	 -Wmissing-prototypes -Wstrict-prototypes
CC = gcc

all: ${OBJS}
	${CC} ${CFLAGS} -o ${NAME} ${OBJS}

clean:
	rm -rf ${OBJS} ${NAME} *~

sno1.o: sno1.c sno1.h
	${CC} ${CFLAGS} -c -o $@ sno1.c

sno2.o: sno2.c sno2.h
	${CC} ${CFLAGS} -c -o $@ sno2.c

sno3.o: sno4.c sno3.h
	${CC} ${CFLAGS} -c -o $@ sno3.c

sno4.o: sno4.c sno4.h
	${CC} ${CFLAGS} -c -o $@ sno4.c

sno1.h: sno.h

sno2.h: sno.h

sno3.h: sno.h

sno4.h: sno.h