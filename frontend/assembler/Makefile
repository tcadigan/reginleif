PROG=assembler
SRCS=code.c main.o parser.c symbolTable.o
OBJS=code.o main.o parser.o symbolTable.c
CFLAGS=-O0 -g -Wall

all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $(PROG)

code.o: code.c code.h
	$(CXX) -c code.c -o code.o

main.o: main.c code.h parser.h symbolTable.h
	$(CXX) -c main.c -o main.o

parser.o: parser.c parser.h
	$(CXX) -c parser.c -o parser.o

symbolTable.o: symbolTable.c symbolTable.h
	$(CXX) -c symbolTable.c -o symbolTable.o

clean:
	rm -rf *.o $(PROG) *~
