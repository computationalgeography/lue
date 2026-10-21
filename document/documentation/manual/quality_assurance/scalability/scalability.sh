#!/usr/bin/env bash
set -euo pipefail

platform_config_prefix="$HOME/development/project/github/computational_geography/lue_qa/configuration"
experiment_config_prefix="$HOME/development/project/github/computational_geography/lue_qa/experiment/configuration"
result_prefix="/tmp/scalability"

command_name="game_of_life.py"
nr_generations=50
command_arguments="$nr_generations"
platform_name="orkney"
worker_name="core_numa_node"

experiment_names="
    partition_shape
    strong_scalability
    weak_scalability
"

for experiment_name in $experiment_names;
do
    echo $experiment_name

    lue_scalability.py \
        $experiment_name script $command_name "$command_arguments" \
        $platform_name $worker_name \
        $platform_config_prefix $experiment_config_prefix $result_prefix

    script="$result_prefix/$platform_name/$command_name/$worker_name/$experiment_name.sh"
    bash $script

    lue_scalability.py \
        $experiment_name import $command_name "$command_arguments" \
        $platform_name $worker_name \
        $platform_config_prefix $experiment_config_prefix $result_prefix

    lue_scalability.py \
        $experiment_name postprocess $command_name "$command_arguments" \
        $platform_name $worker_name \
        $platform_config_prefix $experiment_config_prefix $result_prefix
done
