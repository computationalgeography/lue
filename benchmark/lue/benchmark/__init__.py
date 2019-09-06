from . import partition_shape
from . import strong_scaling
from . import weak_scaling
import json
import math


def array_shape(
        memory_available,
        rank,
        nr_arrays,
        size_of_element,
        nr_nodes):
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


def json_to_data(
        pathname):
    lines = open(pathname).readlines()
    lines = "".join([line for line in lines if not line.strip().startswith("#")])

    return json.loads(lines)
