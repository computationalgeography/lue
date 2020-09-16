#!/usr/bin/env bash
set -e
set -x

benchmark_name="lue_algorithm_sqrt_benchmark"
benchmark_name="lue_algorithm_multiply_benchmark"
benchmark_name="lue_algorithm_iterate_per_element_benchmark"
benchmark_name="lue_algorithm_focal_mean_benchmark"
output_file_basename=$benchmark_name-$(date +%Y%m%d_%H%M)

nr_threads=6
count=1
nr_time_steps=10
max_tree_depth=$nr_time_steps
array_size=10000
partition_size=700  # 300  # 346  # 800
cluster_name=`hostname`

#   --hpx:ini="hpx.parcel.mpi.enable=1"

#    --hpx:bind=balanced --hpx:numa-sensitive


function submit()
{
    command=$1

    # Submit job to SLURM scheduler
    # Ask for 8 numa nodes in a node
    sbatch --job-name $benchmark_name << END_OF_SLURM_SCRIPT
#!/usr/bin/env bash
#SBATCH --nodes=2
#SBATCH --ntasks=16
#SBATCH --cpus-per-task=12
#SBATCH --cores-per-socket=6
#SBATCH --output=${output_file_basename}.out
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

srun --kill-on-bad-exit --qos=swdev --ntasks 16 $command
END_OF_SLURM_SCRIPT
}


command="$LUE_OBJECTS/bin/$benchmark_name --hpx:bind=\"thread:0-$((nr_threads-1))=core:0-$((nr_threads-1)).pu:0\" --hpx:ini=\"hpx.os_threads=$nr_threads\" --hpx:ini=\"hpx.parcel.mpi.enable=1\" --hpx:dump-config --hpx:print-bind --hpx:ini="hpx.agas.max_pending_refcnt_requests!=50" --hpx:ini=\"application.${benchmark_name}.benchmark.cluster_name!=$cluster_name\" --hpx:ini=\"application.${benchmark_name}.benchmark.count!=$count\" --hpx:ini=\"application.${benchmark_name}.benchmark.nr_workers!=16\" --hpx:ini=\"application.${benchmark_name}.benchmark.output!=${output_file_basename}.json \" --hpx:ini=\"application.${benchmark_name}.nr_time_steps!=$nr_time_steps\" --hpx:ini=\"application.${benchmark_name}.array_shape!=[$array_size, $array_size]\" --hpx:ini=\"application.${benchmark_name}.partition_shape!=[$partition_size, $partition_size]\" --hpx:ini=\"application.${benchmark_name}.max_tree_depth!=$max_tree_depth\""

submit "$command"
