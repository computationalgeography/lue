function parse_command_line()
{
    benchmark_name=$1
    scenario=$2
}


function perform_experiment()
{
    experiment_kind=$1
    benchmark_name=$2
    scenario_name=$3

    hostname=`hostname --short`

    if [[ $hostname == "login01" ]]; then
        hostname="eejit"
    fi

    $LUE/benchmark/script/$experiment_kind/$benchmark_name.sh \
        generate_script $scenario_name
    bash ./$hostname-$experiment_kind-$benchmark_name-$scenario_name.sh
}


function post_process_experiment()
{
    experiment_kind=$1
    benchmark_name=$2
    scenario_name=$3

    $LUE/benchmark/script/$experiment_kind/$benchmark_name.sh \
        post_process $scenario_name
}
