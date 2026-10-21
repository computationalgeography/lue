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
        pass

    def simulate(self, iteration: int):
        array = lfr.uniform(
            self.array_shape,
            np.dtype(np.float32),
            min_value=0,
            max_value=1,
            partition_shape=self.partition_shape,
        )
        maximum = lfr.maximum(array)

        return lfr.as_state(maximum)


if __name__ == "__main__":
    main(Model)
