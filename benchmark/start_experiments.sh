set -e

experiments="partition_shape strong_scaling weak_scaling"

for experiment in $experiments; do
    $LUE/benchmark/${experiment}_experiments.sh
done
