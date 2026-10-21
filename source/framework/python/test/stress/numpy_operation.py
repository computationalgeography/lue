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
        self.state = lfr.uniform(
            self.array_shape,
            np.dtype(np.float32),
            min_value=0,
            max_value=1,
            partition_shape=self.partition_shape,
        )

    def simulate(self, iteration: int):
        self.state = lfr.from_numpy(lfr.to_numpy(self.state))

        return lfr.as_state(self.state)


if __name__ == "__main__":
    main(Model)
