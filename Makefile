GCCCXX=g++
GCCCXXFLAGS=-std=c++17

CLANGHOME=./clang5
CLANGCXX=$(CLANGHOME)/bin/clang++
CLANGCXXFLAGS=-std=c++17 -stdlib=libc++

OS := $(shell uname)
ifeq ($(OS),Darwin)
	LDFLAGS=-Xpreprocessor -fopenmp -lomp
else
	LDFLAGS=-fopenmp
endif

HEADERS=src/benchmark.h src/datasets.h src/benchmark_utils.h \
		src/algorithms/binary_search.h src/padded_vector.h \
		src/util.h src/algorithms/div.h src/algorithms/linear_search.h \
		src/algorithms/sip.h src/algorithms/tip.h
SOURCES=src/benchmark.cc

.PHONY: run gdb clean perf dump dump_bin txt_to_bin bin_to_txt sample_bin

##### Run Targets ######
run : searchbench experiments.tsv

gdb : debug_searchbench
gdb :
	gdb --args ./debug experiments.tsv

perf : CLANGCXXFLAGS += -O3 -DNDEBUG -DINFINITE_REPEAT
perf :
	$(CLANGCXX) $(CLANGCXXFLAGS) $(SOURCES) -o$@ $(LDFLAGS)
	perf record -F99 -g ./perf experiments.tsv

clean:
	rm -f ./searchbench ./debug ./dump ./dump_bin ./txt_to_bin ./sample_bin ./bin_to_txt

dumpall: dump dump_bin txt_to_bin bin_to_txt sample_bin

## GCC ###############
####### Build Targets #########

searchbench: GCCCXXFLAGS += -O3 -DNDEBUG
searchbench: $(SOURCES) $(HEADERS)
	$(GCCCXX) $(GCCCXXFLAGS) $(SOURCES) -o$@ $(LDFLAGS)

debug_searchbench : GCCCXXFLAGS += -O0
debug_searchbench : $(SOURCES) $(HEADERS)
		$(GCCCXX) $(GCCCXXFLAGS) $(SOURCES) -o$@ $(LDFLAGS)

dump : src/dump.cc src/benchmark.h
	$(CXX) $(GCCCXXFLAGS) src/dump.cc -o$@ $(LDFLAGS)

dump_bin : src/dump_bin.cc src/benchmark.h
	$(CXX) $(GCCCXXFLAGS) src/dump_bin.cc -o$@ $(LDFLAGS)

txt_to_bin: src/convert_txt_to_bin.cc src/benchmark.h
	$(CXX) $(GCCCXXFLAGS) src/convert_txt_to_bin.cc -o$@ $(LDFLAGS)

bin_to_txt: src/convert_bin_to_txt.cc src/benchmark.h
	$(CXX) $(GCCCXXFLAGS) src/convert_bin_to_txt.cc -o$@ $(LDFLAGS)

sample_bin: src/sample_bin.cc src/benchmark.h
	$(CXX) $(GCCCXXFLAGS) src/sample_bin.cc -o$@ $(LDFLAGS)

## CLANG ######################
####### Build Targets #########

clang5_searchbench: CLANGCXXFLAGS += -O3 -DNDEBUG
clang5_searchbench: $(SOURCES) $(HEADERS)
	$(CLANGCXX) $(CLANGCXXFLAGS) $(SOURCES) -o$@ $(LDFLAGS)

clang5_debug: CLANGCXXFLAGS += -O0
clang5_debug: $(SOURCES) $(HEADERS)
	$(CLANGCXX) $(CLANGCXXFLAGS) $(SOURCES) -o$@ $(LDFLAGS)
