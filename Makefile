CHOME=./clang5
CXX=$(CHOME)/bin/clang++
CXXFLAGS=-std=c++17 -stdlib=libc++
LDFLAGS=-fopenmp

HEADERS=src/benchmark.h src/bin.h src/padded_vector.h src/interpolate.h src/util.h src/div.h src/lin.h src/bin_eyt.h
SOURCES=src/search.cc

.PHONY: run gdb clean perf

##### Run Targets ######
run : searchbench experiments.tsv

gdb : debug
gdb :
		gdb --args ./debug experiments.tsv

perf : CXXFLAGS += -O3 -DNDEBUG -DINFINITE_REPEAT
perf :
		$(CXX) $(CXXFLAGS) $(SOURCES) -o$@ $(LDFLAGS)
		perf record -F99 -g ./perf experiments.tsv

clean:
		rm -f ./searchbench ./debug ./dump

####### Build Targets #########

searchbench: CXXFLAGS += -O3 -DNDEBUG
searchbench: $(SOURCES) $(HEADERS)
		$(CXX) $(CXXFLAGS) $(SOURCES) -o$@ $(LDFLAGS)

debug : CXXFLAGS += -O0
debug: $(SOURCES) $(HEADERS)
		$(CXX) $(CXXFLAGS) $(SOURCES) -o$@ $(LDFLAGS)

dump : src/dump.cc src/benchmark.h
	$(CXX) $(CXXFLAGS) src/dump.cc -o $@ $(LDFLAGS)
