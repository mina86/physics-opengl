CXX    ?= g++

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif

all: cube

cube: cube.cpp types.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o cube cube.cpp $(LIBS)

clean:
	rm -f -- cube

