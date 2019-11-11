## Reproducibility Instructions for:
####  Efficiently Searching In-Memory Sorted Arrays: Revenge of the Interpolation Search?

###### DOI: 10.1145/3299869.3300075

### Introduction

In our paper, we introduced SIP (Slope Re-use Interpolation) and TIP (Three-Point Interpolation), two algorithms for searching sorted in-memory arrays. We compare SIP and TIP to Binary Search. The evaluation presented in our paper compares the algorithm while it varies the size of the arrays we search, the distribution of the search keys and the size of the attached payload of each record contained in the array (key and payload).

### Benchmarking framework

We created a benchmarking framework to experiment with the different search algorithms and easily vary crucial parameters (dataset size, distribution of keys, distribution, search algorithm, record size, #threads used). We have released this framework on GitHub. The benchmarking framework includes the datasets or data generators for all the datasets used in our paper.

Performing new experiments by adding new datasets and modifying the parameters is a straightforward process. Additional search algorithms can be added to the framework.

### Reproduce Paper Experiments

We have created a set of scripts that create the configuration files that our benchmark accepts, 
run the experiments, process the results and create the figures presented in our paper. We have gathered these scripts in this folder.

### Scripts

In the reproduce_experiments folder, we include the following scripts:

##### All scripts should be run from withing the reproduce_experiments directory

| Script Name   | Function       |
| ------------- |:-------------: |
|  install.sh   |  Installs all necessary packages for the benchmarking framework and to reproduce the experiments. Compiles the benchmarking framework. Note: One of the packages installed requires manual input during installation.
| create_configurations.sh | Creates all the experiment configurations (tsv) files, used by the benchmarking framework. The configuration files are stored in the experiments_configurations folder. Each configuration file is named fig<id>.tsv. The code that creates the configuration files can be found inside the same folder.|
| run_experiments.sh | Runs the experiments specified by the configuration files stored in the experiments_configurations folder. Saves the results in the experiments_results folder. The code that runs the experiments is located in the same folder. The results are saved in files named <configuration_file_name>.results. If the results files for an experiment exists the experiment is skipped until the result file is deleted. |
| create_figures.sh | Processes the results of the experiments and produces the figures used in our paper. The figures are stored in the figures directory. The code to produce the figures is contained in a jupyter notebook named CreateFigures.ipynb. This scripts runs the code and produces the figures without the need to open the notebook. |
| master_script.sh | Runs all of the above scripts |
| clean.sh | Deletes the produced experiment configurations, results, and produced figures. |
| start_notebook.sh | Starts the jupyter notebook that creates the figures based on the experiment results. | 

##### Notes
+ The datasets used in our experiments are produced during execution by our benchmarking framework or they are located in the src/datasets folder of the repository. Our setup runs all the experiments one by one. The benchmarking framework provides the option to export the datasets it generates.
+ We run each experiment multiple times to ensure stable results, as we describe in our paper. 
+ We only use one seed per each randomly generated dataset, the results in our paper use multiple seeds to reduce the effects of randomness. 



#### System Requirements
Our experiments require 60 GB of free main memory. The number of threads used is controlled by the experiment but is usually 1 and at all times less than 32. We have tested our scripts using Ubuntu (16.04, 18.04) and Mac ( > 10.12) operating systems. 

#### Hardware Used
For our paper, we used a CloudLab (www.cloudlab.us) hosted server with two Intel(R) Xeon(R) CPU E5-2630 v3 running at 2.40GHz with 20MB ofL3 cache, based on the Haswell architecture, and 128GB of main memory. The CPU Governor setting is set to the "Performance" option. 

#### Running Time
The experiments require 14 hours to run using the hardware described. We offer the option to reduce the size of the datasets used. This can be achieved by modifying the return value of the fullConfiguration() method to False, located in the:
experiments_configurations/create_configurations.py file.
The experiments with the reduced dataset sizes require 1 hour to run and 3 GB of main memory, but not provide the complete results presented in our paper.
#### Results
The figures produced by the scripts we provide are named after their position in the paper. 

