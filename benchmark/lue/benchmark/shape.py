from functools import reduce


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

    assert all(
        [isinstance(multiplier, int) for multiplier in shape_multipliers])

    return shape_multipliers


def ranges_of_partition_shape_multipliers(
        shape,
        min_partition_shape,
        max_partition_shape):
    """
    Return for each dimension a range of multipliers
    """
    min_partition_shape_multipliers = partition_shape_multipliers(
        shape, min_partition_shape)
    max_partition_shape_multipliers = partition_shape_multipliers(
        shape, max_partition_shape)

    rank = len(shape)

    assert all([
        min_partition_shape_multipliers[r] >=
        max_partition_shape_multipliers[r] for r in range(rank)])

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


def range_of_array_shapes(
        min_shape,
        max_nr_elements,
        multiplier):
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
            int(round(nr_elements_per_dimension * extent)) for extent in
                normalized_shape])


    sizes = [nr_elements(min_shape)]

    while multiplier * sizes[-1] <= max_nr_elements:
        sizes.append(multiplier * sizes[-1])

    normalized_shape = tuple([extent / max(min_shape) for extent in min_shape])
    shapes = [shape(size, normalized_shape) for size in sizes]

    assert len(shapes) == 0 or nr_elements(shapes[-1]) <= max_nr_elements, \
        shapes

    return shapes


class Range(object):

    def __init__(self,
            json):
        self.max_nr_elements = json["max_nr_elements"]
        self.multiplier = json["multiplier"]


class Shape(object):

    def __init__(self,
            json):

        self.shape_ = tuple(json["shape"])
        self.range = Range(json["range"]) if "range" in json else None


    def is_fixed(self):

        return self.range is None


    def shapes(self):
        if self.is_fixed():
            result = [self.shape_]
        else:
            # Range of shapes
            result = range_of_array_shapes(
                self.shape_, self.range.max_nr_elements, self.range.multiplier)

        return result

    def shape(self):
        """
        Return shape

        It is assumed that this instance represents a fixed shape,
        not a range of shapes.
        """
        assert self.is_fixed()

        return self.shape_
