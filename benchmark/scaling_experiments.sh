set -e
# set -x


benchmark_prefix=$LUE/benchmark
hostname="gransasso"
data_prefix=$LUE_OBJECTS/$hostname


function perform_experiment()
{
    benchmark="$1"
    scenario="$2"
    experiment="$3"

    rm -fr $data_prefix/lue_${benchmark}_benchmark/$experiment-$scenario
    $benchmark_prefix/${experiment}_experiments.sh $benchmark $scenario
    $benchmark_prefix/post_process_${experiment}_experiments.sh $benchmark $scenario
}


benchmark_name="case_study_heat_ring"
benchmark_name="framework_algorithm_sqrt"
benchmark_name="framework_algorithm_focal_mean"


# perform_experiment $benchmark_name "thread" "partition_shape"
# perform_experiment $benchmark_name "thread" "strong_scaling"
# perform_experiment $benchmark_name "thread" "weak_scaling"
