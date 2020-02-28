#!/usr/bin/env bash
set -e

bin_prefix=$LUE_OBJECTS/bin
source_prefix=$LUE/source/framework/case_study/land_use

make -C $LUE_OBJECTS lue_change_land_use lue_translate

# # Run model
# $bin_prefix/lue_change_land_use \
#     --hpx:app-config=$source_prefix/lue_change_land_use.ini

# Postprocess results
$bin_prefix/lue_translate export \
    --meta $source_prefix/export_elevation.json \
    change_land_use.lue elevation.tif
$bin_prefix/lue_translate export \
    --meta $source_prefix/export_land_use.json \
    change_land_use.lue land_use.tif
