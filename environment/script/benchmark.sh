#!/usr/bin/env bash
set -e


# Make sure the required targets are up to date
make lue_dummy_benchmark lue_translate

# Benchmark parameters
count=10

# Scaling parameters
min_nr_threads=1
max_nr_threads=8
work_size=1

lue_dataset="benchmark.lue"
# epoch=`date --iso-8601=seconds`
epoch="2018-10-23T00:00:00+02:00"


function create_benchmark_dataset()
{
    rm -f $lue_dataset

    # TODO Add loop over work size(?)

    for ((nr_threads=min_nr_threads; nr_threads<=max_nr_threads; nr_threads++)); do
        basename="benchmark-$nr_threads"

        # Run benchmark
        bin/lue_dummy_benchmark \
            --nr_threads=$nr_threads \
            --work_size=$work_size \
            --count=$count \
            $basename.json

        # Add meta-information about the benchmarks to the LUE dataset (once)
        if [[ $nr_threads == $min_nr_threads ]]; then
            benchmark_to_lue_json.py --meta $basename.json $basename-meta-lue.json
            bin/lue_translate import --add $lue_dataset $basename-meta-lue.json
            rm $basename-meta-lue.json
        fi

        # Add current benchmark results to the LUE dataset
        benchmark_to_lue_json.py --epoch $epoch $basename.json $basename-lue.json
        bin/lue_translate import --add $lue_dataset $basename-lue.json
        rm $basename-lue.json
    done
}


function post_process_benchmarks()
{
    # Create a graph of the LUE dataset, for visual inspection
    bin/lue_translate export \
        --meta $LUE/document/lue_translate/dot_properties.json \
        $lue_dataset benchmark.dot
    dot -Tpdf -o benchmark-graph.pdf benchmark.dot
    rm benchmark.dot


    # Create document(s) with results of scaling experiment
    # TODO Pass in parameters for output file naming
    post_process_benchmarks.py $lue_dataset
}


create_benchmark_dataset
post_process_benchmarks
