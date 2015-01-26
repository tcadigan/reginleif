NAME = sno
SRCS = sno1.c sno2.c sno3.c sno4.c
OBJS = sno1.o sno2.o sno3.o sno4.o
CFLAGS = -Wall -Werror
CC = gcc

all: ${OBJS}
	${CC} ${CFLAGS} -o ${NAME} ${OBJS}

clean:
	rm -rf ${OBJS} ${NAME} *~

%.o: %.c sno.h
	${CC} ${CFLAGS} -c -o $@ $^