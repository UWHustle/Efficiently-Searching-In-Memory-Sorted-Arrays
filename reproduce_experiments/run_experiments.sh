#!/usr/bin/env bash

echo "\n\n\n\nCreate all experiment configurations ########################"
python3 ./experiments_configurations/create_configuration.py

echo "\n\n\n\nRunning all experiments ########################"
python3 ./experiments_results/run_experiments.py
