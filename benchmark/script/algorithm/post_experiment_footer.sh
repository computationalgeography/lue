for scenario in $scenarios;
do
    for algorithm_name in $algorithm_names;
    do
        pwd
        echo "$scenario - $algorithm_name"
        $LUE/benchmark/post_process_${experiment_name}_experiments.sh \
            lue_algorithm_${algorithm_name}_benchmark $scenario
    done

    for case_study_name in $case_study_names;
    do
        pwd
        echo "$scenario - $case_study_name"
        $LUE/benchmark/post_process_${experiment_name}_experiments.sh \
            lue_case_study_${case_study_name}_benchmark $scenario
    done
done
