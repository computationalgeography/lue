#!/usr/bin/env python
import numpy as np

import lue.framework as lfr

from stress import main, Shape


class Model(lfr.Model):
    def __init__(self, array_shape: Shape, partition_shape: Shape | None):
        super().__init__()
        self.array_shape = array_shape
        self.partition_shape = partition_shape

    def initialize(self):
        self.kernel = np.full((3, 3), 1, dtype=lfr.boolean_element_type)
        self.array = lfr.uniform(
            self.array_shape,
            np.dtype(np.float32),
            min_value=0,
            max_value=1,
            partition_shape=self.partition_shape,
        )

    def simulate(self, iteration: int):
        self.array = lfr.focal_mean(self.array, self.kernel)

        return lfr.as_state(self.array)


if __name__ == "__main__":
    main(Model)
