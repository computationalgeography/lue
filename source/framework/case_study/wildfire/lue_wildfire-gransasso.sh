#!/usr/bin/env bash
set -e
set -x


bin_prefix=$LUE_OBJECTS/bin
source_prefix=$LUE/source/framework/case_study/wildfire

# Build software
make -C $LUE_OBJECTS lue_burn_wilderness lue_translate


# rm -fr $LUE_OBJECTS/OTF2_archive
# time APEX_OTF2=1 APEX_SCREEN_OUTPUT=1

$bin_prefix/lue_burn_wilderness \
    --hpx:config=$source_prefix/hpx-gransasso.ini \
    --hpx:app-config=$source_prefix/lue_wildfire-gransasso.ini
