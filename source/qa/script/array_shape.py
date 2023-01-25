#!/usr/bin/env python
# -*- encoding: utf8 -*-
import docopt
import functools
import math
import os.path
import sys


usage = """\
Calculate array shape to use for scaling experiments

Usage:
    {command} <memory> <rank> <nr_arrays> <size_of_element> <nr_nodes>

Options:
    memory           Amount of memory to use per node [GiB == 2³⁰ bytes]
    rank             Rank of arrays
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


def array_shape(memory_available, rank, nr_arrays, size_of_element, nr_nodes):
    """
    Determine a maximum array shape to use in scaling experiments, given the
    amount of memory to use, and properties of the arrays

    memory_available: bytes per node
    rank: int
    nr_arrays: int
    size_of_element: bytes
    nr_nodes: int
    """
    assert memory_available > 0
    assert rank > 0
    assert nr_arrays > 0
    assert size_of_element > 0
    assert nr_nodes > 0

    nr_elements = float(nr_nodes * memory_available) / size_of_element
    elements_per_array = nr_elements / nr_arrays
    elements_per_dimension = math.pow(elements_per_array, 1.0 / rank)

    elements_per_dimension = int(math.floor(elements_per_dimension))

    return rank * (elements_per_dimension,)


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    memory_available = int(arguments["<memory>"])
    memory_available *= 2**30
    rank = int(arguments["<rank>"])
    nr_arrays = int(arguments["<nr_arrays>"])
    size_of_element = int(arguments["<size_of_element>"])
    nr_nodes = int(arguments["<nr_nodes>"])

    array_shape_ = array_shape(
        memory_available, rank, nr_arrays, size_of_element, nr_nodes
    )
    elements_per_array = functools.reduce(lambda e1, e2: e1 * e2, array_shape_)
    memory_required_per_array = elements_per_array * size_of_element
    memory_required = nr_arrays * memory_required_per_array

    assert memory_required - (nr_nodes * memory_available) < 1e-4

    # assert all([math.fmod(extent, nr_nodes) == 0 for extent in array_shape_])

    array_shape_per_node = tuple([float(extent) / nr_nodes for extent in array_shape_])

    assert all([isinstance(extent, float) for extent in array_shape_per_node])

    print(
        """\
shape (total): {}
shape (node): {}""".format(
            array_shape_, array_shape_per_node
        )
    )
