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
        condition = lfr.create_array(self.array_shape, lfr.boolean_element_type, 1)
        x_coordinates = lfr.cast(lfr.cell_index(condition, 1), np.float32)
        y_coordinates = lfr.cast(lfr.cell_index(condition, 0), np.float32)
        elevation = lfr.open_simplex_noise(x_coordinates, y_coordinates, 5)

        self.flow_direction = lfr.d8_flow_direction(elevation)
        self.material = lfr.uniform(
            self.array_shape,
            np.dtype(np.float32),
            min_value=0,
            max_value=1,
            partition_shape=self.partition_shape,
        )

    def simulate(self, iteration: int):

        # TODO: Try again once channel stuff is refactored
        # - faster?
        # - less memory?
        # If so, wrap-up the channel stuff and continue with other memory accumulation issues
        self.material = lfr.accu(self.flow_direction, self.material)

        return lfr.as_state(self.material)


if __name__ == "__main__":
    main(Model)
