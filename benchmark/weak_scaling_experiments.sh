set -e

dirname="$( cd "$(dirname "$BASH_SOURCE")" ; pwd -P )"

source $dirname/common.sh
parse_command_line $*
perform_experiment "weak_scaling" $benchmark_name $scenario
