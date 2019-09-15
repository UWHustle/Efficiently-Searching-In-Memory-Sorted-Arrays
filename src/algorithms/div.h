#ifndef DIV_H
#define DIV_H

#include <assert.h>
#include <cstdint>

class FixedPoint {
  using u128 = __uint128_t;

  uint64_t numerator;

  constexpr FixedPoint(uint64_t numerator) : numerator(numerator) {}

public:
  class Gen {
    uint64_t numerator;

  public:
    constexpr Gen(uint64_t numerator) : numerator(numerator) {}
    constexpr FixedPoint operator/(uint64_t denominator) {
      u128 fixed_numerator =
          (((u128)numerator << 64) - 1 + denominator) / denominator;

      assert(numerator < denominator);
      assert(fixed_numerator <= ~0UL);

      return fixed_numerator;
    }
  };

  uint64_t operator*(u128 x) const { return (x * numerator) >> 64; }
};

#endif
