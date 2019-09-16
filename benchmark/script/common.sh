cluster_name=`hostname --short`

if [[ $cluster_name == "login01" ]]; then
    cluster_name="eejit"
fi

configuration=$LUE/benchmark/configuration
cluster=$configuration/$cluster_name
benchmark=$cluster/$benchmark_kind
benchmark_exe=$LUE_OBJECTS/bin/lue_framework_algorithm_${benchmark_name}_benchmark

cluster_settings=$cluster/cluster.json
benchmark_settings=$benchmark/benchmark.json
experiment_settings=$benchmark/$benchmark_name/experiment.json


function print_usage()
{
    echo -e "\
usage: $0 [-h] (generate_script | post_process)

-h               Show (this) usage information.

generate_script  Generate job scheduler script
post_process     Post process experiment results"
}


function parse_commandline()
{
    command=$1
}


function generate_script()
{
    generate_job_scheduler_script.py $benchmark_kind \
        $cluster_settings $benchmark_settings $experiment_settings \
        $cluster_name-$benchmark_kind-$benchmark_name.sh \
        $benchmark_exe
}


function post_process()
{
    post_process_benchmark_results.py $benchmark_kind \
        $cluster_settings $benchmark_settings $experiment_settings \
        $benchmark_exe
}


parse_commandline $*

if [ "$command" = "generate_script" ]; then
    generate_script
elif [ "$command" = "post_process" ]; then
    post_process
else
    echo "Unknown command: $command" 1>&2
    exit 1
fi
