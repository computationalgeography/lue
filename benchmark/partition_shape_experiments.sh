set -e

dirname="$( cd "$(dirname "$BASH_SOURCE")" ; pwd -P )"

source $dirname/common.sh
perform_experiment "partition_shape"
