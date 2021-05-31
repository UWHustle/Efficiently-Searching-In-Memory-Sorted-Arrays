#include "benchmark.h"
#include  "util.h"
#include <sstream>
#include <string>
#include <fstream>

#include <vector>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cout << "Wrong number of args <txtfile, size>\n";
    return 0;
  }

  std::string filename = argv[1];
  std::string new_filename = filename.substr(0, filename.find_last_of(".")) + ".bin";
  uint64_t size = std::stoi(argv[2]);

  std::cout << "Converting " << size << " keys: " << filename << " -> "
            << new_filename << std::endl;

  std::ifstream infile(filename);
  std::ofstream out(new_filename, std::ios_base::trunc | std::ios::binary);
  if (!infile.is_open() || !out.is_open()) {
    std::cerr << "Unable to open " << filename << std::endl;
    exit(EXIT_FAILURE);
  }

  out.write(reinterpret_cast<const char *>(&size), sizeof(uint64_t));

  Key key;
  std::string line;
  for (int i = 0; i < size; ++i) {
    if (i % 100000 == 0) {
      std::cout << "Writen: " << i << std::endl;
    }
    std::getline(infile, line);
    key = std::stol(line);
    out.write(reinterpret_cast<const char *>(&key), sizeof(Key));
  }

  infile.close();
  out.close();

  return 0;
}
