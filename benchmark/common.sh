names="copy iterate_per_element"
names="iterate_per_element"


function perform_experiment()
{
    experiment_kind=$1
    hostname=`hostname --short`

    for name in $names; do
        $LUE/benchmark/script/$experiment_kind/$hostname/$name.sh generate_script
        bash ./$hostname-$experiment_kind-$name.sh
    done
}
