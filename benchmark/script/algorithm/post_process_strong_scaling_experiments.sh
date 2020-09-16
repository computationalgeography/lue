set -e


dirname="$( cd "$(dirname "$BASH_SOURCE")" ; pwd -P )"
source $dirname/header.sh
experiment_name="strong_scaling"


source $dirname/post_experiment_footer.sh
