#!/usr/bin/env bash
set -e

basename="focal_mean"
bin_prefix=$LUE_OBJECTS/bin
# source_prefix=$LUE/source/framework/algorithm/benchmark
tmp_prefix=${TMPDIR:-/tmp}


raster_layer_names="
    array_partition_id
    locality_id
    nr_elements_per_locality
"


for raster_layer_name in $raster_layer_names; do

    tmpfile=$(mktemp $tmp_prefix/$raster_layer_name.XXXXXX)

    cat << EOF > $tmpfile
{
    "$basename": {
        "phenomena": [
            {
                "name": "area",
                "property_sets": [
                    {
                        "name": "constant",
                        "properties": [
                            {
                                "name": "$raster_layer_name"
                            }
                        ]
                    }
                ]
            }
        ]
    }
}
EOF

    $bin_prefix/lue_translate export \
        --meta $tmpfile $basename.lue ${raster_layer_name}.tif
    rm $tmpfile
done

# $LUE_OBJECTS/bin/lue_translate export --meta $LUE/source/framework/algorithm/benchmark/export_array_partition_id.json $basename.lue array_partition_id.tif
