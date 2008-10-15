CXX    ?= g++

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif


all: cube solar


cube.o: cube.cpp camera.hpp vector.hpp
solar.o: solar.cpp camera.hpp vector.hpp sphere.hpp color.hpp text3d.hpp
camera.o: camera.cpp camera.hpp vector.hpp
sphere.o: sphere.cpp sphere.hpp color.hpp camera.hpp vector.hpp sintable.hpp
sphere.o: quadric.hpp
text3d.o: text3d.cpp text3d.hpp
sintable.o: sintable.cpp sintable.hpp
lexer.o: lexer.cpp lexer.hpp parser.hpp color.hpp
quadric.o: quadric.cpp quadric.hpp

parser.o: parser.cpp parser.hpp sphere.hpp color.hpp
parser.cpp: parser.hpp
parser.hpp: parser.y
	exec bison parser.y


cube: cube.o camera.o
	exec $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

solar: solar.o camera.o sphere.o text3d.o sintable.o quadric.o
	exec $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	exec $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


clean:
	exec rm -f -- cube solar *.o parser.[ch]pp *.hh

