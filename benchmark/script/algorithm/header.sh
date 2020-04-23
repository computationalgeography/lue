algorithm_names="
    sqrt
    multiply
    focal_mean
    iterate_per_element
"
scenarios="
    thread_numa_node
    numa_node
    cluster_node
"
#   thread_cluster_node

output_prefix=${LUE_DATA:-.}

if [[ ! -d "${output_prefix}" ]]; then
    echo "Output prefix ${output_prefix} does not exist"
    exit 1
fi

cd $output_prefix
