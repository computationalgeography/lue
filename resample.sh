set -eu

ninja -j20 lue_framework_algorithm_resample_test lue_py_framework

ctest --verbose -R resample

# export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libtcmalloc_minimal.so.4
# export PYTHONPATH=lib/python3.12
#
# python -m unittest discover -v "$LUE/source/framework/python/test" resample_test.py
