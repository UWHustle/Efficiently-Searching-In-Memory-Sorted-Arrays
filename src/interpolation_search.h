#ifndef INTERPOLATION_SEARCH_H
#define INTERPOLATION_SEARCH_H

#include "interpolate.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cmath>
#include <iostream>
#include <iterator>
#include <tuple>
#include <vector>

// Interpolation Seach - IS

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

#endif //INTERPOLATION_SEARCH_H
