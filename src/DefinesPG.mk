HOME=..
GC_HOME=$(HOME)/engine/graphchi-cpp
PX_HOME=$(HOME)/engine/phoenix++-1.0

INCFLAGS = -I/usr/local/include/ -I$(PX_HOME)/include/

CPP = g++
CPPFLAGS = -g -O3 $(INCFLAGS)  -fopenmp -Wall -Wno-strict-aliasing 
LINKERFLAGS = -lz
DEBUGFLAGS = -g -ggdb $(INCFLAGS)
HEADERS=$(shell find $(GC_HOME) -name '*.hpp')

INCLUDE_GC=$(GC_HOME)/example_apps #gc: GraphChi

include $(PX_HOME)/Defines.mk

LIBS += -L$(PX_HOME)/$(LIB_DIR) -l$(PHOENIX)

