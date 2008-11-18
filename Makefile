CXX    ?= g++

ifeq ($(shell uname),Darwin)
LIBS    = -framework OpenGL -framework GLUT
else
LIBS    = -lglut
endif


all: dist/data dist/cube dist/solar dist/globe


# Binaries
dist/cube: objs/cube.o objs/common/camera.o
	exec $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

dist/solar: objs/common/camera.o objs/common/quadric.o \
  objs/common/sintable.o objs/common/texture.o \
  objs/solar/data-loader.o objs/solar/lexer.o objs/solar/solar.o \
  objs/solar/sphere.o objs/common/text3d.o
	exec $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

dist/globe: objs/globe.o
	exec $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

dist/data::
	mkdir -p dist
	ln -fs ../data dist/data


# Object files
objs/globe.o: src/globe.cpp
objs/common/camera.o: src/common/camera.cpp src/common/camera.hpp \
  src/common/vector.hpp src/common/mconst.h
objs/common/quadric.o: src/common/quadric.cpp src/common/quadric.hpp
objs/common/sintable.o: src/common/sintable.cpp src/common/sintable.hpp \
  src/common/mconst.h
objs/common/text3d.o: src/common/text3d.cpp src/common/text3d.hpp
objs/common/texture.o: src/common/texture.cpp src/common/texture.hpp \
  src/common/color.hpp
objs/solar/data-loader.o: src/solar/data-loader.cpp src/solar/data-loader.hpp \
  src/solar/sphere.hpp src/solar/../common/color.hpp \
  src/solar/../common/vector.hpp src/solar/../common/texture.hpp \
  src/solar/../common/color.hpp src/solar/lexer.hpp
objs/solar/lexer.o: src/solar/lexer.cpp src/solar/lexer.hpp
objs/solar/solar.o: src/solar/solar.cpp src/solar/../common/camera.hpp \
  src/solar/../common/vector.hpp src/solar/../common/mconst.h \
  src/solar/sphere.hpp src/solar/../common/color.hpp \
  src/solar/../common/vector.hpp src/solar/../common/texture.hpp \
  src/solar/../common/color.hpp src/solar/../common/sintable.hpp \
  src/solar/../common/text3d.hpp src/solar/../common/quadric.hpp \
  src/solar/data-loader.hpp src/solar/../common/mconst.h
objs/solar/sphere.o: src/solar/sphere.cpp src/solar/sphere.hpp \
  src/solar/../common/color.hpp src/solar/../common/vector.hpp \
  src/solar/../common/texture.hpp src/solar/../common/color.hpp \
  src/solar/../common/camera.hpp src/solar/../common/vector.hpp \
  src/solar/../common/mconst.h src/solar/../common/text3d.hpp \
  src/solar/../common/sintable.hpp src/solar/../common/quadric.hpp \
  src/solar/../common/mconst.h
objs/cube.o: src/cube.cpp src/common/camera.hpp src/common/vector.hpp \
  src/common/mconst.h

objs/%.o: src/%.cpp
	exec mkdir -p $(dir $@)
	exec $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


# Other targets
clean::
	exec rm -rf -- objs

distclean::
	exec rm -rf -- objs dist

