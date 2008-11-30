CXX    ?= g++

ifeq ($(shell uname),Darwin)
LIBS    = -framework OpenGL -framework GLUT
else
LIBS    = -lglut
endif


all: dist/data dist/solar dist/physics


# Documentation
doc::
	exec doxygen Doxyfile


# Binaries
dist/solar: objs/common/camera.o objs/common/quadric.o \
  objs/common/sintable.o objs/common/texture.o \
  objs/solar/data-loader.o objs/solar/lexer.o objs/solar/solar.o \
  objs/solar/sphere.o objs/common/text3d.o
	exec $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

dist/physics: objs/common/camera.o objs/common/quadric.o \
  objs/common/sintable.o objs/common/texture.o objs/common/text3d.o \
  objs/physics/physics.o objs/physics/object.o objs/physics/lexer.o \
  objs/physics/data-loader.o
	exec $(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

dist/data::
	mkdir -p dist
	ln -fs ../data dist/


# Object files
objs/common/camera.o: src/common/camera.hpp \
  src/common/vector.hpp src/common/mconst.h
objs/common/quadric.o: src/common/quadric.hpp
objs/common/sintable.o: src/common/sintable.hpp src/common/mconst.h
objs/common/text3d.o: src/common/text3d.hpp
objs/common/texture.o: src/common/texture.hpp src/common/color.hpp

objs/solar/data-loader.o: src/solar/data-loader.hpp src/solar/sphere.hpp \
  src/common/color.hpp src/common/vector.hpp src/common/texture.hpp \
  src/solar/lexer.hpp
objs/solar/lexer.o: src/solar/lexer.hpp
objs/solar/solar.o: src/common/camera.hpp src/common/vector.hpp \
  src/common/mconst.h src/solar/sphere.hpp src/common/texture.hpp \
  src/common/color.hpp src/common/sintable.hpp src/common/text3d.hpp \
  src/common/quadric.hpp src/solar/data-loader.hpp
objs/solar/sphere.o: src/solar/sphere.hpp src/common/color.hpp \
  src/common/vector.hpp src/common/texture.hpp src/common/camera.hpp \
  src/common/mconst.h src/common/text3d.hpp src/common/sintable.hpp \
  src/common/quadric.hpp

objs/physics/object.o: src/physics/object.hpp src/common/color.hpp \
  src/common/vector.hpp src/common/texture.hpp src/common/camera.hpp \
  src/common/mconst.h src/common/text3d.hpp src/common/sintable.hpp \
  src/common/quadric.hpp
objs/physics/physics.o: src/common/camera.hpp src/common/vector.hpp \
  src/common/mconst.h src/physics/object.hpp src/common/texture.hpp \
  src/common/color.hpp src/common/sintable.hpp src/common/text3d.hpp \
  src/common/quadric.hpp src/physics/data-loader.hpp
objs/physics/data-loader.o: src/physics/data-loader.hpp \
  src/physics/object.hpp src/common/color.hpp src/common/vector.hpp \
  src/common/texture.hpp src/physics/lexer.hpp
objs/physics/lexer.o: src/physics/lexer.hpp

objs/%.o: src/%.cpp
	exec mkdir -p $(dir $@)
	exec $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


# Other targets
clean::
	exec rm -rf -- objs

distclean::
	exec rm -rf -- objs dist doc

