#!/usr/bin/env bash

echo "\n\n\n\nInstalling necessary packages and compiling the benchmarking framework  ########################"
# Install necessary packages needed by the benchmarking framework
# and to produce the paper figures. Compile the benchmarking framework
./install.sh

# Creates experiment configfurations, run experiments.
./run_experiments.sh

# Creates the figures.
./create_figures.sh