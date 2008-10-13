CXX    ?= g++

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif

all: cube

cube.o: cube.cpp camera.hpp
camera.o: camera.cpp camera.hpp

cube: cube.o camera.o
	$(CXX) $(LDFLAGS) -o cube cube.o camera.o $(LIBS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f -- cube

