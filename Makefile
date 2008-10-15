CXX    ?= g++

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif


all: cube solar


cube.o: cube.cpp camera.hpp
solar.o: solar.cpp camera.hpp planet.hpp color.hpp text3d.hpp
camera.o: camera.cpp camera.hpp
planet.o: planet.cpp planet.hpp color.hpp camera.hpp sintable.hpp
text3d.o: text3d.cpp text3d.hpp
sintable.o: sintable.cpp sintable.hpp
lexer.o: lexer.cpp lexer.hpp parser.hpp color.hpp


parser.o: parser.cpp parser.hpp planet.hpp color.hpp
parser.cpp: parser.hpp
parser.hpp: parser.y
	exec bison parser.y


cube: cube.o camera.o
	exec $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

solar: solar.o camera.o planet.o text3d.o sintable.o
	exec $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	exec $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


clean:
	exec rm -f -- cube solar *.o parser.[ch]pp *.hh

