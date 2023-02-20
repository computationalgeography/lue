#!/usr/bin/env python
# -*- encoding: utf8 -*-
import functools
import math
import os.path
import sys

# sys.path = [
#     os.path.join(os.path.split(__file__)[0], "..", "..", "benchmark", "lue")
# ] + sys.path
# import benchmark
import docopt


usage = """\
Calculate memory usage given array shape used for scaling experiments

Usage:
    {command} <shape> <nr_arrays> <size_of_element> <nr_nodes>

Options:
    shape            Shape of arrays
    nr_arrays        Number of arrays used in calculation
    size_of_element  Size of elements in arrays [bytes]
    nr_nodes         Number of nodes used in calculation
    -h --help        Show this screen

Wikipedia:
    In the context of computer memory, gigabyte and GB are customarily
    used to mean 1024³ (2³⁰) bytes.
""".format(
    command=os.path.basename(sys.argv[0])
)


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    array_shape = [int(extent) for extent in arguments["<shape>"].split(",")]
    nr_arrays = int(arguments["<nr_arrays>"])
    size_of_element = int(arguments["<size_of_element>"])
    nr_nodes = int(arguments["<nr_nodes>"])

    elements_per_array = functools.reduce(lambda e1, e2: e1 * e2, array_shape)

    nr_bytes = float(nr_arrays * elements_per_array * size_of_element)
    nr_gigabytes = nr_bytes / 1000 / 1000 / 1000
    nr_gibibytes = nr_bytes / 1024 / 1024 / 1024

    assert math.fmod(nr_bytes, 1) == 0.0, nr_bytes

    print(
        """\
memory used (total):
    {:.2E} bytes
    {:.2E} Gigabyte (GB)
    {:.2E} Gibibyte (GiB)
memory used (node):
    {:.2E} bytes
    {:.2E} Gigabyte (GB)
    {:.2E} Gibibyte (GiB)""".format(
            nr_bytes,
            nr_gigabytes,
            nr_gibibytes,
            nr_bytes / nr_nodes,
            nr_gigabytes / nr_nodes,
            nr_gibibytes / nr_nodes,
        )
    )
