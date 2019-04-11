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


class Shape(object):

    def __init__(self,
            json):

        if "shape" in json:
            # Fixed size shape
            self.min_shape = tuple(json["shape"])
            self.max_shape = self.min_shape
            self.shape_step = None
        else:
            # Range of shapes
            self.min_shape = tuple(json["min_shape"])
            self.max_shape = tuple(json["max_shape"])
            self.shape_step = json["shape_step"]


    def is_fixed(self):

        return self.shape_step is None


    def shapes(self):
        if self.is_fixed():
            assert self.min_shape == self.max_shape
            result = [self.min_shape]
        else:
            # Range of shapes
            result = partition_shapes(
                self.min_shape, self.max_shape, self.shape_step)

        return result

    def shape(self):
        """
        Return shape

        It is assumed that this instance represents a fixed shape,
        not a range of shapes.
        """
        assert self.is_fixed()

        return self.min_shape
