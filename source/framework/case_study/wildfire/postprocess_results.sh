#!/usr/bin/env bash
set -e

bin_prefix=$LUE_OBJECTS/bin
source_prefix=$LUE/source/framework/case_study/wildfire

make -C $LUE_OBJECTS lue_translate

raster_layer_names="
    ignite_probability
    spot_ignite_probability
    burning
    fire
    fire_age
    nr_burnt_cells
    state
"


for raster_layer_name in $raster_layer_names; do
    $bin_prefix/lue_translate export \
        --meta $source_prefix/export_${raster_layer_name}.json \
        burn_wilderness.lue ${raster_layer_name}.tif
done


echo "aguila --timesteps [1,5000] $raster_layer_names"
