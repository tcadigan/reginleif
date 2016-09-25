PROG=VM
SRCS=code-writer.c main.c parser.c
OBJS=code-writer.o main.o parser.o
CFLAGS=-O0 -g -Wall

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)

code-writer.o: code-writer.c code-writer.h
	$(CC) -c code-writer.c -o code-writer.o

main.o: main.c code-writer.h parser.h
	$(CC) -c main.c -o main.o

parser.o: parser.c parser.h
	$(CC) -c parser.c -o parser.o

clean:
	rm -rf *.o $(PROG) *~
