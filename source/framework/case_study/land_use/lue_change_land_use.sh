#!/usr/bin/env bash
set -e


install_prefix=$LUE_OBJECTS
bin_prefix=$install_prefix/bin
source_prefix=$LUE/source/framework/case_study/land_use


function build_required_software()
{
    make -C $LUE_OBJECTS lue_change_land_use lue_translate
}


function execute_model()
{
    nr_threads=24

    $bin_prefix/lue_change_land_use \
        --hpx:ini="hpx.os_threads=$nr_threads" \
        --hpx:app-config="$source_prefix/lue_change_land_use.ini"
}


function post_process_results()
{
    $bin_prefix/lue_translate export \
        --meta $source_prefix/export_locality_id.json \
        change_land_use.lue locality_id.tif
    $bin_prefix/lue_translate export \
        --meta $source_prefix/export_elevation.json \
        change_land_use.lue elevation.tif
    $bin_prefix/lue_translate export \
        --meta $source_prefix/export_land_use.json \
        change_land_use.lue land_use.tif
}


build_required_software
execute_model
post_process_results
