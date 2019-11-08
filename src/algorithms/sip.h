#ifndef SIP_H
#define SIP_H


// Slope-reuse Interpolation Search - SIP
template <int record_bytes, bool multiple_iterations = true, int guard_off = 8>
class sip {
  using Vector = PaddedVector<record_bytes>;
  using Linear = LinearUnroll<Vector>;

  const Vector &data;

  /////// Interpolator /////////////////////////
  const FixedPoint slope;
  const double f_aL;
  const double f_width_range;

  Index interpolate(const Key x, const Index mid, bool approx = true) {
    return approx ? (x < data[mid] ? mid - slope * (uint64_t)(data[mid] - x)
                                   : mid + slope * (uint64_t)(x - data[mid]))
                  : mid + (Index)(((double)x - (double)data[mid])
                    * f_width_range);
  }

  Index interpolate(const Key x, bool approx = true) {
    return approx ? slope * (uint64_t)(x - data[0])
                  : (Index)(((double)x - f_aL) * f_width_range);
  }

  //////////////////////////////////////////////
 public:
  sip(const Vector &data)
      : data(data),
        slope(FixedPoint::Gen(data.size() - 1) / (data.back() - data[0])),
        f_aL(data[0]),
        f_width_range((double)((uint64_t)data.size() - 1) /
            (double)(data.back() - data[0]))
  {}

  __attribute__((always_inline)) Key search(const Key x) {
    assert(data.size() >= 1);
    // set bounds and do first interpolation
    Index left = 0, right = data.size() - 1, next = interpolate(x);

    for (int i = 0; multiple_iterations; i++) {
      // update bounds and check for match
      if (data[next] < x)
        left = next + 1;
      else if (data[next] > x)
        right = next - 1;
      else
        return data[next];
      if (left == right)
        return data[left];

      // next interpolation
      assert(left < right);
      assert(left >= 0);
      assert(right < data.size());
      next = interpolate(x, next);

      // apply guards
      if (next + guard_off >= right)
        return data[Linear::reverse(data, right, x)];
      else if (next - guard_off <= left)
        return data[Linear::forward(data, left, x)];

      assert(next >= left);
      assert(next <= right);
    }
    // linear search base case
    if (data[next] >= x) {
      return data[Linear::reverse(data, next, x)];
    } else {
      return data[Linear::forward(data, next + 1, x)];
    }

    return 0;
  }

  __attribute__((always_inline)) std::pair<int, int> search_metadata(const Key x) {
    assert(data.size() >= 1);
    // set bounds and do first interpolation
    Index left = 0, right = data.size() - 1, next = interpolate(x);
    int i = 0;
    for (; multiple_iterations; i++) {
      // update bounds and check for match
      if (data[next] < x)
        left = next + 1;
      else if (data[next] > x)
        right = next - 1;
      else
        return {i, 0};
      if (left == right)
        return {i, 0};

      // next interpolation
      assert(left < right);
      assert(left >= 0);
      assert(right < data.size());
      next = interpolate(x, next);

      // apply guards
      if (next + guard_off >= right)
        return {i, LinearUnrollMetadata<Vector>::reverse(data, right, x)};
      else if (next - guard_off <= left)
        return {i, LinearUnrollMetadata<Vector>::forward(data, left, x)};
      assert(next >= left);
      assert(next <= right);
    }
    // linear search base case
    if (data[next] >= x) {
      return {i, LinearUnrollMetadata<Vector>::reverse(data, next, x)};
    } else {
      return {i, LinearUnrollMetadata<Vector>::forward(data, next + 1, x)};
    }

    return {0,0};
  }
};

#endif //SIP_H
