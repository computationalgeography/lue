#!/usr/bin/env bash

python -m unittest discover \
    --start-directory $LUE/benchmark/test \
    --pattern "*_test.py"
