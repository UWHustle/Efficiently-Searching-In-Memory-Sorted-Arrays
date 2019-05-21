#include "div.h"

using namespace std;
int main() {
  using l = DivLut::Numerator;
  constexpr int LG_N = 8;
  constexpr int N = 1 << LG_N;

  DivLut dL;

  const int N_TEST = 1 << 10;
  l max2 = 0, sum2 = 0, cnt2 = 0;

  for (int i = 2; i < N; i++) {
    int d = rand();
    // d = i;

    auto d2 = DivLut::Divisor(d);
    auto d3 = DivLut::Divisor(DivLut::Gen(d));
    for (l k = 0; k < N_TEST; k++) {
      l n = rand();
      // n = k;
      l r = n / d;
      l r2 = n / d2;
      l r3 = n / d3;
      l d2 = r - r2, d3 = r - r3;
      max2 = max(max2, d2);
      sum2 += d2;
      // if (0L < d2*d2) {
      printf("%lu / %d = %lu ~ %lu %lu\n", n, d, r, r2, r3);
      cnt2++;
      //}
    }
  }
  printf("max d2 %ld sum d2 %ld cnt d2 %ld\n", max2, sum2, cnt2);
  //#endif

  /*
  printf("p = { 0x%lx", dL3.pT[0] << dL2.lg_qT[0]);
  for (int i = 1; i < N; i++) {
    printf("UL , 0x%lx", dL3.pT[i] << dL2.lg_qT[i]);
  }
  printf("UL };\n");

  printf("p = { 0x%lx", dL2.pT[0]);
  for (int i = 1; i < N; i++) {
    printf("UL , 0x%lx", dL2.pT[i]);
  }
  printf("UL };\n");

  printf("q = { %d", dL2.lg_qT[0]);
  for (int i = 1; i < N; i++) {
    printf(", %d", dL2.lg_qT[i]);
  }
  printf(" };\n");
  */
  return 0;
}
