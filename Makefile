GCCCXX=g++
GCCCXXFLAGS=-std=c++17

CLANGHOME=./clang5
CLANGCXX=$(CLANGHOME)/bin/clang++
CLANGCXXFLAGS=-std=c++17 -stdlib=libc++
LDFLAGS=-fopenmp

HEADERS=src/benchmark.h src/bin.h src/padded_vector.h src/interpolate.h src/util.h src/div.h src/lin.h src/bin_eyt.h
SOURCES=src/search.cc

.PHONY: run gdb clean perf

##### Run Targets ######
run : searchbench experiments.tsv

gdb : debug
gdb :
	gdb --args ./debug experiments.tsv

perf : CLANGCXXFLAGS += -O3 -DNDEBUG -DINFINITE_REPEAT
perf :
	$(CLANGCXX) $(CLANGCXXFLAGS) $(SOURCES) -o$@ $(LDFLAGS)
	perf record -F99 -g ./perf experiments.tsv

clean:
	rm -f ./searchbench ./debug ./dump

## CLANG ######################
####### Build Targets #########

searchbench: CLANGCXXFLAGS += -O3 -DNDEBUG
searchbench: $(SOURCES) $(HEADERS)
	$(CLANGCXX) $(CLANGCXXFLAGS) $(SOURCES) -osearchbench $(LDFLAGS)

debug: CLANGCXXFLAGS += -O0
debug: $(SOURCES) $(HEADERS)
	$(CLANGCXX) $(CLANGCXXFLAGS) $(SOURCES) -o$@ $(LDFLAGS)

dump: src/dump.cc src/benchmark.h
	$(CXX) $(CLANGCXXFLAGS) src/dump.cc -o $@ $(LDFLAGS)


## GCC ###############
####### Build Targets #########

searchbench_gcc: GCCCXXFLAGS += -O3 -DNDEBUG
searchbench_gcc: $(SOURCES) $(HEADERS)
	$(GCCCXX) $(GCCCXXFLAGS) $(SOURCES) -osearchbench $(LDFLAGS)

debug_gcc : GCCCXXFLAGS += -O0
debug_gcc : $(SOURCES) $(HEADERS)
		$(GCCCXX) $(GCCCXXFLAGS) $(SOURCES) -odebug $(LDFLAGS)

dump_gcc : src/dump.cc src/benchmark.h
	$(CXX) $(GCCCXXFLAGS) src/dump.cc -odump $(LDFLAGS)
