#ifndef BIN_H
#define BIN_H

#include <assert.h>
#include <cinttypes>

#include "lin.h"
#include "padded_vector.h"
#include "util.h"

#include <limits>

// Two implemetations of binary search, BinaryLR and Binary.
// The two implementation mainly differ in how hey maintain the search interval
// They template controls the records size to search similarly to the
// IS, SIP, TIP but also BS specific parameters.

template <int record_bytes = 8, bool RETURN_EARLY = true, bool TEST_EQ = true,
          bool FOR = false, int MIN_EQ_SZ = 1>
class BinaryLR {
  using Index = unsigned long;
  using Vector = PaddedVector<record_bytes>;
  using Linear = LinearUnroll<Vector>;

  const Vector &A;
  int lg_v;

public:
  BinaryLR(const Vector &_a) : A(_a) {
    lg_v = 0;
    for (auto n = A.size(); n > MIN_EQ_SZ; n -= (n / 2))
      lg_v++;
  }
  __attribute__((always_inline)) Key operator()(const Key x) {
    // use pointer
    Index l = 0;
    Index r = A.size();

    for (int i = 0; FOR ? i < lg_v : r - l > 1; i++) {
      assert(l <= r);     // ordering check
      assert(l + r >= r); // overflow check
      Index m = (l + r) / 2;
      if (TEST_EQ) {
        if (A[m] < x) {
          l = m + 1;
        } else if (A[m] > x) {
          r = m;
        } else {
          if (RETURN_EARLY)
            return A[m];
          l = r = m;
        }
      } else {
        if (A[m] <= x)
          l = m;
        else
          r = m;
      }
    }
    if (MIN_EQ_SZ == 1) {
      return A[l];
    }

    Index guess = (l + r) / 2;
    if (A[guess] < x)
      return A[Linear::forward(A, guess + 1, x)];
    else
      return A[Linear::reverse(A, guess, x)];
  }
};

template <int record_bytes = 8, bool RETURN_EARLY = false, bool TEST_EQ = false,
          bool FOR = true, bool POW_2 = false, int MIN_EQ_SZ = 32,
          typename Index = unsigned long>
class Binary {
  using Vector = PaddedVector<record_bytes>;
  using Linear = LinearUnroll<Vector>;

  const Vector &A;
  int lg_v, lg_min;

public:
  Binary(const Vector &_a) : A(_a) {
    lg_v = lg_min = 0;
    for (auto n = A.size(); n > 1; n -= (n / 2)) {
      lg_v++;
      if (n > MIN_EQ_SZ)
        lg_min++;
    }
  }

  __attribute__((always_inline)) Key operator()(const Key x) {
    Index n = A.size();
    Index left = 0L;
    if (POW_2) {
      Index mid = n - (1UL << (lg_v - 1));
      left = A[mid] <= x ? mid : left;
      n -= mid;
    }
// TODO how to set unroll = 4 if MIN_EQ_SZ > 1?, else 8?
#define LOOP                                                                   \
  assert(left + n == A.size() || A[left + n] > x);                             \
  assert(A[left] <= x);                                                        \
  Index half = n / 2;                                                          \
  if (TEST_EQ) {                                                               \
    if (x < A[left + half]) {                                                  \
      n = half;                                                                \
    } else if (A[left + half] < x) {                                           \
      left = left + half + 1;                                                  \
      n = n - half - 1;                                                        \
    } else {                                                                   \
      if (RETURN_EARLY)                                                        \
        return A[left + half];                                                 \
      left += half;                                                            \
      if (FOR)                                                                 \
        i = lg_min;                                                            \
      else                                                                     \
        n = 0;                                                                 \
    }                                                                          \
  } else {                                                                     \
    left = A[left + half] <= x ? left + half : left;                           \
    if (POW_2)                                                                 \
      n = half;                                                                \
    else                                                                       \
      n -= half;                                                               \
  }
    if (MIN_EQ_SZ == 1) {
#pragma unroll(8)
      for (int i = POW_2 ? 1 : 0; FOR ? i < lg_min : n > MIN_EQ_SZ; i++) {
        LOOP
      }
    } else {
#pragma unroll(4)
      for (int i = POW_2 ? 1 : 0; FOR ? i < lg_min : n > MIN_EQ_SZ; i++) {
        LOOP
      }
    }
    if (MIN_EQ_SZ == 1)
      return A[left];

    Index guess = left + n / 2;
    if (A[guess] < x)
      return A[Linear::forward(A, guess + 1, x)];
    else
      return A[Linear::reverse(A, guess, x)];
  }
};

#endif
