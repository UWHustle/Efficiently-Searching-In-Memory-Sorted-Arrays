#!/usr/bin/env bash


echo "Installing necessary packages and compiling the benchmarking framework"
# Install necessary packages needed by the benchmarking framework
# and to produce the paper figures. Compile the benchmarking framework
./install.sh

# Creates experiment configfurations, run experiments and create the figures.
./create_figures.sh