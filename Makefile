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
INCFLAGS = -I/usr/local/include/ -I$(PX_HOME)/include/

include $(PX_HOME)/Defines.mk

#echo $(OSTYPE)

default: all
all: phoenix graphchi hybrid

phoenix:
	echo $(OSTYPE)
	@echo ======================================== phoenix
	( cd src && $(MAKE) -f Makefile.phoenix )

graphchi:
	@echo ======================================== graphchi
	( cd src && $(MAKE) -f Makefile.graphchi )

hybrid: 
	@echo ======================================== hybrid
	( cd src && $(MAKE) -f Makefile.hybrid )

echo:
	echo $(HEADERS)

clean:
	@rm -rf bin/*
	( cd src && rm -rf *.o )
