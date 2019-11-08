#ifndef BIN_EYT_H
#define BIN_EYT_H

// The search methods proposed in https://arxiv.org/pdf/1509.05053.pdf

template <int record_bytes = 8, bool prefetch = false,
    typename Index = unsigned long>
class b_eyt {
  using Vector = PaddedVector<record_bytes>;
  using Linear = LinearUnroll<Vector>;

  const Vector A;
  static const Index multiplier = 64 / sizeof(record_bytes);
  static const Index offset = multiplier + multiplier / 2;

  template <typename T, typename ForwardIterator>
  ForwardIterator copy_data(ForwardIterator a0, size_t i, T &out) {
    if (i >= out.size())
      return a0;

    // visit left child
    a0 = copy_data(a0, 2 * i + 1, out);

    // put data at the root
    out[i] = *a0++;

    // visit right child
    a0 = copy_data(a0, 2 * i + 2, out);

    return a0;
  }

  template <typename T> T eytzinger_array(T in) {
    T rv(in.size());
    copy_data(in.begin(), 0, rv);
    return rv;
  }

 public:
  b_eyt(const Vector &_a) : A(eytzinger_array(_a)) {}

  // Branch-free code with or without prefetching
  __attribute__((always_inline)) Key search(const Key x) {
    Index i = 0;
    while (i < A.size()) {
      if (prefetch)
        __builtin_prefetch(&A[0] + (multiplier * i + offset));
      i = (x <= A[i]) ? (2 * i + 1) : (2 * i + 2);
    }
    Index j = (i + 1) >> __builtin_ffs(~(i + 1));
    return A[j - 1];
  }
};

#endif