PROG = vm eq-solver
CXX = clang++
CXXFLAGS = -Wall --std=c++17

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

all: ${PROG}

${PROG}: ${.TARGET:=.o}
	${CXX} ${CXXFLAGS} ${.TARGET:=.o} -o ${.TARGET}

clean:
	rm -rf *~ *.o ${PROG} eqsolver

.cpp.o:
	${BEAR} ${CXX} ${CXXFLAGS} -c ${.IMPSRC} -o ${.TARGET}
