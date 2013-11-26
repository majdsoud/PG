HOME=.
GC_HOME=$(HOME)/engine/graphchi-cpp
PX_HOME=$(HOME)/engine/phoenix++-1.0

INCFLAGS = -I/usr/local/include/ -I./src/

CPP = g++
CPPFLAGS = -g -O3 $(INCFLAGS)  -fopenmp -Wall -Wno-strict-aliasing 
LINKERFLAGS = -lz
DEBUGFLAGS = -g -ggdb $(INCFLAGS)
HEADERS=$(shell find $(GC_HOME) -name '*.hpp')

all: phoenix graphchi
phoenix: yixiu
grpahchi: matrix_multiply

echo:
	echo $(HEADERS)
clean:
	@rm -rf bin/*

INCLUDE_GC=$(GC_HOME)/example_apps #gc: GraphChi

src/graphchi/%: src/graphchi/%.cpp $(HEADERS)
	@mkdir -p bin/$(@D)
	$(CPP) $(CPPFLAGS) -I$(INCLUDE_GC) -I$(GC_HOME)/src $@.cpp -o bin/$@ $(LINKERFLAGS) 
