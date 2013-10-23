CXX    ?= g++

ifeq ($(shell uname),Darwin)
LIBS    = -framework OpenGL -framework GLUT
else
LIBS    = -lglut -lGL -lGLU
endif

all: dist/data dist/graph dist/doc.pdf


# This currently does not compile
dist/physics:
	@exec mkdir -p -- objs
	exec qmake -makefile -o objs/physics.make src/physics.pro
	exec make -C objs -f physics.make


# Graph
dist/graph:
	@exec mkdir -p -- objs
	exec qmake -makefile -o objs/graph.make src/graph.pro
	exec make -C objs -f graph.make


# Data
dist/data:
	exec mkdir -p dist/data
	exec $(MAKE) -C data DATA_DIR=../dist/data all


# Documentation
dist/doc.pdf: doc/doc.pdf
	exec mv -- $< $@

doc/doc.pdf::
	exec $(MAKE) -C doc doc.pdf

# Misc
clean::
	exec $(MAKE) -C doc clean

distclean::
	exec rm -rf -- objs dist
	exec $(MAKE) -C doc distclean

.PHONY: dist/data dist/graph clean distclean
