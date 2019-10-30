# This file containes a helper function gives the appropriate configuration
# file to the benchmarking framework and saves the results.
# This file also run the experiments that produce the figures of our paper.

from os import path, remove
import sys
import subprocess
from subprocess import call, check_output, DEVNULL


def run(tsvname):
    print("Running experiment : " + tsvname)
    if not path.exists("../searchbench"):
        print(
            "Please make sure searchbench is compiled. You can compile this by running make on the parent directory.")
        sys.exit()

    resultFile = "experiments_results/" + tsvname + ".results"
    if path.exists(resultFile):
        print(
            "This tsv has been already executed and the results have been saved.")
        print(
                    "If you want to rerun the experiments please delete the file: " + tsvname + ".results")
    else:
        with open(resultFile, "w") as log_file:
            subprocess.run(["python3", "./getTimes.py",
                            "./reproduce_experiments/experiments_configurations/" + tsvname],
                           stdout=log_file, stderr=DEVNULL, cwd="../")


##########################

run("fig2.tsv")
run("fig5.tsv")
run("fig6.tsv")
run("fig7.tsv")
run("fig8_8.tsv")
run("fig8_32.tsv")
run("fig8_128.tsv")
run("fig9.tsv")
run("fig10.tsv")
run("fig11.tsv")
