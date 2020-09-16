#!/usr/bin/env bash
set -e

benchmark_name="lue_algorithm_sqrt_benchmark"
benchmark_name="lue_algorithm_multiply_benchmark"
benchmark_name="lue_algorithm_iterate_per_element_benchmark"
benchmark_name="lue_algorithm_focal_mean_benchmark"
benchmark_name="lue_case_study_wildfire_benchmark"
benchmark_name="lue_algorithm_multiply_benchmark"
# trace_prefix=$LUE_OBJECTS/trace/$benchmark_name-$(date +%Y%m%d_%H%M)
trace_prefix=trace/$benchmark_name-$(date +%Y%m%d_%H%M)
trace_prefix=/scratch/depfg/jong0137/development/object/RelWithDeInfo/lue/trace/$benchmark_name-$(date +%Y%m%d_%H%M)

mkdir $trace_prefix

export APEX_OTF2=1
export APEX_OTF2_ARCHIVE_PATH=$trace_prefix
export APEX_TASKGRAPH_OUTPUT=1

count=1
nr_time_steps=10
max_tree_depth=$nr_time_steps

# cluster_name=snowdon
# nr_threads=4
# array_size=35000
# partition_size=1000  # 300  # 346  # 800

# $LUE_OBJECTS/bin/$benchmark_name \
#     --hpx:ini="hpx.agas.max_pending_refcnt_requests!=50" \
#     --hpx:bind="thread:0-$((nr_threads-1))=core:0-$((nr_threads-1)).pu:0" \
#     --hpx:ini="hpx.os_threads=$nr_threads" \
#     --hpx:dump-config --hpx:print-bind \
#     --hpx:ini="application.${benchmark_name}.benchmark.cluster_name!=$cluster_name" \
#     --hpx:ini="application.${benchmark_name}.benchmark.count!=$count" \
#     --hpx:ini="application.${benchmark_name}.benchmark.nr_workers!=$nr_threads" \
#     --hpx:ini="application.${benchmark_name}.benchmark.output!=$trace_prefix/run.json" \
#     --hpx:ini="application.${benchmark_name}.nr_time_steps!=$nr_time_steps" \
#     --hpx:ini="application.${benchmark_name}.array_shape!=[$array_size, $array_size]" \
#     --hpx:ini="application.${benchmark_name}.partition_shape!=[$partition_size, $partition_size]" \
#     --hpx:ini="application.${benchmark_name}.max_tree_depth!=$max_tree_depth"



cluster_name=eejit
nr_threads=6
array_size=1000  # 35000
partition_size=400  # 1000

nr_workers=1  # 2
total_nr_tasks=2  # 16

sbatch --job-name $benchmark_name << END_OF_SLURM_SCRIPT
#!/usr/bin/env bash
#SBATCH --nodes=$nr_workers
#SBATCH --ntasks=$total_nr_tasks
#SBATCH --cpus-per-task=12
#SBATCH --cores-per-socket=6
#SBATCH --output=$trace_prefix/${benchmark_name}.out
#SBATCH --partition=allq
#SBATCH --qos=swdev


set -e

module purge
module load opt/all
module load userspace/all
module load libraries/zstd/1.3.7
module load gcc/10.2.0
module load openmpi/gcc-10.2.0/4.0.4
module load libraries/papi/5.7.0
module load perftools/2.7

srun --nodes 1 --ntasks 1 mkdir -p $trace_prefix
srun --kill-on-bad-exit --qos=swdev --ntasks $total_nr_tasks \
    $LUE_OBJECTS/bin/$benchmark_name \
        --hpx:ini="hpx.agas.max_pending_refcnt_requests!=50" \
        --hpx:bind="thread:0-$((nr_threads-1))=core:0-$((nr_threads-1)).pu:0" \
        --hpx:ini="hpx.os_threads=$nr_threads" \
        --hpx:dump-config --hpx:print-bind \
        --hpx:ini="application.${benchmark_name}.benchmark.cluster_name!=$cluster_name" \
        --hpx:ini="application.${benchmark_name}.benchmark.count!=$count" \
        --hpx:ini="application.${benchmark_name}.benchmark.nr_workers!=$nr_workers" \
        --hpx:ini="application.${benchmark_name}.benchmark.output!=$trace_prefix/run.json" \
        --hpx:ini="application.${benchmark_name}.nr_time_steps!=$nr_time_steps" \
        --hpx:ini="application.${benchmark_name}.array_shape!=[$array_size, $array_size]" \
        --hpx:ini="application.${benchmark_name}.partition_shape!=[$partition_size, $partition_size]" \
        --hpx:ini="application.${benchmark_name}.max_tree_depth!=$max_tree_depth"
END_OF_SLURM_SCRIPT
