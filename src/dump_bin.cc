#include "benchmark.h"
#include  "util.h"

#include <vector>

int main(int argc, char *argv[]) {
  // Parse configuration from input
  using RunTuple = std::tuple<DatasetParam::Tuple, std::string, int>;
  std::vector<Run> runs = loadRunsFromFile(std::ifstream(argv[1]));

  for (Run r : runs) {
    std::string filename = r.dataset_param.distribution + "_"
        + r.dataset_param.param + "_"
        + std::to_string(r.dataset_param.n) + "_sorted.bin";

    std::cout << "Writing: " << filename << std::endl;
    auto dataset_params = split(r.dataset_param.param);
    std::ofstream out(filename, std::ios_base::trunc | std::ios::binary);
    if (!out.is_open()) {
      std::cerr << "Unable to open " << filename << std::endl;
      exit(EXIT_FAILURE);
    }

    const uint64_t size = r.dataset_param.n;
    out.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));

    for (auto key : Dataset<8>(r.dataset_param.n,
                               r.dataset_param.distribution,
                               dataset_params).keys) {

      out.write(reinterpret_cast<const char*>(&key), sizeof(Key));
    }
    out.close();

  }
  return 0;
}
