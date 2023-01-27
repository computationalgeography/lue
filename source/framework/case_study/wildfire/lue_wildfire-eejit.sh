#!/usr/bin/env bash
set -e


bin_prefix=$LUE_OBJECTS/bin
source_prefix=$LUE/source/framework/case_study/wildfire

# Build software
make -C $LUE_OBJECTS lue_burn_wilderness lue_translate


# Run model
sbatch --job-name lue_burn_wilderness  << END_OF_SLURM_SCRIPT
#!/usr/bin/env bash
#SBATCH --nodes=2
#SBATCH --ntasks=16
#SBATCH --cpus-per-task=12
#SBATCH --output=wildfire.out
#SBATCH --cores-per-socket=6
#SBATCH --partition=allq


set -e

module purge
module load opt/all
module load userspace/all
module load libraries/zstd/1.3.7
module load gcc/10.2.0
module load openmpi/gcc-10.2.0/4.0.4
module load perftools/2.7
module load libraries/papi/5.7.0

srun \
    --ntasks 16 --kill-on-bad-exit \
    $bin_prefix/lue_burn_wilderness \
        --hpx:config=$source_prefix/hpx-eejit.ini \
        --hpx:app-config=$source_prefix/lue_wildfire-eejit.ini
END_OF_SLURM_SCRIPT


# # Postprocess results
# raster_layer_names="
#     array_partition_id
#     locality_id
#     ignite_probability
#     spot_ignite_probability
#     burning
#     fire
#     fire_age
#     state
# "
# raster_layer_names="
#     array_partition_id
#     locality_id
# "
#
#
# for raster_layer_name in $raster_layer_names; do
#     $bin_prefix/lue_translate export \
#         --meta $source_prefix/export_${raster_layer_name}.json \
#         burn_wilderness.lue ${raster_layer_name}.tif
# done
