#!/usr/bin/env python
import os.path
import sys
sys.path = [
    os.path.join(os.path.split(__file__)[0], "..", "..", "benchmark", "lue")
] + sys.path
import benchmark
import docopt


usage = """\
Import benchmark results

Usage:
    {command} (partition_shape|strong_scaling|weak_scaling) <prefix>

Options:
    partition_shape   Benchmark for partition shape
    strong_scaling    Benchmark for strong scaling characteristics
    weak_scaling      Benchmark for weak scaling characteristics
    prefix            Pathname of directory containing results
    -h --help         Show this screen
""".format(
    command = os.path.basename(sys.argv[0]))


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    if arguments["partition_shape"]:
        experiment = "partition_shape"
    elif arguments["strong_scaling"]:
        experiment = "strong_scaling"
    elif arguments["weak_scaling"]:
        experiment = "weak_scaling"

    prefix_pathname = os.path.join(arguments["<prefix>"], experiment)
    assert os.path.isdir(prefix_pathname), prefix_pathname

    if experiment == "partition_shape":
        benchmark.partition_shape.import_results(prefix_pathname)
    elif experiment == "strong_scaling":
        benchmark.strong_scaling.import_results(prefix_pathname)
    elif experiment == "weak_scaling":
        benchmark.weak_scaling.import_results(prefix_pathname)
