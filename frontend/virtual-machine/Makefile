PROG=VM
SRCS=codeWriter.c main.c parse.c
OBJS=codeWriter.o main.o parse.o
CFLAGS=-O0 -g -Wall

all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(PROG)

codeWriter.o: codeWriter.c codeWriter.h
	$(CXX) -c codeWriter.c -o codeWriter.o

main.o: main.c codeWriter.h parse.h
	$(CXX) -c main.c -o main.o

parse.o: parse.c parse.h
	$(CXX) -c parse.c -o parse.o

clean:
	rm -rf *.o $(PROG) *~
