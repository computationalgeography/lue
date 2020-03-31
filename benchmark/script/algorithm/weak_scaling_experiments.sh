set -e


experiment_name="weak_scaling"
algorithm_names="
    sqrt
    multiply
    focal_mean
    iterate_per_element
"
scenarios="
    thread_numa_node
    thread_cluster_node
    numa_node
    cluster_node
"


# algorithm_names="
# "
# scenarios="
# "


for scenario in $scenarios; do
    for algorithm_name in $algorithm_names; do
        echo "$scenario - $algorithm"
        $LUE/benchmark/${experiment_name}_experiments.sh \
            lue_algorithm_${algorithm_name}_benchmark $scenario
    done
done
