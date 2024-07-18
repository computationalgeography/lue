#!/usr/bin/env bash
set -e


# For some reason, on Windows / msys / bash, the conda function is not available anymore in a
# subshell. To make it work, before calling it, do:
# export -f conda __conda_exe __conda_activate __conda_reactivate __add_sys_prefix_to_path


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
if [[ "$OSTYPE" == "linux-gnu"* ]];
then
    TCMALLOC=$(find $CONDA_PREFIX -name libtcmalloc_minimal.so.4)
    echo "Preloading $TCMALLOC"
    export LD_PRELOAD=$TCMALLOC
elif [[ "$OSTYPE" == "darwin"* ]];
then
    TCMALLOC=$(find $CONDA_PREFIX -name libtcmalloc_minimal.dylib)
    echo "Not preloading $TCMALLOC"
    # Uncomment and test, once needed
    # export DYLD_INSERT_LIBRARIES=$TCMALLOC
fi

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
