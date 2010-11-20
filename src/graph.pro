TARGET = graph
unix:LIBS = -lgsl -lgslcblas
DEFINES += USE_GSL
include(project.pro)
