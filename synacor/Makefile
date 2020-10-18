PROG = vm
CXX = clang++90
CXXFLAGS = -Wall -std=c++17

.ifndef DISABLE_BEAR
BEAR = bear -a
.endif

SRCS = vm.cpp

all: ${PROG}

${PROG}: ${SRCS:.cpp=.o}
	${CXX} ${CXXFLAGS} ${SRCS:.cpp=.o} -o ${.TARGET}

clean:
	rm -rf *~ *.o ${PROG}

.cpp.o:
	${BEAR} ${CXX} ${STATIC_CXXFLAGS} ${CXXFLAGS} -c ${.IMPSRC} -o ${.TARGET}
