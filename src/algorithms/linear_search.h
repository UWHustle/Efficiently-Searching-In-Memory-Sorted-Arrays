#ifndef LIN_H
#define LIN_H

#include "../padded_vector.h"
#include "../util.h"
#include <x86intrin.h>

template <class Vector, int n = 8> class LinearUnroll {
  template <bool reverse = false>
  static int64_t linearSearchUnroll(const Vector &a, int64_t m, Key k) {
    for (;; m = (reverse ? m - n : m + n)) {
      for (int i = 0; i < n; i++) {
        if (reverse ? (a[m - i] <= k) : (a[m + i] >= k)) {
          return reverse ? (m - i) : (m + i);
        }
      }
    }
  }

public:
  static int64_t forward(const Vector &a, const int64_t guessIx, const Key x) {
    return linearSearchUnroll<false>(a, guessIx, x);
  }
  static int64_t reverse(const Vector &a, const int64_t guessIx, const Key x) {
    return linearSearchUnroll<true>(a, guessIx, x);
  }
};

#endif
