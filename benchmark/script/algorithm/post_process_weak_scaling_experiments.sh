set -e


dirname="$( cd "$(dirname "$BASH_SOURCE")" ; pwd -P )"
source $dirname/header.sh
experiment_name="weak_scaling"


# algorithm_names="
# "
# scenarios="
# "


source $dirname/post_experiment_footer.sh
