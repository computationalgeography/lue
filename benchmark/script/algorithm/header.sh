algorithm_names="
    accu_threshold
    accu_threshold3
    accu3
    focal_mean
    iterate_per_element
    multiply
    sqrt
    zonal_sum
"
case_study_names="
    routing
    wildfire
"
scenarios="
    thread_cluster_node
    thread_numa_node
    numa_node
    cluster_node
"


algorithm_names="
"
case_study_names="
"
scenarios="
"


output_prefix=${LUE_DATA:-.}

if [[ ! -d "${output_prefix}" ]]; then
    echo "Output prefix ${output_prefix} does not exist"
    exit 1
fi

cd $output_prefix
