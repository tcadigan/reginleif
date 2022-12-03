2015 = aoc_2015_d1 aoc_2015_d2 aoc_2015_d3 aoc_2015_d4 aoc_2015_d5 aoc_2015_d6 \
       aoc_2015_d7 aoc_2015_d8 aoc_2015_d9 aoc_2015_d10 aoc_2015_d11 \
       aoc_2015_d12 aoc_2015_d13 aoc_2015_d14 aoc_2015_d15 aoc_2015_d16 \
       aoc_2015_d17 aoc_2015_d18 aoc_2015_d19 aoc_2015_d20 aoc_2015_d21 \
       aoc_2015_d22 aoc_2015_d23 aoc_2015_d24 aoc_2015_d25

CXX = clang
CXXFLAGS = -Wall

.ifdef DISABLE_BEAR
BEAR =
.else
BEAR = bear --append --
.endif

PROGS = ${2015:=_p1} ${2015:=_p2}

all: ${PROGS}

${PROGS}: ${.TARGET:=.o}
	${CXX} ${CXXFLAGS} ${.TARGET:=.o} -o ${.TARGET}

clean:
	rm -rf *~ *.o ${PROGS}

.cpp.o:
	${BEAR} ${CXX} ${CXXFLAGS} -c ${.IMPSRC} -o ${.TARGET}
