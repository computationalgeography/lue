dirname="$( cd "$(dirname "$BASH_SOURCE")" ; pwd -P )"

# These names relate to the main script sourcing us
script_name=`basename "$0"`
benchmark_name="${script_name%.*}"

source $dirname/../common.sh
