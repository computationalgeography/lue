#!/usr/bin/env bash
set -e

for script_name in `ls *.py`; do
    python $script_name
done

for dataset_name in `ls *.lue`; do
    lue_validate $dataset_name
done

lue_to_svg.sh
