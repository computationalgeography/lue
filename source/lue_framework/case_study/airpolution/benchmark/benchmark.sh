#!/usr/bin/env bash
set -e
# set -x


# Split the creation of the benchmark results from the postprocessing
# - Create
#     - Run experiment on a single node
#         - Iterate over nr_threads: 1, 2, 4, 8, 16
#     - Run experiment on a number of nodes in a cluster
#         - Iterate over nr_nodes: 1, 2, 4, 8, 16
# - All this results in raw outputs per run. Postprocess this as a
#     seperate step.
#     - Wait until results are available
#     - Create graphs


export PYTHONPATH="$LUE_OBJECTS/lib:$PYTHONPATH"

program_name="lue_airpolution_benchmark"
program_pathname="$LUE_OBJECTS/bin/$program_name"


# partition_name="defq"
partition_name="allq"
max_duration=10

# terminate called after throwing an instance of 'hpx::detail::exception_with_info<hpx::exception>'
#   what():  <unknown>: HPX(network_error)
spawn_interval=1  # This has to be larger than zero: why?


# TODO Also scale over localities

# srun -p <partition> -N <number-of-nodes> -n <number-of-instances>
# -N is the total number of nodes to use
# -n is the total number of HPX localities to start









# Program parameters
nr_time_steps=100
nr_rows=60
nr_cols=40
nr_rows_grain=5
nr_cols_grain=5

# Benchmark parameters
count=5
let "work_size = $nr_rows * $nr_cols"


lue_dataset="$program_name.lue"


epoch=`date --iso-8601=seconds`
epoch="2018-12-06T00:00:00+02:00"


# Scale over nodes and scale over threads
min_nr_nodes=1  # 1
max_nr_nodes=8  # 8
min_nr_threads=48
max_nr_threads=48

# --hpx:threads=$nr_threads


function build_software()
{
    make -C $LUE_OBJECTS ${program_name}_exe lue_translate
}


function create_benchmark_dataset()
{
    # Scale over nodes
    for ((nr_nodes=min_nr_nodes; nr_nodes<=max_nr_nodes; nr_nodes*=2)); do

        # Scale over threads
        for ((nr_threads=min_nr_threads; nr_threads<=max_nr_threads; nr_threads*=2)); do

            # Names unique for current benchmark
            basename="$program_name-$nr_nodes-$nr_threads"
            batch_filename="$basename.sh"
            output_filename="$basename.txt"
            benchmark_result_filename="$basename.json"

            # Create batch-file
            cat > $batch_filename <<END
#!/usr/bin/env bash

#SBATCH -N $nr_nodes
#SBATCH -n $nr_nodes
#SBATCH -c $nr_threads
#SBATCH --output $output_filename
#SBATCH --partition $partition_name
#SBATCH --time $max_duration


# module load userspace/all
# module load mpich/gcc72/mlnx/3.2.1


srun $program_pathname \
    --hpx:ini="hpx.parcel.mpi.enable=0" \
    --hpx:ini="application.${program_name}.benchmark.count!=$count" \
    --hpx:ini="application.${program_name}.benchmark.work_size!=$work_size" \
    --hpx:ini="application.${program_name}.benchmark.output!=$benchmark_result_filename" \
    --hpx:ini="application.${program_name}.nr_time_steps!=$nr_time_steps" \
    --hpx:ini="application.${program_name}.nr_rows!=$nr_rows" \
    --hpx:ini="application.${program_name}.nr_cols!=$nr_cols" \
    --hpx:ini="application.${program_name}.nr_rows_grain!=$nr_rows_grain" \
    --hpx:ini="application.${program_name}.nr_cols_grain!=$nr_cols_grain"
echo "DONE!"
END

            # Run benchmark (asynchronous!), storing results in
            # $benchmark_result_filename
            sbatch $batch_filename

            # Sleep a bit. Ideally, the spawned benchmark is done.
            echo "Sleep for ${spawn_interval}m..."
            sleep ${spawn_interval}m

        done
    done
}


function post_process_benchmarks()
{
    lue_translate="$LUE_OBJECTS/bin/lue_translate"
    rm -f $lue_dataset

    for ((nr_nodes=min_nr_nodes; nr_nodes<=max_nr_nodes; nr_nodes*=2)); do
        for ((nr_threads=min_nr_threads; nr_threads<=max_nr_threads; nr_threads*=2)); do

            basename="$program_name-$nr_nodes-$nr_threads"
            benchmark_result_filename="$basename.json"

            # Add meta-information about the benchmarks to the LUE dataset
            # (once)
            if [[ \
                    $nr_nodes == $min_nr_nodes && \
                    $nr_threads == $min_nr_threads ]]; then

                benchmark_to_lue_json.py \
                    --meta $benchmark_result_filename \
                    $basename-meta-lue.json

                $lue_translate import --add $lue_dataset $basename-meta-lue.json

                rm $basename-meta-lue.json

            fi

            # Add current benchmark results to the LUE dataset
            benchmark_to_lue_json.py \
                --epoch $epoch \
                $benchmark_result_filename \
                $basename-lue.json

            $lue_translate import --add $lue_dataset $basename-lue.json

            rm $basename-lue.json

        done
    done


    # Create a graph of the LUE dataset, for visual inspection
    $lue_translate export \
        --meta $LUE/document/lue_translate/dot_properties.json \
        $lue_dataset benchmark.dot
    dot -Tpdf -o benchmark-graph.pdf benchmark.dot
    rm benchmark.dot

    # Create document(s) with results of scaling experiment
    # TODO Pass in parameters for output file naming
    post_process_benchmarks.py $lue_dataset
}


build_software

# Pick one of these:
# create_benchmark_dataset
post_process_benchmarks
