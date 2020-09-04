#!/usr/bin/env bash
set -e
set -x


bin_prefix=$LUE_OBJECTS/bin
source_prefix=$LUE/source/framework/case_study/wildfire

# Build software
make -C $LUE_OBJECTS lue_burn_wilderness lue_translate


rm -fr $LUE_OBJECTS/OTF2_archive

time APEX_OTF2=1 APEX_SCREEN_OUTPUT=1 \
    $bin_prefix/lue_burn_wilderness \
        --hpx:config=$source_prefix/hpx-gransasso.ini \
        --hpx:app-config=$source_prefix/lue_wildfire-gransasso.ini


### # Postprocess results
### raster_layer_names="
###     ignite_probability
###     spot_ignite_probability
###     burning
###     fire
###     fire_age
###     state
### "
### 
### 
### for raster_layer_name in $raster_layer_names; do
###     $bin_prefix/lue_translate export \
###         --meta $source_prefix/export_${raster_layer_name}.json \
###         burn_wilderness.lue ${raster_layer_name}.tif
### done


### aguila --timesteps [1,5000] $raster_layer_names
