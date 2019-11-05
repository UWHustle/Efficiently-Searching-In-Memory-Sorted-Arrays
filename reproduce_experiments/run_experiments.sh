#!/usr/bin/env bash

echo $'\n\n########################  Create all experiment configurations  ########################'

python3 ./experiments_configurations/create_configuration.py

echo $'\n\n########################  Running all experiments  ########################'
python3 ./experiments_results/run_experiments.py
