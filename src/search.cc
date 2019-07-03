#include "benchmark.h"
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
  using RunTuple = std::tuple<InputParam::Tuple, std::string, int>;

  std::vector<Run> runs = Run::load(std::ifstream(argv[1]));
  auto inputs = InputBase::load(runs);
  bool first = true;
  int run_ix = 0;

  RunTuple old_param;
  auto t0 = std::chrono::steady_clock::now();
  for (auto &run : runs) {
    auto[distribution, param, n, record_bytes] = run.input_param;
    RunTuple new_param{run.input_param, run.name, run.n_thds};
    auto t1 = std::chrono::steady_clock::now();
    if (new_param != old_param) {
      std::cerr << "Running experiment: ";
      std::cerr << n << ' ' << distribution << ' ' << param << ' '
                << record_bytes << ' ' << run.name << ' ' << run.n_thds << '\n';
      old_param = new_param;
      t0 = t1;
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
    } else if (std::chrono::duration<double, std::milli>(t1 - t0).count() >
               1000.0) {
      std::cerr << '.';
      t0 = t1;
    }

    for (auto ns : run(*inputs.at(run.input_param))) {
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
