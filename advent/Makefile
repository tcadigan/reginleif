2015 = aoc_2015_d1 aoc_2015_d2 aoc_2015_d3 aoc_2015_d4 aoc_2015_d5 aoc_2015_d6 \
       aoc_2015_d7 aoc_2015_d8

CXX = clang++90

.ifndef DISABLE_BEAR
BEAR = bear -a
.endif

PROGS = ${2015:=_p1} ${2015:=_p2}

all: ${PROGS}

${PROGS}: ${.TARGET:=.o}
	${CXX} ${CXXFLAGS} ${.TARGET:=.o} -o ${.TARGET:=}

clean:
	rm -rf *~ *.o ${PROGS}

.cpp.o:
	${BEAR} ${CXX} ${STATIC_CXXFLAGS} ${CXXFLAGS} -c ${.IMPSRC} -o ${.TARGET}
