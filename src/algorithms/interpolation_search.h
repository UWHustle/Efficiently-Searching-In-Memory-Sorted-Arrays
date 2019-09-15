#ifndef INTERPOLATION_SEARCH_H
#define INTERPOLATION_SEARCH_H

#include "div.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cmath>
#include <iostream>
#include <iterator>
#include <tuple>
#include <vector>


// Interpolation Seach - IS
template <int record_bytes>
class InterpolationSearch {
  using Vector = PaddedVector<record_bytes>;
  using Linear = LinearUnroll<Vector>;

  const Vector &data;

  /////// Interpolator /////
  const FixedPoint slope;
  const double f_aL;
  const double f_width_range;

  Index interpolate(const Key x) {
    return (Index)(((double)x - f_aL) * f_width_range);
  }

  Index interpolate(const Key x, const Index left, const Index right) {
    return left +
      ((double)x - (double)(data[left])) / (double)(data[right] - data[left]) *
          (double)(right - left);
  }
  /////////////////////////

 public:
  InterpolationSearch(const Vector &data)
    : data(data),
      slope(FixedPoint::Gen(data.size() - 1) / (data.back() - data[0])),
      f_aL (data[0]),
      f_width_range ((double)((uint64_t)data.size() - 1) /
          (double)(data.back() - data[0]))
  {}

  __attribute__((always_inline)) Key search(const Key x) {
    assert(data.size() >= 1);
    Index left = 0, right = data.size() - 1, next = interpolate(x);
    while(true) {
      if (data[next] < x)
        left = next + 1;
      else if (data[next] > x)
        right = next - 1;
      else
        return data[next];
      if (left == right)
        return data[left];

      assert(left < right);
      assert(left >= 0);
      assert(right < data.size());
      next = interpolate(x, left, right);
      assert(next > -32);
      assert(next < data.size() + 32);
      assert(next >= left);
      assert(next <= right);
    }
    // linear search base case
    if (data[next] >= x) {
      return data[Linear::reverse(data, next, x)];
    } else {
      return data[Linear::forward(data, next + 1, x)];
    }
  }
};

#endif //INTERPOLATION_SEARCH_H
