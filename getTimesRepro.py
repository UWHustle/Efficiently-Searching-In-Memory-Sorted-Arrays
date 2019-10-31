#!/usr/bin/env python

from os import path
from os import popen
from subprocess import call
from pandas import read_csv, set_option
import sys
import subprocess
from subprocess import DEVNULL

tsv="experiments.tsv"
if len(sys.argv) > 1:
        tsv=sys.argv[1]

if not path.exists("./searchbench"):
    print("Please compile the searchbench before using this script.\n")
    exit()

run_param = ['DatasetSize','Distribution','Parameter','#threads','SearchAlgorithm','RecordSizeBytes']

with open("outfile", "w") as log_file:
    subprocess.run(["./searchbench", tsv], stdout=log_file, stderr=DEVNULL)

df = read_csv("outfile", sep='\t')

print("\nTime to search one record:")
set_option('display.max_rows', len(df))
print(df.groupby(run_param)['TimeNS'].mean())

call(['rm', 'outfile'])

#More detailed statistics
#print(df.groupby(run_param)['TimeNS'].describe(percentiles=[.25,0.75]))
