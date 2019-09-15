#ifndef BENCHMARK_INPUT_H
#define BENCHMARK_INPUT_H

#include "util.h"
#include "padded_vector.h"

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

struct DatasetParam {
  std::string distribution, param;
  long n;
  int record_bytes;

  // Used as a key for an DatasetParam
  using Tuple = std::tuple<std::string, std::string, long, int>;
  operator Tuple() { return Tuple{distribution, param, n, record_bytes}; }
};

struct DatasetBase {
  using DatasetMap =
                std::map<DatasetParam::Tuple, std::unique_ptr<DatasetBase>>;
};


// Parse and create the datasets, generate the keys or import from a file.
template <int record_bytes> struct Dataset : public DatasetBase {
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

  Dataset(const long n, const std::string &distribution,
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

#endif