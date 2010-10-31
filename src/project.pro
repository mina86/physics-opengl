TEMPLATE = app
DEPENDPATH += common $$TARGET

QT += opengl
DESTDIR = ../dist
MOC_DIR = ../objs
OBJECTS_DIR = ../objs

HEADERS += $$system(find $$DEPENDPATH -name \*.hpp -o -name \*.h)
SOURCES += $$system(find $$DEPENDPATH -name \*.cpp)
