UVA = uva_100 uva_101 uva_102 uva_108 uva_111 uva_112 uva_113 uva_120 uva_123 \
      uva_136 uva_146 uva_151 uva_152 uva_156 uva_160 uva_253 uva_272 uva_299 \
      uva_340 uva_400 uva_401 uva_409 uva_414 uva_424 uva_445 uva_457 uva_458 \
      uva_465 uva_488 uva_489 uva_490 uva_494 uva_537 uva_591 uva_644 uva_673 \
      uva_694 uva_748 uva_755 uva_10010 uva_10018 uva_10035 uva_10038 \
      uva_10055 uva_10071 uva_10082 uva_10106 uva_10115 uva_10161 uva_10189 \
      uva_10194 uva_10300 uva_10361 uva_10370 uva_10420 uva_10474 uva_10494 \
      uva_10633 uva_10783 uva_10785 uva_10815 uva_10878

CXX = clang++

.ifndef DISABLE_BEAR
BEAR = bear -a
.endif

all: ${UVA}

${UVA}: ${.TARGET:=.o}
	${CXX} ${CXXFLAGS} ${.TARGET:=.o} -o ${.TARGET:=}

clean:
	rm -rf *~ *.o ${UVA}

.cpp.o:
	${BEAR} ${CXX} ${STATIC_CXXFLAGS} ${CXXFLAGS} -c ${.IMPSRC} -o ${.TARGET}
