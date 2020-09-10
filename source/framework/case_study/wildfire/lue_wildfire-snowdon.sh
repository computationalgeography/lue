#!/usr/bin/env bash
set -e


bin_prefix=$LUE_OBJECTS/bin
source_prefix=$LUE/source/framework/case_study/wildfire

make -C $LUE_OBJECTS lue_burn_wilderness

rm -fr $LUE_OBJECTS/OTF2_archive

time APEX_OTF2=1 APEX_SCREEN_OUTPUT=1 $bin_prefix/lue_burn_wilderness \
    --hpx:config=$source_prefix/hpx-snowdon.ini \
    --hpx:app-config=$source_prefix/lue_wildfire-snowdon.ini
