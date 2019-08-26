## Efficiently Searching In MemorySorted Arrays
This repo hosts the code used in the paper   
["Efficiently Searching In-Memory Sorted Arrays:Revenge of the Interpolation 
Search?"](http://pages.cs.wisc.edu/~chronis/files/efficiently_searching_sorted_arrays.pdf)
 to be presented at ACM SIGMOD 2019.

## Introduction
We have implemented various search methods and a benchmarking framework that
measures the performance of these methods. We measure the time to search for a
record of a dataset. Each record consists of a key and a payload, the keys are 
8 Byte integers but the payloads can be varied across datasets.
The benchmarking framework checks the results that are returned from each 
search algorithm are correct.

Using the benchmark framework we can easily control the following parameters:
+ size of the dataset  
  (minimum size: 1000 records)
+ distribution of the keys
+ parameter of the distribution (described in detail later)
+ search algorithm  
    (Interpolation Search, SIP, TIP, Binary Search, ...)  
+ record size  
    (each record contains an 8 Byte Key and a Payload, the record size parameter
     specifies the size of the entire record) 
     Valid options : 8, 32, 128
+ number of threads, the search for a single record is handled by one thread.

## Performance Evaluation
To measure the performance of an algorithm for a given dataset, we randomly
permute the keys contained in the dataset and search for all of them, this is
called a run.
We measure the execution time to search subsets of 1,000 keys for each run.
To compare algorithms we use the time required to seacrh for one record.  
The time to search for one record is calculated as an average over the time to 
search each subset.

## How to use
In our paper we used clang5 to compile our code and to replicate our experiments
we provide the option to dowload and use the same version of clang. As an alternative 
you can use gcc.
+ The installation scripts are validated on Ubuntu 16.04 and 18.04

### Compilation with clang5
1) Run the install script "install.sh", which will download clang5, openmp,
   python3 and pandas. clang5 will be downloaded inside this repo.
2) Run "make clang5_searchbench" to produce the "searchbench" executable.

### Compilation with gcc
1) Run the install script "install_noclang.sh", which will download openmp,
   python3 and pandas.
2) Run "make" to produce the "searchbench" executable.

### Usage
The "searchbench" execution expects one argument, a tsv file of
experiments. Each line of the tsv specifies the parameters of one experiment,
as described in  [Introduction](#introduction).

Example "experiments.tsv" :
```bash
$ cat experiments.tsv
DatasetSize Distribution  Parameter SearchAlgorithm RecordSizeBytes #threads
2000        uniform       42        bs              8               1
2000        uniform       42        sip             8               1
```
"searchbench" runs each experiment and reports the time required to search each 
subset of 1000 records, in nanoseconds.
```bash
$ ./searchbench experiments.tsv
Loading Dataset size:2000, distribution: uniform, distribution parameter: 42

Running experiment: 2000 uniform 42 8 bs 1
Run	DatasetSize	Distribution	Parameter	#threads	SearchAlgorithm	RecordSizeBytes	TimeNS	
  0	       2000	     uniform	       42	       1	             bs	              8	130.34	
  0	       2000	     uniform	       42	       1	             bs	              8	120.16	

Running experiment: 2000 uniform 42 8 sip 1
Run	DatasetSize	Distribution	Parameter	#threads	SearchAlgorithm	RecordSizeBytes	TimeNS	
  1	       2000	     uniform	       42	       1	            sip	              8	 77.27	
  1	       2000	     uniform	       42	       1	            sip	              8	65.243
```

We provide a helper function implemented in Python "getTimes.py" that
runs the "searchbench" using as input the file named "experiments.tsv" and reports back for each run
the time to search one record, calculated as described in Section [Performance Evaluation](#performance-evaluation)
This is the easiest way to benchmark different search methods. The getTimes.py can be easily modified to report more
statistics from each experiment.
```bash
$ python3 getTimes.py 
make: 'searchbench' is up to date.

Time to search one record:
Run  DatasetSize  Distribution  Parameter  #threads  SearchAlgorithm  RecordSizeBytes
0    2000              uniform  42         1                      bs  8                  112.695
1    2000              uniform  42         1                     sip  8                   64.251
```


### TSV format
The experiment tsv should include the following header:
```bash
DatasetSize	Distribution	Parameter	SearchAlgorithm	RecordSizeBytes	#threads
```
All the values must be tab separated.

### Datasets
A dataset is identified by its name and one parameter as described in the following table:

| Dataset       | Parameter                                                 |
| ------------- |:-------------:                                            |
| uniform       | seed for the ranfom generator (integer)                   |
| gap           | seed for the ranfom generator (integer), gap parameter (double from 0.1 to 1.0)  |
| fal           | shape parameter (double)                                  |
| cfal          | shape parameter (double)                                  |                   
| file          | path of file                                              |

When the dataset is "file" then the file identified by "path of file" specifies the keys that
will be used in the dataset. When a dataset from a file is used the DatasetSize parameter
does not affect the size of the dataset.The file should contain one key per line. 
Examples of dataset file can be found in the src/datasets folder.

For explanation of the parameters and dataset please refer to our paper:
["Efficiently Searching In-Memory Sorted Arrays:Revenge of the Interpolation 
Search?"](http://pages.cs.wisc.edu/~jignesh/publ/Revenge_of_the_Interpolation_Search.pdf).

#### Generating datasets
All datasets that are not loaded from a file are generated by the benchmarking framework.
We have build a utility that outputs the keys used in the datasets. You can
build this utility using "make dump" and use the utility by executing "dump experiments.tsv".
The output will be the keys of the records of the datasets specified
in the "experiments.tsv".

### Algorithms

The algorithm we have implemented in our code are:

| SearchAlgorithm       | Description                      |
| ------------- |:-------------:                  |
| is            | Interpolation Search            |
| bs            | Binary Search                    |
| sip           | SIP - Slope Reuse Interpolation Search    |
| tip           | TIP - Three Point Interpolation Search    |


### Note
+ If the searchbench is not producing output for an experiment the most probable cause it that a parameter is not
tab separated in the "experiment.tsv"
+ The results of experiments with small datasets can have significant variance due to uncontrollable hardware factors (i.e. caching). Repeating such experiments multiple times is suggested to get stable results.    

## Implementation
Please consult the README in the src folder for more information on how the code is structured.
