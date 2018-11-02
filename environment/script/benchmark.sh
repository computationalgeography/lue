#!/usr/bin/env bash
set -e


make lue_dummy_benchmark lue_translate

count=10
min_nr_threads=1
max_nr_threads=8
# epoch=`date --iso-8601=seconds`
epoch="2018-10-23T00:00:00+02:00"
work_size=1

rm -f benchmark.lue

for ((nr_threads=min_nr_threads; nr_threads<=max_nr_threads; nr_threads++)); do
    basename="benchmark-$nr_threads"

    # bin/lue_dummy_benchmark \
    #     --nr_threads=$nr_threads \
    #     --work_size=$work_size \
    #     --count=$count \
    #     $basename.json

    if [[ $nr_threads == $min_nr_threads ]]; then
        # Dump meta-information about the benchmarks (once)
        benchmark_to_lue_json.py --meta $basename.json $basename-meta-lue.json
        bin/lue_translate import --add benchmark.lue $basename-meta-lue.json
        # rm $basename-meta-lue.json
    fi

    benchmark_to_lue_json.py --epoch $epoch $basename.json $basename-lue.json
    bin/lue_translate import --add benchmark.lue $basename-lue.json
    # rm $basename-lue.json
done

bin/lue_translate export \
    --meta $LUE/document/lue_translate/dot_properties.json \
    benchmark.lue benchmark.dot
dot -Tpdf -o benchmark-graph.pdf benchmark.dot
post_process_benchmarks.py benchmark.lue
