names="copy iterate_per_element"
names="iterate_per_element"


function perform_experiment()
{
    experiment_kind=$1

    hostname=`hostname --short`

    if [[ $hostname == "login01" ]]; then
        hostname="eejit"
    fi

    for name in $names; do
        $LUE/benchmark/script/$experiment_kind/$hostname/$name.sh generate_script
        bash ./$hostname-$experiment_kind-$name.sh
    done
}


function post_process_experiment()
{
    experiment_kind=$1

    hostname=`hostname --short`

    if [[ $hostname == "login01" ]]; then
        hostname="eejit"
    fi

    for name in $names; do
        $LUE/benchmark/script/$experiment_kind/$hostname/$name.sh post_process
    done
}
