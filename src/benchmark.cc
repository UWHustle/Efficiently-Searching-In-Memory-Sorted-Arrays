#include "benchmark.h"
#include "datasets.h"
#include "benchmark_utils.h"
#include "util.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <libgen.h>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <x86intrin.h>
#include <cstring>


// Takes one argument, a tsv file with the specification of the experiment to
// run.
int main(int argc, char *argv[]) {
  using RunTuple = std::tuple<DatasetParam::Tuple, std::string, int>;

  // Load the experiment specification from the Dataset file
  std::vector<Run> runs = loadRunsFromFile(std::ifstream(argv[1]));

  // create the Dataset needed by the experiments
  DatasetBase::DatasetMap datasets_map;
  for (Run r : runs){
    createDataset(r.dataset_param, datasets_map);
  }

  // Run the experiments
  bool first = true;
  int run_ix = 0;

  RunTuple old_param;
  auto t0 = std::chrono::steady_clock::now();
  for (auto &run : runs) {
    auto[distribution, param, n, record_bytes] = run.dataset_param;
    RunTuple new_param{run.dataset_param, run.name, run.n_thds};
    if (new_param != old_param) {
      std::cerr << "Running experiment: ";
      std::cerr << n << ' ' << distribution << ' ' << param << ' '
                << record_bytes << ' ' << run.name << ' ' << run.n_thds << '\n';
      old_param = new_param;
      if (run_ix == 0) {
        std::cout << std::setw(3) << "Run\t" << std::setw(11) << "DatasetSize\t"
                  << std::setw(12) << "Distribution\t" << std::setw(10)
                  << "Parameter\t" << std::setw(8) << "#threads\t"
                  << std::setw(15) << "SearchAlgorithm\t" << std::setw(15)
                  << "RecordSizeBytes\t" << std::setw(6) << "TimeNS\t"
                  << "\n";
      } else {
        std::cerr << std::setw(3) << "Run\t" << std::setw(11) << "DatasetSize\t"
                  << std::setw(12) << "Distribution\t" << std::setw(10)
                  << "Parameter\t" << std::setw(8) << "#threads\t"
                  << std::setw(15) << "SearchAlgorithm\t" << std::setw(15)
                  << "RecordSizeBytes\t" << std::setw(6) << "TimeNS\t"
                  << "\n";
      }
    }

    // Run the experiment using the run specificatiom and the corresponding
    // dataset. Each experiments measures the time to search subsets of
    // 1000 keys.
    for (auto ns : run.search(*datasets_map.at(run.dataset_param))) {
      if (!run.ok) {
        break;
      }
      std::cout << std::setw(3) << run_ix << "\t" << std::setw(11) << n << "\t"
                << std::setw(12) << distribution.c_str() << "\t"
                << std::setw(10) << basename(strdup(param.c_str())) << "\t"
                << std::setw(8) << run.n_thds << "\t" << std::setw(15)
                << run.name.c_str() << "\t" << std::setw(15) << record_bytes
                << "\t" << std::setw(6) << std::setprecision(5) << ns << "\t"
                << "\n";
    }
    run_ix++;
    std::cerr << "\n";;
  }
  std::cerr << '\n';
}
