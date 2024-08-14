#!/usr/bin/env bash

####SBATCH -N 2
####SBATCH -n 2
#SBATCH --output meh.txt

srun bin/lue_pollute_air \
    --hpx:app-config=$LUE/source/lue_framework/case_study/airpolution/benchmark/lue_pollute_air.ini
