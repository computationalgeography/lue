dirname="$( cd "$(dirname "$BASH_SOURCE")" ; pwd -P )"
script_name=`basename "$0"`
benchmark_name="${script_name%.*}"

source $dirname/../common.sh
