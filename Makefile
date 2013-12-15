##1. For hybrid model: 
##Step 1: gm_sparse_phoenix.cpp
##Step 2: mm_sparse_graphchi.cpp
##2. For phoenix:
##Step 1: gm_dense_phoenix.cpp
##Step 2: mm_dense_phoenix.cpp
##3. For grapchi:
##Step 1: gm_sparse_graphchi.cpp
## Step 2: mm_sparse_graphchi.cpp

HOME=.
GC_HOME=$(HOME)/engine/graphchi-cpp
PX_HOME=$(HOME)/engine/phoenix++-1.0

INCFLAGS = -I/usr/local/include/ -I./src/

CPP = g++
CPPFLAGS = -g -O3 $(INCFLAGS)  -fopenmp -Wall -Wno-strict-aliasing 
LINKERFLAGS = -lz
DEBUGFLAGS = -g -ggdb $(INCFLAGS)
HEADERS=$(shell find $(GC_HOME) -name '*.hpp')

default: all
all: phoenix graphchi hybrid

phoenix:
	@echo ======================================== phoenix
	( $(MAKE) Makefile.phoenix )

#phoenix: src/gm_dense_phoenix src/mm_dense_phoenix
#graphchi: src/gm_sparse_graphchi src/mm_sparse_graphchi
#hybrid: src/gm_sparse_phoenix.cpp src/mm_sparse_graphchi

echo:
	echo $(HEADERS)
clean:
	@rm -rf bin/*

INCLUDE_GC=$(GC_HOME)/example_apps #gc: GraphChi

include $(PX_HOME)/Defines.mk

LIBS += -L$(PX_HOME)/$(LIB_DIR) -l$(PHOENIX)

src/%: src/%.cpp
	

src/%: $(phoenix).cpp $(HEADERS)
	@mkdir -p bin/$(@D)
	$(CPP) $(CPPFLAGS) -I$(INCLUDE_GC) -I$(GC_HOME)/src $@.cpp -o bin/$@ $(LINKERFLAGS)


src/%.o: src/%.cpp
	$(CXX) -fpermissive $(CFLAGS) -c $< -o $@ -I$(PX_HOME)/$(INC_DIR)

src/%: src/%.o
	@mkdir -p bin/$(@D)
	$(CXX) -fpermissive $(CFLAGS) -o bin/$@ $@.o $(LIBS)

