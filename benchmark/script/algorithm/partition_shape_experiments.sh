set -e


dirname="$( cd "$(dirname "$BASH_SOURCE")" ; pwd -P )"
source $dirname/header.sh
experiment_name="partition_shape"


algorithm_names="
    sqrt
"
scenarios="
    thread_cluster_node
"


source $dirname/experiment_footer.sh
