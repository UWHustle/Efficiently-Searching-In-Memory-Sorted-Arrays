#ifndef PADDED_VECTOR_H
#define PADDED_VECTOR_H

#include "util.h"
#include <limits>
#include <vector>

template <int record_bytes = 128, int pad = 32> class PaddedVector {
  static constexpr int payload_bytes = record_bytes - sizeof(Key);
  using Payload = char[payload_bytes];
  struct Record {
    Key k;
    Payload p;
    Record() {}
    Record(Key k) : k(k) {}
    bool operator<(const Record &r) const { return k < r.k; }
    operator Key () const { return k; }
  };

  std::vector<Record> v;

public:
  PaddedVector(size_t n) : v(n + 2 * pad) {
    std::fill(this->v.begin(), this->v.begin() + pad,
              std::numeric_limits<Key>::min());
    std::fill(this->v.end() - pad, this->v.end(),
              std::numeric_limits<Key>::max());
  }
  PaddedVector(const std::vector<Key> &v) : v(v.size() + 2 * pad) {
    std::copy(v.begin(), v.end(), this->v.begin() + pad);
    std::fill(this->v.begin(), this->v.begin() + pad,
              std::numeric_limits<Key>::min());
    std::fill(this->v.end() - pad, this->v.end(),
              std::numeric_limits<Key>::max());
  }
  Key &operator[](long ix) {
    // allow some inaccuracy to reduce needed precision
    assert(ix >= -pad);
    assert(ix <= size() + pad);
    return v[ix + pad].k;
  }
  const Key &operator[](long ix) const {
    // allow some inaccuracy to reduce needed precision
    assert(ix >= -pad);
    assert(ix <= size() + pad);
    return v[ix + pad].k;
  }
  auto begin() { return v.begin() + pad; }
  auto end() { return v.end() - pad; };
  const Key *cbegin() const { return v.data() + pad; }
  size_t size() const { return v.size() - 2 * pad; }
  Key back() const { return (*this)[size() - 1]; }
  auto get_pad() const { return pad; }
};

#endif
