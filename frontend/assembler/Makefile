PROG=assembler
SRCS=code.c main.o parser.c symbol-table.o
OBJS=code.o main.o parser.o symbol-table.c
CFLAGS=-O0 -g -Wall

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)

code.o: code.c code.h
	$(CC) -c code.c -o code.o

main.o: main.c code.h parser.h symbol-table.h
	$(CC) -c main.c -o main.o

parser.o: parser.c parser.h
	$(CC) -c parser.c -o parser.o

symbol-table.o: symbol-table.c symbol-table.h
	$(CC) -c symbol-table.c -o symbol-table.o

clean:
	rm -rf *.o $(PROG) *~
