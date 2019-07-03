#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "bin.h"
#include "bin_eyt.h"
#include "interpolate.h"
#include "lin.h"

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


template <typename T> T parse(std::string s) {
  T t;
  std::stringstream(s) >> t;
  return t;
}

struct Run;

struct InputParam {
  std::string distribution, param;
  long n;
  int record_bytes;

  using Tuple = std::tuple<std::string, std::string, long, int>;

  operator Tuple() { return Tuple{distribution, param, n, record_bytes}; }
};

struct InputBase {
  using InputMap = std::map<InputParam::Tuple, std::unique_ptr<InputBase>>;
  static InputMap load(std::vector<Run> runs);
};

// Parse and create the datasets, generate the keys or import from file.
template <int record_bytes> struct Input : public InputBase {
private:
  auto uniform(long seed) {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<Key> dist(1, (1ULL << 63) - 2);
    std::vector<Key> v(keys.size());
    for (auto &y : v)
      y = dist(rng);
    std::sort(v.begin(), v.end());
    return v;
  }

  auto gap(long seed, double sparsity) {
    assert(sparsity <= 1.0);

    std::vector<Key> v(keys.size());
    long range = v.size() / sparsity;
    assert(range >= v.size());

    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<Key> dist(1, range + 1);
    std::set<Key> skips;
    while (skips.size() + v.size() < range)
      skips.insert(dist(rng));
    for (Key k = 1, i = 0; k < range + 1; k++)
      if (skips.count(k) == 0)
        v[i++] = k;
    return v;
  }

  auto fal(double shape) {
    std::vector<Key> v(keys.size());
    auto n = v.size();
    for (auto i = 0; i < v.size() - 1; i++) {
      // scale up to ensure elements are distinct for as long as possible
      v[i] = pow((double)(n - i), -shape) * std::numeric_limits<Key>::max();
    }
    v.back() = std::numeric_limits<Key>::max();
    return v;
  }

  auto cfal(double shape) {
    auto v = fal(shape);
    auto max_sum = std::accumulate(v.begin(), v.end(), 0.0L);
    auto scale = std::numeric_limits<Key>::max() / max_sum;
    std::transform(v.begin(), v.end(), v.begin(),
                   [scale](auto x) { return x * scale; });
    std::partial_sum(v.begin(), v.end(), v.begin());
    return v;
  }

  void fill(const std::vector<Key> &&v, long seed = 42) {
    keys = std::move(v);
    permuted_keys.resize(keys.size());
    std::copy(keys.begin(), keys.end(), permuted_keys.begin());
    std::shuffle(permuted_keys.begin(), permuted_keys.end(),
                 std::mt19937(seed));
  }

public:
  using Id = std::tuple<long, std::string, std::string>;

  std::vector<Key> permuted_keys;
  PaddedVector<record_bytes> keys;
  unsigned long sum;

  Input(const long n, const std::string &distribution,
        const std::vector<std::string> &params)
      : permuted_keys(n), keys(n) {
    auto param = params.begin();
    // datasetname - parameter
    // uniform     - random gen seed
    // gap         - random gen seed,gap parameter
    // file        - path
    // fal         - shape
    // cfal        - shape
    if (distribution == "uniform") {
      auto seed = parse<long>(param[0]);
      fill(uniform(seed));
    } else if (distribution == "gap") {
      auto[seed, sparsity] =
          std::tuple{parse<long>(param[0]), parse<double>(param[1])};
      fill(gap(seed, sparsity));
    } else if (distribution == "fal") {
      auto shape = parse<double>(param[0]);
      fill(fal(shape));
    } else if (distribution == "cfal") {
      auto shape = parse<double>(param[0]);
      fill(cfal(shape));
    } else if (std::set<std::string>{"file", "fb", "wf", "lognormal"}.count(
                   distribution) == 1) {
      std::ifstream file{param[0]};
      fill(std::vector<Key>{std::istream_iterator<Key>{file},
                            std::istream_iterator<Key>()});
    } else {
      assert(!"No distribution found.");
    }

    sum = std::accumulate(keys.begin(), keys.end(), 0UL);
  }
};

struct Run {
  static auto reverse_index(std::vector<std::string> v) {
    std::map<std::string, long> m;
    int i = 0;
    for (auto s : v)
      m[s] = i++;
    return m;
  }

