# This file includes helper function that create configuration files for
# our benchmarking framework.
from os import path, remove

repeat_times = {10 ** 3: 10000, 10 ** 4: 1000, 10 ** 5: 1000, 10 ** 6: 5,
                10 ** 7: 5, 10 ** 8: 1, 10 ** 9: 1}


def validate_args(datasetSize, distribution, parameter, searchAlgo,
                  recordSizeBytes, nThreads):
    if recordSizeBytes not in [8, 32, 128]:
        print("The valid record size options are 8,32,128")
        exit()


# Adds a experiment to the tsv configuration file. Each experiment described the dataset
# that is going to be searched and which search algorithm is going to be used.
# Input Args:
#             tsvpath: the path of the tsvfile
#             datasetSize: number of records of the dataset to be searched
#             distribution: distribution of the keys of the dataset to be searched
#             parameter:    datasets parameter
#             searchAlgo: name of the search algorithm to use
#             recordSizeBytes: size of record (key + payload)
#             nThreads: number of threads to be used for search
def add_to_tsv(tsvpath, datasetSize, distribution, parameter, searchAlgo,
               recordSizeBytes, nThreads):
    validate_args(datasetSize, distribution, parameter, searchAlgo,
                  recordSizeBytes, nThreads)
    if not path.exists("experiments_configurations/" + tsvpath):
        with open("experiments_configurations/" + tsvpath, "w") as f:
            f.write(
                "DatasetSize\tDistribution\tParameter\tSearchAlgorithm\tRecordSizeBytes\t#threads\n")
    with open("experiments_configurations/" + tsvpath, "a") as f:
        conf = str(datasetSize) + "\t" + \
               str(distribution) + "\t" + \
               str(parameter) + "\t" + \
               str(searchAlgo) + "\t" + \
               str(recordSizeBytes) + "\t" + \
               str(nThreads) + "\n"
        times = repeat_times[datasetSize]
        for i in range(0, times):
            f.write(conf)


def UaR_to_tsv(tsvpath, datasetSize, searchAlgo, recordSizeBytes, nThreads,
               parameter=42):
    add_to_tsv(tsvpath, datasetSize, "uniform", parameter, searchAlgo,
               recordSizeBytes, nThreads)


def fbids_to_tsv(tsvpath, searchAlgo, recordSizeBytes, nThreads):
    add_to_tsv(tsvpath, 10 ** 5, "file", "src/datasets/fb/fb-289000.txt",
               searchAlgo,
               recordSizeBytes, nThreads)

def fbids1k_to_tsv(tsvpath, searchAlgo, recordSizeBytes, nThreads):
    add_to_tsv(tsvpath, 10 ** 5, "file", "src/datasets/fb/fb-1000.txt",
               searchAlgo,
               recordSizeBytes, nThreads)


def fbids10k_to_tsv(tsvpath, searchAlgo, recordSizeBytes, nThreads):
    add_to_tsv(tsvpath, 10 ** 5, "file", "src/datasets/fb/fb-10000.txt",
               searchAlgo,
               recordSizeBytes, nThreads)

def fbids100k_to_tsv(tsvpath, searchAlgo, recordSizeBytes, nThreads):
    add_to_tsv(tsvpath, 10 ** 5, "file", "src/datasets/fb/fb-100000.txt",
               searchAlgo,
               recordSizeBytes, nThreads)


def freq1_to_tsv(tsvpath, searchAlgo, recordSizeBytes, nThreads):
    add_to_tsv(tsvpath, 10 ** 6, "file", "src/datasets/wf/wiki.txt", searchAlgo,
               recordSizeBytes, nThreads)


def freq2_to_tsv(tsvpath, searchAlgo, recordSizeBytes, nThreads):
    add_to_tsv(tsvpath, 10 ** 5, "file", "src/datasets/wf/newman.txt",
               searchAlgo,
               recordSizeBytes, nThreads)


def fal_to_tsv(tsvpath, searchAlgo, recordSizeBytes, nThreads, parameter,
               datasetSize):
    add_to_tsv(tsvpath, datasetSize, "fal", parameter, searchAlgo,
               recordSizeBytes, nThreads)


def cfal_to_tsv(tsvpath, searchAlgo, recordSizeBytes, nThreads, parameter,
                datasetSize):
    add_to_tsv(tsvpath, datasetSize, "cfal", parameter, searchAlgo,
               recordSizeBytes, nThreads)


def gap_to_tsv(tsvpath, searchAlgo, recordSizeBytes, nThreads, random_seed,
               shape, datasetSize):
    add_to_tsv(tsvpath, datasetSize, "gap", str(random_seed) + "," + str(shape),
               searchAlgo, recordSizeBytes, nThreads)


def rm_tsv(tsvpath):
    if path.exists("./experiments_configurations/" + tsvpath):
        remove("./experiments_configurations/" + tsvpath)
