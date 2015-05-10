NAME    = PixelCity
SRCS    = $(wildcard *.cpp)
OBJS    = $(patsubst %.cpp,%.o,$(SRCS))
CFLAGS  = -Wall 
LDFLAGS = -lGL -lGLU -lglut
CXX     = g++

all: ${OBJS}
	${CXX} ${CFLAGS} ${LDFLAGS} -o ${NAME} ${OBJS}

clean:
	rm -rf ${OBJS}

%.o: %.cpp
	${CXX} ${CLAGS} -c -o $@ $^
