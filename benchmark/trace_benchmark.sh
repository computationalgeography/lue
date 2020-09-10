#!/usr/bin/env bash
set -e

benchmark_name="lue_algorithm_sqrt_benchmark"
benchmark_name="lue_algorithm_multiply_benchmark"
benchmark_name="lue_algorithm_iterate_per_element_benchmark"
benchmark_name="lue_algorithm_focal_mean_benchmark"
benchmark_name="lue_case_study_wildfire_benchmark"
trace_prefix=$LUE_OBJECTS/trace/$benchmark_name-$(date +%Y%m%d_%H%M)

nr_threads=4
count=1
nr_time_steps=10
max_tree_depth=$nr_time_steps
array_size=5000
partition_size=300  # 300  # 346  # 800
cluster_name=snowdon

# Make sure SLURM and APEX can create the output files
mkdir $trace_prefix

export APEX_OTF2=1
export APEX_OTF2_ARCHIVE_PATH=$trace_prefix


#   --hpx:ini="hpx.parcel.mpi.enable=1"



#    --hpx:bind=balanced --hpx:numa-sensitive

$LUE_OBJECTS/bin/$benchmark_name \
    --hpx:ini="hpx.agas.max_pending_refcnt_requests!=50" \
    --hpx:bind="thread:0-$((nr_threads-1))=core:0-$((nr_threads-1)).pu:0" \
    --hpx:ini="hpx.os_threads=$nr_threads" \
    --hpx:dump-config --hpx:print-bind \
    --hpx:ini="application.${benchmark_name}.benchmark.cluster_name!=$cluster_name" \
    --hpx:ini="application.${benchmark_name}.benchmark.count!=$count" \
    --hpx:ini="application.${benchmark_name}.benchmark.nr_workers!=$nr_threads" \
    --hpx:ini="application.${benchmark_name}.benchmark.output!=$trace_prefix/run.json" \
    --hpx:ini="application.${benchmark_name}.nr_time_steps!=$nr_time_steps" \
    --hpx:ini="application.${benchmark_name}.array_shape!=[$array_size, $array_size]" \
    --hpx:ini="application.${benchmark_name}.partition_shape!=[$partition_size, $partition_size]" \
    --hpx:ini="application.${benchmark_name}.max_tree_depth!=$max_tree_depth"







### # Submit job to SLURM scheduler
### sbatch --job-name $benchmark_name << END_OF_SLURM_SCRIPT
### #!/usr/bin/env bash
### #SBATCH --nodes=1
### #SBATCH --ntasks=1
### #SBATCH --cpus-per-task=96
### #SBATCH --output=$trace_prefix/${benchmark_name}.out
### #SBATCH --partition=allq
### 
### 
### set -e
### 
### module purge
### module load opt/all
### module load userspace/all
### module load gcc/7.2.0
### module load boost/gcc72/1.65.1
### module load mpich/gcc72/mlnx/3.2.1
### module load libraries/papi/5.7.0
### 
### #       --hpx:ini="hpx.thread_queue.min_tasks_to_steal_staged!=2"
### #       --hpx:queuing=shared-priority
### #       --hpx:queuing=abp-priority-fifo \
### srun --kill-on-bad-exit --mpi=pmi2 --qos=priority \
###     $LUE_OBJECTS/bin/$benchmark_name \
###         --hpx:bind=balanced \
###         --hpx:numa-sensitive \
###         --hpx:ini="hpx.parcel.mpi.enable=1" \
###         --hpx:ini="hpx.os_threads=$nr_threads" \
###         --hpx:dump-config --hpx:print-bind \
###         --hpx:ini="application.${benchmark_name}.benchmark.cluster_name!=eejit" \
###         --hpx:ini="application.${benchmark_name}.benchmark.count!=$count" \
###         --hpx:ini="application.${benchmark_name}.benchmark.output!=$trace_prefix/run.json" \
###         --hpx:ini="application.${benchmark_name}.nr_time_steps!=$nr_time_steps" \
###         --hpx:ini="application.${benchmark_name}.max_tree_depth!=$max_tree_depth" \
###         --hpx:ini="application.${benchmark_name}.array_shape!=[15000, 15000]" \
###         --hpx:ini="application.${benchmark_name}.partition_shape!=[800, 800]"
### END_OF_SLURM_SCRIPT
