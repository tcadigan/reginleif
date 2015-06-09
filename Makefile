NAME    = PixelCity
SRCS    = $(wildcard *.cpp)
OBJS    = $(patsubst %.cpp,%.o,$(SRCS))
CXXFLAGS  = -Wall 
LDFLAGS = -lGL -lGLU
CXX     = g++

all: ${OBJS}
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS) 

clean:
	rm -rf ${OBJS}

%.o: %.cpp
	${CXX} ${CLAGS} -c -o $@ $^