  static auto load(std::ifstream &&file) {
    auto header = reverse_index(split(read_line(file), '\t'));
    assert(header.size() == 6);
    std::vector<Run> runs;
    for (; file.good();) {
      auto fields = split(read_line(file), '\t');
      if (!file.good())
        break;
      assert(fields.size() == header.size());
      InputParam input_param{.distribution = fields[header["Distribution"]],
                             .param = fields[header["Parameter"]],
                             .n = parse<long>(fields[header["DatasetSize"]]),
                             .record_bytes =
                                 parse<int>(fields[header["RecordSizeBytes"]])};
      runs.emplace_back(input_param, fields[header["SearchAlgorithm"]],
                        parse<int>(fields[header["#threads"]]));
    }
    return runs;
  }

  InputParam input_param;
  std::string name;
  int n_thds;
  bool ok;

  Run(InputParam input_param, std::string name, int n_thds)
      : input_param(input_param), name(name), n_thds(n_thds), ok(true) {}

  template <typename Search, int record_bytes>
  static std::vector<double> measure2(Run &run, const InputBase &input) {
    const auto &inputC = static_cast<const Input<record_bytes> &>(input);
#ifdef INFINITE_REPEAT
    constexpr bool infinite_repeat = true;
#else
    constexpr bool infinite_repeat = false;
#endif
    constexpr int sample_size = 1000;
    const int n_samples = inputC.keys.size() / sample_size;
    auto &queries = inputC.permuted_keys;

    // TODO this can't be a template of a template have to specialize earlier
    // have to specialize in the class itself. Maybe template macros?
    Search search(inputC.keys);

    std::vector<double> ns(n_samples * run.n_thds);
    std::vector<int> subset_indexes(n_samples * run.n_thds);
    auto rng = std::mt19937(42);
    for (auto it = subset_indexes.begin(); it != subset_indexes.end();
         it += n_samples) {
      std::iota(it, it + n_samples, 0);
      if (it != subset_indexes.begin())
        std::shuffle(it, it + n_samples, rng);
    }

#pragma omp parallel default(none)                                             \
    num_threads(run.n_thds) shared(queries, run, search, ns, subset_indexes)
    {
      const int tid = omp_get_thread_num();
      const auto &thread_ns = &ns[tid * n_samples];
      thread_ns[0] = 0.0;
      auto valSum = 0UL;
      for (int sample_index = 0;; sample_index++) {
        if (sample_index == n_samples) {
          if (!infinite_repeat || valSum != inputC.sum)
            break;
          valSum = sample_index = 0;
        }
        int query_index =
            subset_indexes[tid * n_samples + sample_index] * sample_size;

        auto t0 = std::chrono::steady_clock::now();
        for (int i = query_index; i < query_index + sample_size; i++) {
          auto val = search(queries[i]);
          valSum += val;
          assert(val == queries[i]);
        }

        auto t1 = std::chrono::steady_clock::now();
        double ns_elapsed = std::chrono::nanoseconds(t1 - t0).count();
        // thread_ns[0] += ns_elapsed;
        if (!infinite_repeat)
          thread_ns[sample_index] = ns_elapsed / sample_size;
      }
#pragma omp critical
      run.ok = run.ok && valSum == inputC.sum; // Verify correct results.
    }
    return ns;
  }

