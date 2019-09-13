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
using Index = int64_t;

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

#endif
