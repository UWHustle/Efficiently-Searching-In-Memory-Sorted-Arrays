#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include "div.h"
#include "linear_search.h"
#include "padded_vector.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cmath>
#include <iostream>
#include <iterator>
#include <tuple>
#include <vector>

// The base for all interpolation based search algorithms.
// Implements the interpolation functions used in the search algorithms.
template <int record_bytes> class InterpolationBase {
public:
  using Vector = PaddedVector<record_bytes>;
  using Index = int64_t;

  static constexpr int Recurse = -1;

protected:
  // The array to search.
  const Vector &A;

  InterpolationBase(const Vector &v) : A(v) {}
};

// Implementation of the search algoritms.
// The templates control the size of the record to search, the interpolation
// function to use and the size of the guard.

// Interpolation Seach - IS
template <int record_bytes,
          class Interpolate = typename InterpolationBase<record_bytes>::template Float<>,
          int nIter = InterpolationBase<record_bytes>::Recurse, int guard_off = 16,
          bool min_width = false>
class InterpolationSearch : public InterpolationBase<record_bytes> {
  using InterBase = InterpolationBase<record_bytes>;
  using Vector = typename InterBase::Vector;
  using typename InterBase::Index;
  using InterBase::A;
  using Linear = LinearUnroll<Vector>;

  Interpolate interpolate;

public:
  InterpolationSearch(const Vector &v) : InterBase(v), interpolate(A) {}

  __attribute__((always_inline)) Key operator()(const Key x) {
    assert(A.size() >= 1);
    Index left = 0, right = A.size() - 1, next = interpolate(x);
    for (int i = 1; nIter < 0 ? true : i < nIter; i++) {
      if (A[next] < x)
        left = next + 1;
      else if (A[next] > x)
        right = next - 1;
      else
        return A[next];
      if (min_width) {
        if (right - left <= guard_off)
          return A[Linear::reverse(A, right, x)];
      } else if (left == right)
        return A[left];

      assert(left < right);
      assert(left >= 0);
      assert(right < A.size());
      next = interpolate(x, left, right);
      assert(next > -32);
      assert(next < A.size() + 32);

      if (guard_off >= 0 && next + guard_off >= right) {
        auto r = A[Linear::reverse(A, right, x)];
        return r;
      } else if (guard_off >= 0 && next - guard_off <= left) {
        auto r = A[Linear::forward(A, left, x)];
        return r;
      }
      assert(next >= left);
      assert(next <= right);
    }
    // linear search base case
    if (A[next] >= x) {
      return A[Linear::reverse(A, next, x)];
    } else {
      return A[Linear::forward(A, next + 1, x)];
    }
  }
};

// Slope-reuse Interpolation Search - SIP
template <int record_bytes, int nIter = InterpolationBase<record_bytes>::Recurse,
          class Interpolate = typename InterpolationBase<record_bytes>::template Float<>,
          int guard_off = 8>
class SIPSearch : public InterpolationBase<record_bytes> {
  using InterBase = InterpolationBase<record_bytes>;
  using Vector = typename InterBase::Vector;
  using typename InterBase::Index;
  using InterBase::A;
  using InterBase::Recurse;
  using Linear = LinearUnroll<Vector>;

  Interpolate interpolate;

public:
  SIPSearch(const Vector &v) : InterBase(v), interpolate(A) {}

  // TODO replace with flatten?
  __attribute__((always_inline)) Key operator()(const Key x) {
    assert(A.size() >= 1);
    // set bounds and do first interpolation
    Index left = 0, right = A.size() - 1, next = interpolate(x);

    for (int i = 1; (nIter < 0 ? true : i < nIter); i++) {
      // update bounds and check for match
      if (A[next] < x)
        left = next + 1;
      else if (A[next] > x)
        right = next - 1;
      else
        return A[next];
      if (left == right)
        return A[left];

      // next interpolation
      assert(left < right);
      assert(left >= 0);
      assert(right < A.size());
      next = interpolate(x, next);

      if (nIter == Recurse) {
        // apply guards
        if (guard_off == -1)
          next = std::min(std::max(left, next), right);
        else {
          if (next + guard_off >= right)
            return A[Linear::reverse(A, right, x)];
          else if (next - guard_off <= left)
            return A[Linear::forward(A, left, x)];
        }
        assert(next >= left);
        assert(next <= right);
      }
    }
    // linear search base case
    if (A[next] >= x) {
      return A[Linear::reverse(A, next, x)];
    } else {
      return A[Linear::forward(A, next + 1, x)];
    }

    return 0;
  }
};

// Three Point Interpolation Search - TIP Search
template <int record_bytes, int guard_off,
    class Interpolate = typename InterpolationBase<record_bytes>::template ThreePointInterpolation<>>
class tip : public InterpolationBase<record_bytes> {
  using InterBase = InterpolationBase<record_bytes>;
  using Vector = typename InterBase::Vector;
  using typename InterBase::Index;
  using InterBase::A;
  using Linear = LinearUnroll<Vector>;
  static constexpr int nIter = InterBase::Recurse;
  static constexpr bool min_width = false;

  Interpolate interpolate;

  __attribute__((always_inline)) Key linear_search(const Key x, Index y) const {
    if (A[y] >= x) {
      return A[Linear::reverse(A, y, x)];
    } else {
      return A[Linear::forward(A, y + 1, x)];
    }
  }

 public:
  tip(const Vector &v) : InterBase(v), interpolate(A) { assert(A.size() >= 1); }

  __attribute__((always_inline)) Key operator()(const Key x) {
    Index left = 0, right = A.size() - 1, next_1 = A.size() >> 1,
        next_2 = interpolate(x);
    for (int i = 1; nIter < 0 || i < nIter; i++) {
      if (next_2 - next_1 <= guard_off && next_2 - next_1 >= -guard_off)
        return linear_search(x, next_2);
      assert(next_1 >= left);
      assert(next_1 <= right);
      assert(next_2 >= left);
      assert(next_2 <= right);
      assert(next_1 != next_2);
      if (A[next_1] != A[next_2]) {
        if (next_1 < next_2) {
          assert(A[next_1] <= x); // f(x) <= f(x') ==> x <= x'
          left = next_1;
        } else {
          assert(A[next_1] >= x); // f(x) >= f(x') ==> x >= x'
          right = next_1;
        }
        if (next_2 + guard_off >= right) {
          auto r = A[Linear::reverse(A, right, x)];
          return r;
        } else if (next_2 - guard_off <= left) {
          auto r = A[Linear::forward(A, left, x)];
          return r;
        }
      }
      next_1 = next_2;

      assert(left < right);
      assert(left >= 0);
      assert(right < A.size());
      assert(next_1 != left);
      assert(next_1 != right);

      next_2 = interpolate(x, left, next_1, right);

      assert(next_2 >= left);
      assert(next_2 <= right);
    }
    return linear_search(x, next_2);
  }
};

template <int record_bytes>
using is = InterpolationSearch<record_bytes,
        typename InterpolationBase<record_bytes>::template Float<false>,
        InterpolationBase<record_bytes>::Recurse, -1>;
template <int RECORD, int GUARD>
using sip = SIPSearch<RECORD, InterpolationBase<RECORD>::Recurse,
typename InterpolationBase<RECORD>::template Float<>, GUARD>;

#endif
