for scenario in $scenarios; do
    for algorithm_name in $algorithm_names; do
        pwd
        echo "$scenario - $algorithm_name"
        $LUE/benchmark/${experiment_name}_experiments.sh \
            lue_algorithm_${algorithm_name}_benchmark $scenario
    done
done
