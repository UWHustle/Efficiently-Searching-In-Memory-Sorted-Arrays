#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <assert.h>
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using Key = int64_t;
using SearchFn = int64_t(const Key *, int64_t, Key);

#ifdef NDEBUG
#define ERR(A, args...)
#else
#define ERR(A, args...) fprintf(stderr, A, args)
#endif

#ifndef N_SAMPLES
#define N_SAMPLES 10
#endif

#ifndef SUBSET_SIZE
#define SUBSET_SIZE -1
#endif

constexpr inline unsigned ceil_lg(unsigned x) {
  assert(x >= 2); // subtracting and clz < 1 is undefined.
  return 32 - __builtin_clz(x - 1);
}

constexpr inline long ceil_lg(uint64_t x) {
  assert(x >= 2);
  return 64 - __builtin_clzl(x);
}

constexpr inline long ceil_lg(__uint128_t x) {
  uint64_t hi = x >> 64;
  return x < 2 ? 1 : hi != 0 ? 64 + ceil_lg(hi) : ceil_lg((uint64_t)x);
}

constexpr inline unsigned flr_lg(unsigned x) {
  assert(x >= 1);
  return 32 - __builtin_clz(x);
}

constexpr inline int flr_lgl(uint64_t x) {
  assert(x >= 1); // clz < 1 undefined
  return 64 - __builtin_clzl(x);
}

std::string removedoubletabs(std::string s) {
  std::size_t pos = s.find("\t\t");
  while (pos != std::string::npos) {
    s.replace(pos, 2, "\t");
    pos = s.find("\t\t");
  }
  return s;
}

std::vector<std::string> split(std::string s, char delim = ',') {
  std::vector<std::string> v;
  s = removedoubletabs(s);
  for (auto it = s.begin(); it != s.end();) {
    auto start = it;
    it = std::find(it, s.end(), delim);
    v.emplace_back(start, it);
    if (it != s.end())
      it++;
  }
  return v;
}

auto read_line(std::ifstream &f) {
  std::string line;
  std::getline(f, line);
  return line;
}

#endif
