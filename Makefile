CXX    ?= g++

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif


HEADERS	 = camera.hpp data-loader.hpp quadric.hpp sphere.hpp texture.hpp
HEADERS	+= color.hpp lexer.hpp sintable.hpp text3d.hpp vector.hpp
OBJS	 = camera.o data-loader.o lexer.o sintable.o sphere.o texture.o
OBJS	+= quadric.o solar.o text3d.o


all: cube solar


cube.o       : cube.cpp        $(HEADERS)
solar.o      : solar.cpp       $(HEADERS)
sphere.o     : sphere.cpp      $(HEADERS)
data-loader.o: data-loader.cpp $(HEADERS)
camera.o     : camera.cpp      camera.hpp
text3d.o     : text3d.cpp      text3d.hpp
sintable.o   : sintable.cpp    sintable.hpp
lexer.o      : lexer.cpp       lexer.hpp
quadric.o    : quadric.cpp     quadric.hpp
texture.o    : texture.cpp     texture.hpp


cube: cube.o camera.o
	exec $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

solar: $(OBJS)
	exec $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

globe: globe.cpp
	exec $(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)


%.o: %.cpp
	exec $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


clean:
	exec rm -f -- cube solar *.o parser.[ch]pp *.hh

