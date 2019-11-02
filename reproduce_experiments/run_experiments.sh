#!/usr/bin/env bash

echo "Create all experiment configurations"
python3 ./experiments_configurations/create_configuration.py

echo "Running all experiments"
python3 ./experiments_results/run_experiments.py
