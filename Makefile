CXX    ?= g++

ifeq ($(shell uname),Darwin)
LIBS    = -framework OpenGL -framework GLUT
else
LIBS    = -lglut
endif

all: dist/data dist/physics dist/graph

doc::
	exec doxygen Doxyfile

dist/physics:
	@exec mkdir -p -- objs
	exec qmake -makefile -o objs/physics.make -unix src/physics.pro
	exec make -C objs -f physics.make

dist/graph:
	@exec mkdir -p -- objs
	exec qmake -makefile -o objs/graph.make -unix src/graph.pro
	exec make -C objs -f graph.make

dist/data:
	exec mkdir -p dist/data
	exec $(MAKE) -C data DATA_DIR=../dist/data all

clean::
	exec rm -rf -- objs

distclean:: clean
	exec rm -rf -- dist doc

.PHONY: dist/data dist/physics dist/graph clean distclean
