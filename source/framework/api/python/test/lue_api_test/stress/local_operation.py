#!/usr/bin/env python
import numpy as np

import lue.framework_x as lfrx

from stress import main, Shape


class Model(lfrx.Model):
    def __init__(self, array_shape: Shape, partition_shape: Shape | None):
        super().__init__()
        self.array_shape = array_shape
        self.partition_shape = partition_shape

    def initialize(self):
        self.state = lfrx.uniform(
            self.array_shape,
            self.partition_shape,
            lfrx.as_field(lfrx.create_scalar(0, np.float32)),
            lfrx.as_field(lfrx.create_scalar(1, np.float32)),
        )

    def simulate(self, iteration: int):
        self.state = self.state + self.state - self.state + self.state - self.state

        return lfrx.as_state(self.state)


if __name__ == "__main__":
    main(Model)
