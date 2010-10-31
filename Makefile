CXX    ?= g++

ifeq ($(shell uname),Darwin)
LIBS    = -framework OpenGL -framework GLUT
else
LIBS    = -lglut
endif

all: dist/data dist/physics

doc::
	exec doxygen Doxyfile

dist/physics:
	@exec mkdir -p -- objs
	exec qmake -makefile -o objs/physics.make -unix src/project.pro TARGET=physics
	exec make -C objs -f physics.make

dist/data:
	exec mkdir -p dist/data
	exec $(MAKE) -C data DATA_DIR=../dist/data all

clean::
	exec rm -rf -- objs

distclean:: clean
	exec rm -rf -- dist doc

.PHONY: dist/data dist/physics clean distclean
