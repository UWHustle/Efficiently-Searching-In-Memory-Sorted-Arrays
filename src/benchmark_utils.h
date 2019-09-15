#ifndef BENCHMARK_UTILS_H
#define BENCHMARK_UTILS_H

#include "benchmark.h"
#include "datasets.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>


std::string removedoubletabs(std::string s) {
  std::size_t pos = s.find("\t\t");
  while (pos != std::string::npos) {
    s.replace(pos, 2, "\t");
    pos = s.find("\t\t");
  }
  return s;
}

template <typename T> T parse2(std::string s) {
  T t;
  std::stringstream(s) >> t;
  return t;
}

auto read_line(std::ifstream &f) {
  std::string line;
  std::getline(f, line);
  return line;
}

std::vector<std::string> split(std::string s, char delim = ',') {
  std::vector<std::string> v;
  s = removedoubletabs(s);
  for (auto it = s.begin(); it != s.end();) {
    auto start = it;
    it = std::find(it, s.end(), delim);
    v.emplace_back(start, it);
    if (it != s.end())
      it++;
  }
  return v;
}

struct Run;
struct DatasetParam;

static auto reverseRunFileIndex(std::vector<std::string> v) {
  std::map<std::string, long> m;
  int i = 0;
  for (auto s : v)
    m[s] = i++;
  return m;
}

static auto loadRunsFromFile(std::ifstream &&file) {
  auto header = reverseRunFileIndex(split(read_line(file), '\t'));
  assert(header.size() == 6);
  std::vector<Run> runs;
  for (; file.good();) {
    auto fields = split(read_line(file), '\t');
    if (!file.good())
      break;
    assert(fields.size() == header.size());
    DatasetParam dataset_param{.distribution = fields[header["Distribution"]],
        .param = fields[header["Parameter"]],
        .n = parse2<long>(fields[header["DatasetSize"]]),
        .record_bytes =
        parse2<int>(fields[header["RecordSizeBytes"]])};
    runs.emplace_back(dataset_param, fields[header["SearchAlgorithm"]],
                      parse2<int>(fields[header["#threads"]]));
  }
  return runs;
}

static void createDataset(DatasetParam dataset_param,
                          DatasetBase::DatasetMap& input_map) {
  // The dataset does not exist
  if (input_map.count(dataset_param) == 0) {
      std::cerr << "Creating Dataset -> size:" << dataset_param.n
                << ", distribution: " << dataset_param.distribution
                << ", distribution parameter: " << dataset_param.param << '\n';

      auto dataset_params = split(dataset_param.param);
    input_map.emplace((DatasetParam::Tuple)dataset_param, [=]() {
        switch (dataset_param.record_bytes) {
          case 8:
              return static_cast<std::unique_ptr<DatasetBase>>(
                  std::make_unique<Dataset<8>>(
                      dataset_param.n, dataset_param.distribution, dataset_params));
          case 32:
              return static_cast<std::unique_ptr<DatasetBase>>(
                  std::make_unique<Dataset<32>>(
                      dataset_param.n, dataset_param.distribution, dataset_params));
          case 128:
              return static_cast<std::unique_ptr<DatasetBase>>(
                  std::make_unique<Dataset<128>>(
                      dataset_param.n, dataset_param.distribution, dataset_params));
          default:
            assert(!"record size not supported");
        };
        return std::make_unique<DatasetBase>();
      }());
  }
  std::cerr << std::endl;
}

#endif //BENCHMARK_UTILS_H
