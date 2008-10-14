CXX    ?= g++

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif

all: cube

cube.o: cube.cpp camera.hpp
camera.o: camera.cpp camera.hpp
planet.o: planet.cpp planet.hpp
text3d.o: text3d.cpp text3d.hpp

cube: cube.o camera.o
	$(CXX) $(LDFLAGS) -o cube cube.o camera.o $(LIBS)

solar: solar.o camera.o planet.o text3d.o
	$(CXX) $(LDFLAGS) -o solar solar.o camera.o planet.o text3d.o $(LIBS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f -- cube solar *.o

