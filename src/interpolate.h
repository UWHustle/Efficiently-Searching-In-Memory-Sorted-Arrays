#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include "div.h"
#include "lin.h"
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
template <int record_bytes> class IBase {
public:
  using Vector = PaddedVector<record_bytes>;
  using Index = int64_t;

  static constexpr int Recurse = -1;
  static constexpr bool Precompute = true;
  static constexpr bool Intercept = true;

  // Liner interpolation function using floating point arithmetic.
  // appx flag controls if fixed point arithmetic is used.
  template <bool appx = true> struct Float {
    Float(const Vector &a)
        : A(a), slope(FixedPoint::Gen(A.size() - 1) / (A.back() - A[0])),
          f_aL(A[0]), f_width_range((double)((uint64_t)A.size() - 1) /
                                    (double)(A.back() - A[0])) {}

    const Vector &A;
    const FixedPoint slope;
    const double f_aL;
    const double f_width_range;

    Index operator()(const Key x, const Index left, const Index right) {
      return left +
             ((double)x - (double)(A[left])) / (double)(A[right] - A[left]) *
                 (double)(right - left);
    }

    Index operator()(const Key x, const Index mid) {
      return appx ? (x < A[mid] ? mid - slope * (uint64_t)(A[mid] - x)
                                : mid + slope * (uint64_t)(x - A[mid]))
                  : mid + (Index)(((double)x - (double)A[mid]) * f_width_range);
    }

    Index operator()(const Key x) {
      return appx ? slope * (uint64_t)(x - A[0])
                  : (Index)(((double)x - f_aL) * f_width_range);
    }
  };

  // Linear interpolation function that uses interger arithmetic.
  struct IntDiv {
    IntDiv(const Vector &a)
        : A(a), i_range_width((A.back() - A[0]) / ((uint64_t)A.size() - 1)) {}

    const Vector &A;
    Key i_range_width;

    Index operator()(const Key x, const Index left, const Index right) {
      return left + (x - A[left]) / ((A[right] - A[left]) / (right - left));
    }
    Index operator()(const Key x, const Index mid) {
      return mid + (x - A[mid]) / i_range_width;
    }
    Index operator()(const Key x) { return (x - A[0]) / i_range_width; }
  };

  // Three point interpolation function.
  template <bool t = false, bool appx = false> struct ThreePointInterpolation {
    ThreePointInterpolation (const Vector &a)
        : A(a), d((uint64_t)A.size() >> 1), y_1(A[d]), diff_y_01(A[0] - y_1),
          a_0(diff_y_01 == (y_1 - A.back())
                  ? 0.99999999999999
                  : diff_y_01 / (double)(y_1 - A.back())),
          diff_scale(A[0] - a_0 * A.back()), d_a((1.0 + a_0) * d) {}
    const Vector &A;
    const Index d;
    const Key y_1;
    const double diff_y_01, a_0, diff_scale, d_a;
    // The same function implemented in python.
    /*
     * def ThreePointInterpolation(points, y_star):
         x, y = zip(*[(x, y - y_star) for x, y in points])
         error = (x[1] - x[2]) * (x[1] - x[0]) * y[1] * (y[2] - y[0]) / (
             (x[1] - x[2]) * (y[0] - y[1]) * y[2] + (x[1] - x[0]) * (y[1] -
     y[2]) * y[0])
         return x[1] + error
     */
    Index operator()(const Key y, const Index x_0, const Index x_1,
                     const Index x_2) const {
      double y_0 = A[x_0] - y, y_1 = A[x_1] - y, y_2 = A[x_2] - y,
             error = y_1 * (x_1 - x_2) * (x_1 - x_0) * (y_2 - y_0) /
                     (y_2 * (x_1 - x_2) * (y_0 - y_1) +
                      y_0 * (x_1 - x_0) * (y_1 - y_2));
      return x_1 + (Index)error;
    }

    Index operator()(const Key x) {
      return d + (Index)(d_a * (y_1 - x) / (diff_scale - x * (a_0 + 1.0)));
    }
  };

protected:
  // The array to search.
  const Vector &A;


  IBase(const Vector &v) : A(v) {}
};

// Implementation of the search algoritms.
// The templates control the size of the record to search, the interpolation
// function to use and the size of the guard.

// Interpolation Seach - IS
template <int record_bytes,
          class Interpolate = typename IBase<record_bytes>::template Float<>,
          int nIter = IBase<record_bytes>::Recurse, int guard_off = 16,
          bool min_width = false>
class InterpolationSearch : public IBase<record_bytes> {
  using Super = IBase<record_bytes>;
  using Vector = typename Super::Vector;
  using typename Super::Index;
  using Super::A;
  using Linear = LinearUnroll<Vector>;

  Interpolate interpolate;

public:
  InterpolationSearch(const Vector &v) : Super(v), interpolate(A) {}

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
template <int record_bytes, int nIter = IBase<record_bytes>::Recurse,
          class Interpolate = typename IBase<record_bytes>::template Float<>,
          int guard_off = 8>
class SIPSearch : public IBase<record_bytes> {
  using Super = IBase<record_bytes>;
  using Vector = typename Super::Vector;
  using typename Super::Index;
  using Super::A;
  using Super::Recurse;
  using Linear = LinearUnroll<Vector>;

  Interpolate interpolate;

public:
  SIPSearch(const Vector &v) : Super(v), interpolate(A) {}

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
    class Interpolate = typename IBase<record_bytes>::template ThreePointInterpolation<>>
class tip : public IBase<record_bytes> {
  using Super = IBase<record_bytes>;
  using Vector = typename Super::Vector;
  using typename Super::Index;
  using Super::A;
  using Linear = LinearUnroll<Vector>;
  static constexpr int nIter = Super::Recurse;
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
  tip(const Vector &v) : Super(v), interpolate(A) { assert(A.size() >= 1); }

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

/*
 * is : Vanilla Interpolation Search
 * sip : SIP
 * is_seq : Interpolation-Sequential
 * sip_no_reuse: Don't re-use slope
 * sip_no_guard : SIP with no guard (guard = 0)
 * sip_fp : use FP division
 * sip_idiv : use int division
 * tip : Three Point Interpolation Search
 */
template <int record_bytes>
using is = InterpolationSearch<record_bytes,
                         typename IBase<record_bytes>::template Float<false>,
                         IBase<record_bytes>::Recurse, -1>;
template <int RECORD, int GUARD>
using sip = SIPSearch<RECORD, IBase<RECORD>::Recurse,
                               typename IBase<RECORD>::template Float<>, GUARD>;
template <int RECORD>
using is_seq = SIPSearch<RECORD, 1>;

template <int RECORD, int GUARD>
using sip_no_reuse =
  InterpolationSearch<RECORD, typename IBase<RECORD>::template Float<>,
                  IBase<RECORD>::Recurse, GUARD>;
template <int RECORD>
using sip_no_guard =
  SIPSearch<RECORD, IBase<RECORD>::Recurse,
                       typename IBase<RECORD>::template Float<>, -1>;
template <int RECORD>
using sip_fp = SIPSearch<RECORD, IBase<RECORD>::Recurse,
  typename IBase<RECORD>::template Float<false>>;

template <int RECORD>
using sip_idiv = SIPSearch<RECORD, IBase<RECORD>::Recurse,
                                    typename IBase<RECORD>::IntDiv>;

#endif
