algorithm_names="
    sqrt
    multiply
    focal_mean
    iterate_per_element
    zonal_sum
"
case_study_names="
    wildfire
"
scenarios="
    thread_numa_node
    numa_node
    cluster_node
"
#   thread_cluster_node

algorithm_names=""
case_study_names=""

output_prefix=${LUE_DATA:-.}

if [[ ! -d "${output_prefix}" ]]; then
    echo "Output prefix ${output_prefix} does not exist"
    exit 1
fi

cd $output_prefix
