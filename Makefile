CXX    ?= g++

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif

all: cube solar

cube.o: cube.cpp camera.hpp
solar.o: solar.cpp camera.hpp planet.hpp text3d.hpp
camera.o: camera.cpp camera.hpp
planet.o: planet.cpp planet.hpp camera.hpp sintable.hpp
text3d.o: text3d.cpp text3d.hpp
sintable.o: sintable.cpp sintable.hpp

cube: cube.o camera.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

solar: solar.o camera.o planet.o text3d.o sintable.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f -- cube solar *.o

