#!/usr/bin/env bash
set -e


# Assume Conda is installed. Do the minimal amount of work to verify the latest LUE package
# works fine. Nothing else.

# Create and activate new Conda environment
conda create --yes --name test_lue python=3.10

# https://github.com/conda/conda/issues/7980
CONDA_BASE=$(conda info --base)
source $CONDA_BASE/etc/profile.d/conda.sh

conda activate test_lue

# Install packages
conda install --yes --channel conda-forge lue

# Perform some hello world test
TCMALLOC=$(find /usr/lib -name libtcmalloc_minimal.so.4)
export LD_PRELOAD=$TCMALLOC

python <<EOF
import lue
import lue.framework as lfr
import numpy as np


@lfr.runtime_scope
def hello_lue():

    array = lfr.uniform(lfr.create_array((10000, 10000), (1000, 1000), np.dtype(np.float32), 5), 0, 1000)
    print("Hello LUE version {}, based on Git commit with hash {}".format(lue.__version__, lue.git_short_sha1))
    print("Hello partitioned array: {}".format(array))


hello_lue()
EOF


# Deactivate Conda package
conda deactivate

# Remove Conda package
conda env remove --name test_lue
