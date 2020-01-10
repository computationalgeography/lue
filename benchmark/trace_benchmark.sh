#!/usr/bin/env bash
set -e

benchmark_name="lue_framework_algorithm_focal_mean_benchmark"
trace_prefix=$LUE_OBJECTS/trace/$benchmark_name

nr_threads=20
count=1
nr_time_steps=50
max_tree_depth=5

# Make sure SLURM and APEX can create the output files
mkdir $trace_prefix

export APEX_OTF2=1
export APEX_OTF2_ARCHIVE_PATH=$trace_prefix

# Submit job to SLURM scheduler
sbatch --job-name $benchmark_name << END_OF_SLURM_SCRIPT
#!/usr/bin/env bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=96
#SBATCH --output=$trace_prefix/${benchmark_name}.out
#SBATCH --partition=allq


set -e

module purge
module load opt/all
module load userspace/all
module load gcc/7.2.0
module load boost/gcc72/1.65.1
module load mpich/gcc72/mlnx/3.2.1
module load libraries/papi/5.7.0

srun --kill-on-bad-exit --mpi=pmi2 --qos=priority \
    $LUE_OBJECTS/bin/$benchmark_name \
        --hpx:ini="hpx.parcel.mpi.enable=1" \
        --hpx:ini="hpx.os_threads=$nr_threads" \
        --hpx:dump-config --hpx:print-bind \
        --hpx:ini="application.${benchmark_name}.benchmark.cluster_name!=eejit" \
        --hpx:ini="application.${benchmark_name}.benchmark.count!=$count" \
        --hpx:ini="application.${benchmark_name}.benchmark.output!=$trace_prefix/run.json" \
        --hpx:ini="application.${benchmark_name}.nr_time_steps!=$nr_time_steps" \
        --hpx:ini="application.${benchmark_name}.max_tree_depth!=$max_tree_depth" \
        --hpx:ini="application.${benchmark_name}.array_shape!=[5000, 5000]" \
        --hpx:ini="application.${benchmark_name}.partition_shape!=[566, 566]"
END_OF_SLURM_SCRIPT
