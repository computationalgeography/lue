from functools import reduce
import math


def partition_shape_multipliers(
        shape,
        partition_shape):
    """
    Given an array shape and a partition shape, determine for each dimension
    how many times the partition's extent can fit in the shape's extent.

    Returns a list of multipliers: for each dimension one.
    """

    rank = len(shape)
    assert len(partition_shape) == rank

    assert all([extent > 0 for extent in shape])
    assert all([extent > 0 for extent in partition_shape])

    for r in range(rank):
        assert shape[r] % partition_shape[r] == 0

    shape_multipliers = [shape[r] // partition_shape[r] for r in range(rank)]

    assert all([isinstance(multiplier, int) for multiplier in shape_multipliers])

    return shape_multipliers


def ranges_of_partition_shape_multipliers(
        shape,
        min_partition_shape,
        max_partition_shape):
    """
    Return for each dimension a range of multipliers
    """
    min_partition_shape_multipliers = partition_shape_multipliers(shape, min_partition_shape)
    max_partition_shape_multipliers = partition_shape_multipliers(shape, max_partition_shape)

    rank = len(shape)

    assert all([
        min_partition_shape_multipliers[r] >= max_partition_shape_multipliers[r] for r in range(rank)])

    multiplier_ranges = [
            range(
                min_partition_shape_multipliers[r],
                max_partition_shape_multipliers[r], -1) for r in range(rank)]

    assert len(multiplier_ranges) == rank

    return multiplier_ranges


def shape_ranges(
        min_shape,
        max_shape,
        step):

    assert len(min_shape) == len(max_shape)
    assert step > 0
    rank = len(min_shape)
    assert rank > 0
    assert all([min_shape[r] <= max_shape[r] for r in range(rank)])

    return [range(min_shape[r], max_shape[r] + 1, step) for r in range(rank)]


def partition_shapes(
        min_shape,
        max_shape,
        step):

    # TODO Rename this function. Related to shapes, not partition shapes

    shape_ranges_ = shape_ranges(min_shape, max_shape, step)

    return zip(*shape_ranges_)


def range_of_shapes(
        min_shape,
        max_nr_elements,
        multiplier,
        method):
    """
    Determine a range of shapes given the folowing requirements:
    - First shape equals min_shape
    - Number of cells in last shape smaller or equal to max_nr_elements
    - Each next shape contains multiplier times more elements than the
        previous shape
    """
    def nr_elements(
            shape):

        return reduce(lambda e1, e2: e1 * e2, shape)


    def shape(
            nr_elements,
            normalized_shape):

        rank = len(normalized_shape)

        nr_elements_per_dimension = nr_elements ** (1.0 / rank)

        return tuple([
            int(math.floor(nr_elements_per_dimension * extent)) for extent in
                normalized_shape])


    sizes = [nr_elements(min_shape)]

    assert method in ["linear", "exponential"]

    assert multiplier > 1, multiplier

    if method == "linear":

        # Increase the number of cells linearly, by
        # i * (multiplier - 1) * sizes[0]
        new_size = lambda i: sizes[0] + i * (multiplier - 1) * sizes[0]
        i = 1

        while new_size(i) <= max_nr_elements:
            sizes.append(new_size(i))
            i += 1

    elif method == "exponential":

        # Increase the number of cells exponentially, by multiplier *
        # previous number of elements
        while multiplier * sizes[-1] <= max_nr_elements:
            sizes.append(multiplier * sizes[-1])

    normalized_shape = tuple([float(extent) / max(min_shape) for extent in min_shape])
    shapes = [shape(size, normalized_shape) for size in sizes]

    assert len(shapes) == 0 or nr_elements(shapes[-1]) <= max_nr_elements, \
        "{}, {}".format(shapes, max_nr_elements)

    return shapes


class Range(object):

    def __init__(self,
            data):

        self.from_json(data)

    def from_json(self, data):

        self.max_nr_elements = data["max_nr_elements"]
        self.multiplier = data["multiplier"]
        self.method = data["method"]

        assert self.method in ["linear", "exponential"]

        if self.method == "linear":
            assert self.multiplier > 1

    def to_json(self):

        return {
                "max_nr_elements": self.max_nr_elements,
                "multiplier": self.multiplier,
                "method": self.method,
            }


class Shape(object):

    def __init__(self,
            data):

        self.from_json(data)

    def from_json(self, data):

        self._shape = tuple(data["shape"])
        self._range = Range(data["range"]) if "range" in data else None

    def to_json(self):

        result = {
                "shape": self._shape
            }

        if self._range:
            result["range"] = self._range.to_json()

        return result

    def is_fixed(self):

        return self._range is None

    @property
    def shapes(self):

        if self.is_fixed():
            result = [self._shape]
        else:
            # Range of shapes
            result = range_of_shapes(
                self._shape, self._range.max_nr_elements,
                self._range.multiplier, self._range.method)

        return result

    @property
    def shape(self):
        """
        Return shape

        It is assumed that this instance represents a fixed shape,
        not a range of shapes.
        """
        assert self.is_fixed()

        return self._shape
