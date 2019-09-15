#ifndef TIP_H
#define TIP_H

// Three Point Interpolation Search - TIP Search
template <int record_bytes, int guard_off>
class tip {
  using Vector = PaddedVector<record_bytes>;
  using Linear = LinearUnroll<Vector>;

  const Vector &data;

  //// Interpolator //////////////////
  const Index d;
  const Key y_1;
  const double diff_y_01, a_0, diff_scale, d_a;

  Index interpolate(const Key y, const Index x_0, const Index x_1,
                    const Index x_2) const {
    double y_0 = data[x_0] - y, y_1 = data[x_1] - y, y_2 = data[x_2] - y,
        error = y_1 * (x_1 - x_2) * (x_1 - x_0) * (y_2 - y_0) /
        (y_2 * (x_1 - x_2) * (y_0 - y_1) +
            y_0 * (x_1 - x_0) * (y_1 - y_2));
    return x_1 + (Index)error;
  }

  Index interpolate(const Key x) {
    return d + (Index)(d_a * (y_1 - x) / (diff_scale - x * (a_0 + 1.0)));
  }


  ////////////////////////////////////

  __attribute__((always_inline)) Key linear_search(const Key x, Index y) const {
    if (data[y] >= x) {
      return data[Linear::reverse(data, y, x)];
    } else {
      return data[Linear::forward(data, y + 1, x)];
    }
  }

 public:
  tip(const Vector &data)
      : data(data),
        d((uint64_t)data.size() >> 1),
        y_1(data[d]),
        diff_y_01(data[0] - y_1),
        a_0(diff_y_01 == (y_1 - data.back())
            ? 0.99999999999999
            : diff_y_01 / (double)(y_1 - data.back())),
        diff_scale(data[0] - a_0 * data.back()),
        d_a((1.0 + a_0) * d)
  {
    assert(data.size() >= 1);
  }

  __attribute__((always_inline)) Key search(const Key x) {
    Index left = 0, right = data.size() - 1, next_1 = data.size() >> 1,
        next_2 = interpolate(x);
    while(true) {
      if (next_2 - next_1 <= guard_off && next_2 - next_1 >= -guard_off)
        return linear_search(x, next_2);
      assert(next_1 >= left);
      assert(next_1 <= right);
      assert(next_2 >= left);
      assert(next_2 <= right);
      assert(next_1 != next_2);
      if (data[next_1] != data[next_2]) {
        if (next_1 < next_2) {
          assert(data[next_1] <= x); // f(x) <= f(x') ==> x <= x'
          left = next_1;
        } else {
          assert(data[next_1] >= x); // f(x) >= f(x') ==> x >= x'
          right = next_1;
        }
        if (next_2 + guard_off >= right) {
          auto r = data[Linear::reverse(data, right, x)];
          return r;
        } else if (next_2 - guard_off <= left) {
          auto r = data[Linear::forward(data, left, x)];
          return r;
        }
      }
      next_1 = next_2;

      assert(left < right);
      assert(left >= 0);
      assert(right < data.size());
      assert(next_1 != left);
      assert(next_1 != right);

      next_2 = interpolate(x, left, next_1, right);

      assert(next_2 >= left);
      assert(next_2 <= right);
    }
    return linear_search(x, next_2);
  }
};

#endif //TIP_H
