set -e

dirname="$( cd "$(dirname "$BASH_SOURCE")" ; pwd -P )"

source $dirname/common.sh
parse_command_line $*
perform_experiment "partition_shape" $benchmark_name $scenario
