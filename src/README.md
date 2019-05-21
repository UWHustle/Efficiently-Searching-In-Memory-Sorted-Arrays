# Implementation
The benchmarking framework and search methods are implemented in a modular way
using templates to make experimentation 
and rapid iteration faster.

+ search.cc: contains the main function and is the entry point to the framework
+ bin.h, bin_eyt.h: different implementations of Binary Search 
+ interpolate.h: implements all the variations of the IS, SIP, TIP. At the end
  of the file you can find the definitions of the different search methods.
+ benchmark.h: the orchestrator of the benchmarking framework. Also contains a 
  more detailed list of all the implemented search algorithms.
