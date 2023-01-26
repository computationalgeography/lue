import numpy as np


def is_monotonically_increasing(array):

    # np.diff returns the difference between consecutive elements

    # Cast to float64 because when the type is unsigned int, this
    # logic fails because a negative value cannot be stored (out of range)

    return np.all(np.diff(array.astype(np.float64)) > 0)
