#!/usr/bin/env python

from os import popen
from subprocess import call
from pandas import read_csv
import sys
import subprocess
from subprocess import DEVNULL 

call(['make', 'searchbench'])
run_param = ['Run','DatasetSize','Distribution','Parameter','#threads','SearchAlgorithm','RecordSizeBytes']

with open("outfile", "w") as log_file:
    subprocess.run(["./searchbench","experiments.tsv"], stdout=log_file, stderr=DEVNULL)

df = read_csv("outfile", sep='\t')

print("\nTime to search one record:")
print(df.groupby(run_param)['TimeNS'].mean())

call(['rm', 'outfile'])

#More detailed statistics
#print(df.groupby(run_param)['TimeNS'].describe(percentiles=[.25,0.75]))
