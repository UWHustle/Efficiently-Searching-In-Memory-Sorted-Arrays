#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "datasets.h"
#include "benchmark_utils.h"
#include "algorithms/binary_search.h"
#include "algorithms/linear_search.h"
#include "algorithms/interpolation_search.h"
#include "algorithms/tip.h"
#include "algorithms/sip.h"
#include "omp.h"
#include "util.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

using Fn = std::vector<double>(Run &, const DatasetBase &);
using fn_tuple = std::tuple<const char *, Fn *>;
using std::make_tuple;

struct Run {
  DatasetParam input_param;
  std::string name;
  int n_thds;
  bool ok;

  Run(DatasetParam input_param, std::string name, int n_thds)
      : input_param(input_param), name(name), n_thds(n_thds), ok(true) {}

  template<typename SearchAlgorithm, int record_bytes>
  static std::vector<double> searchAndMeasure(Run &run,
                                              const DatasetBase &dataset) {
    const auto
        &inputDataset = static_cast<const Dataset<record_bytes> &>(dataset);
#ifdef INFINITE_REPEAT
    constexpr bool infinite_repeat = true;
#else
    constexpr bool infinite_repeat = false;
#endif
    constexpr int sample_size = 1000;
    const int n_samples = inputDataset.keys.size() / sample_size;
    auto &keys_to_search_for = inputDataset.permuted_keys;

    // TODO this can't be a template of a template have to specialize earlier
    // have to specialize in the class itself. Maybe template macros?
    SearchAlgorithm searchAlgorithm(inputDataset.keys);

    //Stores the times to search each subset
    std::vector<double> ns(n_samples * run.n_thds);
    // Stores the start of each subset in keys_to_search_for
    std::vector<int> subset_indexes(n_samples * run.n_thds);
    auto rng = std::mt19937(42);
    for (auto it = subset_indexes.begin(); it != subset_indexes.end();
         it += n_samples) {
      std::iota(it, it + n_samples, 0);
      if (it != subset_indexes.begin())
        std::shuffle(it, it + n_samples, rng);
    }

    // make copy to pass it easier in the parallel region as
    // private copy (firstprivate)
    const auto inputsum = inputDataset.sum;

#pragma omp parallel default(none)                                             \
    num_threads(run.n_thds) firstprivate(n_samples, inputsum) shared(keys_to_search_for, run, searchAlgorithm, ns, subset_indexes)
    {
      const int tid = omp_get_thread_num();
      const auto &thread_ns = &ns[tid * n_samples];
      thread_ns[0] = 0.0;
      auto valSum = 0UL;
      for (int sample_index = 0;; sample_index++) {
        if (sample_index == n_samples) {
          if (!infinite_repeat || valSum != inputsum)
            break;
          valSum = sample_index = 0;
        }
        int query_index =
            subset_indexes[tid * n_samples + sample_index] * sample_size;

        auto t0 = std::chrono::steady_clock::now();
        for (int i = query_index; i < query_index + sample_size; i++) {
          auto val = searchAlgorithm.search(keys_to_search_for[i]);
          valSum += val;
          assert(val == keys_to_search_for[i]);
        }

        auto t1 = std::chrono::steady_clock::now();
        double ns_elapsed = std::chrono::nanoseconds(t1 - t0).count();
        // thread_ns[0] += ns_elapsed;
        if (!infinite_repeat)
          thread_ns[sample_index] = ns_elapsed / sample_size;
      }
#pragma omp critical
      run.ok = run.ok && valSum == inputsum; // Verify correct results.
    }
    return ns;
  }

  template<int record_bytes>
  static std::vector<double>
              findAlgorithmAndSearch(Run &run, const DatasetBase &dataset) {
    constexpr auto algorithm_mapper = std::array < fn_tuple,
    4 > {
        // Interpolation Search
        make_tuple("is",
                   searchAndMeasure<InterpolationSearch<record_bytes>,
                                    record_bytes>),
        // SIP and TIP
        make_tuple("sip", searchAndMeasure<sip<record_bytes, 8>, record_bytes>),
        make_tuple("tip",
                   searchAndMeasure<tip<record_bytes, 64>, record_bytes>),
        // Binary Search
        make_tuple("bs", searchAndMeasure<Binary<record_bytes>, record_bytes>),
    };
    // Find the correct search algorithm to use as specified in the run.
    auto it = std::find_if(
        algorithm_mapper.begin(), algorithm_mapper.end(), [run](const auto &x) {
          return std::string(std::get<const char *>(x)) == run.name;
        });

    if (it == algorithm_mapper.end()) {
      std::cerr << "algorithm " << run.name << " not found.";
      assert(!"Algorithm not found");
      return std::vector<double>();
    }
    // Run the earch algorithm and return the results
    return std::get<Fn *>(*it)(run, dataset);
  }

  auto search(const DatasetBase &dataset) {
    auto[n, distribution, param, record_bytes] = input_param;
    // Stores the times to search each 1000 record subset
    std::vector<double> ns;

    // Find the correct alorithm and run it
    switch (input_param.record_bytes) {
      case 8:ns = findAlgorithmAndSearch<8>(*this, dataset);
        break;
      case 32:ns = findAlgorithmAndSearch<32>(*this, dataset);
        break;
      case 128:ns = findAlgorithmAndSearch<128>(*this, dataset);
        break;
      default:assert(!"record not supported");
    }

    // If not ok then execution failed, due to wrong results of
    // the search algorithm.
    if (!this->ok)
      std::cerr << "Execution failed" << param << ' ' << this->name << '\n';
    return ns;
  }
};

#endif
