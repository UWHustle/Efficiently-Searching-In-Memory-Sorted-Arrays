#include "benchmark.h"
#include  "util.h"

#include <vector>

int main(int argc, char *argv[]) {
  // Parse configuration from input
  using RunTuple = std::tuple<DatasetParam::Tuple, std::string, int>;
  std::vector<Run> runs = loadRunsFromFile(std::ifstream(argv[1]));

  // For each dataset output them to st::out
  for (Run r : runs) {
    auto dataset_params = split(r.dataset_param.param);
    for (auto key : Dataset<8>(r.dataset_param.n,
                               r.dataset_param.distribution,
                               dataset_params).keys) {
      std::cout << key << '\n';
    }
  }
  return 0;
}
