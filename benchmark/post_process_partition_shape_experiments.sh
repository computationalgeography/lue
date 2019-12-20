set -e

dirname="$( cd "$(dirname "$BASH_SOURCE")" ; pwd -P )"

source $dirname/common.sh
parse_command_line $*
post_process_experiment "partition_shape" $benchmark_name $scenario
