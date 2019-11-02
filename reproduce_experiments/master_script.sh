#!/usr/bin/env bash

echo "Installing necessary packages and compiling the benchmarking framework"
# Install necessary packages needed by the benchmarking framework
# and to produce the paper figures. Compile the benchmarking framework
./install.sh


# Clean configurations, results, figures.
./clean.sh

# Creates experiment configfurations, run experiments.
./run_experiments.sh

# Creates the figures.
./create_figures.sh