  template <int record_bytes>
  static auto measure(Run &run, const InputBase &input) {
    using Fn = std::vector<double>(Run &, const InputBase &);
    using fn_tuple = std::tuple<const char *, Fn *>;
    using std::make_tuple;
    constexpr auto algorithm_mapper = std::array<fn_tuple, 25>{
        // Interpolation Search
        make_tuple("is", measure2<is<record_bytes>, record_bytes>),
        // SIP and TIP
        make_tuple("sip", measure2<sip<record_bytes, 8>, record_bytes>),
        make_tuple("tip", measure2<tip<record_bytes, 64>, record_bytes>),
        // Interpolation Sequential
        make_tuple("is-seq", measure2<is_seq<record_bytes>, record_bytes>),
        // Binary Search
        make_tuple("bs", measure2<Binary<record_bytes>, record_bytes>),
        // A simpler version of binary search (keeps tracks of the search
        // interval
        // in a different way.
        make_tuple("b-naive", measure2<BinaryLR<record_bytes>, record_bytes>),
        // The search methods proposed in https://arxiv.org/pdf/1509.05053.pdf
        make_tuple("b-eyt", measure2<b_eyt<record_bytes, false>, record_bytes>),
        make_tuple("b-eyt-p",
                   measure2<b_eyt<record_bytes, true>, record_bytes>),
        // SIP and TIP with different guard sizes
        make_tuple("sip-0", measure2<sip<record_bytes, 0>, record_bytes>),
        make_tuple("sip-16", measure2<sip<record_bytes, 16>, record_bytes>),
        make_tuple("sip-32", measure2<sip<record_bytes, 32>, record_bytes>),
        make_tuple("sip-64", measure2<sip<record_bytes, 64>, record_bytes>),
        make_tuple("sip-128", measure2<sip<record_bytes, 128>, record_bytes>),
        make_tuple("tip-0", measure2<tip<record_bytes, 0>, record_bytes>),
        make_tuple("tip-8", measure2<tip<record_bytes, 8>, record_bytes>),
        make_tuple("tip-16", measure2<tip<record_bytes, 16>, record_bytes>),
        make_tuple("tip-32", measure2<tip<record_bytes, 32>, record_bytes>),
        make_tuple("tip-128", measure2<tip<record_bytes, 128>, record_bytes>),
        make_tuple("tip-256", measure2<tip<record_bytes, 256>, record_bytes>),
        make_tuple("tip-512", measure2<tip<record_bytes, 512>, record_bytes>),
        make_tuple("tip-1024", measure2<tip<record_bytes, 1024>, record_bytes>),
        // Variations of SIP with optimizations disabled.
        make_tuple("sip-no-guard",
                   measure2<sip_no_guard<record_bytes>, record_bytes>),
        make_tuple("sip-fp", measure2<sip_fp<record_bytes>, record_bytes>),
        make_tuple("sip-idiv", measure2<sip_idiv<record_bytes>, record_bytes>),
        make_tuple("sip-no-reuse-16",
                   measure2<sip_no_reuse<record_bytes, 16>, record_bytes>),
    };
    auto it = std::find_if(
        algorithm_mapper.begin(), algorithm_mapper.end(), [run](const auto &x) {
          return std::string(std::get<const char *>(x)) == run.name;
        });
    if (it == algorithm_mapper.end()) {
      std::cerr << "algorithm " << run.name << " not found.";
      assert(!"Algorithm not found");
      return std::vector<double>();
    }
    return std::get<Fn *>(*it)(run, input);
  }

  auto operator()(const InputBase &input) {
    auto[n, distribution, param, record_bytes] = input_param;
    std::vector<double> ns;
    switch (input_param.record_bytes) {
#define CASE(N)                                                                \
  case N:                                                                      \
    ns = measure<N>(*this, input);                                             \
    break
      CASE(8);
      CASE(32);
      CASE(128);
#undef CASE

    default:
      assert(!"record not supported");
    }
    // If not ok then execution failed, due to wrong results of
    // the search algorithm.
    if (!this->ok)
      std::cerr << "Execution failed" << param << ' ' << this->name << '\n';
    return ns;
  }
};

InputBase::InputMap InputBase::load(std::vector<Run> runs) {
  InputMap inputs;
  for (auto r : runs) {
    auto input_param = r.input_param;
    if (inputs.count(input_param) == 0) {
      std::cerr << "Loading Dataset size:" << input_param.n
                << ", distribution: " << input_param.distribution
                << ", distribution parameter: " << input_param.param << '\n';
      auto distribution_param = split(input_param.param);
      inputs.emplace((InputParam::Tuple)input_param, [=]() {
        switch (input_param.record_bytes) {
#define CASE(N)                                                                \
  case N:                                                                      \
    return static_cast<std::unique_ptr<InputBase>>(std::make_unique<Input<N>>( \
        input_param.n, input_param.distribution, distribution_param))
          CASE(8);
          CASE(32);
          CASE(128);
#undef CASE
        default:
          assert(!"record size not supported");
        };
        return std::make_unique<InputBase>();
      }());
    }
  }
  std::cerr << std::endl;
  return inputs;
}

#endif
