TEMPLATE = app
DEPENDPATH += gl ui lib $$TARGET

LIBS += -lGLU -lglut

QT += opengl gui
DESTDIR = ../dist
MOC_DIR = ../objs/$$TARGET
OBJECTS_DIR = ../objs/$$TARGET

HEADERS += $$system(find $$DEPENDPATH -name *.hpp -o -name *.h)
SOURCES += $$system(find $$DEPENDPATH -name *.cpp)
FORMS   += $$system(find $$DEPENDPATH -name *.ui)
