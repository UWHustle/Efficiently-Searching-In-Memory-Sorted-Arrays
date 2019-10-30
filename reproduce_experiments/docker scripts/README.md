## Reproducability of Results
This directory provides the necessary tools to reproduce the experiments presentes at our paper   
["Efficiently Searching In-Memory Sorted Arrays:Revenge of the Interpolation 
Search?"](http://pages.cs.wisc.edu/~chronis/files/efficiently_searching_sorted_arrays.pdf)
presented at ACM SIGMOD 2019.

## Introduction
We have created a jupyter notebook that runs the necesary experiments and produces the figures shown in the paper.
In the Reproducibility.ipynb notebook we have created a cell for each figure and we have created helper functions that
configure and run the exepriments.

The notebook can be run via a browser or without a graphical interface. Also we provide the configuration for two docker containers
that can run the notebook in these two modes.

The result of executing the notebook are the figures of the paper, and are saved in the figures directory. The results directory 
stores the raw results. In the format described in the README of the root folder of this repository.

We have tested our scripts in Ubuntu 18.04 and 16.04, if you want to use any other operating system we reccomend you use 
the docker images we provide.

## How to use - Run Locally
In the local mode, the docker container are not used. 
The first step is to install all the necessary packages and compile the search benchmark:
```bash
./install.sh
```
### Use the jupyter notebook interface
You can start the notebook by running:
```bash
./run_local_notebook.sh
```
A browser windon will open. By executing all the cells of the notebook you will be able to see the figures and also they will be saved
in pdf format in the figures folder.

### No gui
You produce all the figures by running:
```bash
./run_local_without_gui.sh
```
The figures will be saved in pdf format in the figures folder. Also a file named Reproducibility.ipynb.html will be produced
and will contained the executed notebook will all its outputs.

## How to use - Run in a Docker container
First [install docker](https://docs.docker.com/v17.09/engine/installation/#desktop) on your system.

### Use the jupyter notebook interface
You can start the notebook by running:
```bash
./run_docker.sh
```
When the container is built and started you will see on your terminal the address for accessing the notebook running on the container
from your host system's browser. By executing all the cells of the notebook you will be able to see the figures in your browser.
You can use the script:
```bash
./copy_results_from_docker.sh
```
to copy the generated figures from the docker container to your host machine. The will be saved in the figures_from_docker directory.

### No gui
You can start the container by running:
```bash
./run_docker_without_gui.sh
```
When the container is built and started you it will execute all the experiments. On your terminal you will see a message 
when the experiments are completed.
You can use the script:
```bash
./copy_results_from_docker.sh
```
to copy the generated figures from the docker container to your host machine. The will be saved in the figures_from_docker directory.

## Notes
Some of the experiments require a long time to run. The notebook we provide runs a scaled version of the exeperiments by default. 
You can run the full experiment by replacing this line in the notebook:
#### TODO



