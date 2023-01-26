#!/usr/bin/env python
# coding: utf-8
from pcraster import *
from pcraster.framework import *


# - [ ] uniform
# - [ ] less_than
# - [ ] maximum
# - [ ] focal_mean
# - [ ] where(array, scalar, scalar)
# - [ ] divide
# - [ ] focal_sum
# - [ ] logical_not / boolean_not / !
# - [ ] logical_and / boolean_and / &&
# - [ ] multiply
# - [ ] larger_than
# - [ ] logical_or / boolean_or / ||
# - [ ] where(array, array, array)
# - [ ] where(array, scalar, array)


class ForestFireModel(DynamicModel):
    def __init__(self):

        DynamicModel.__init__(self)
        setclone("clone.map")

    def initial(self):

        # Initial burning locations, self.fire represents area that is
        # burning or has burnt down
        # self.fire = order(uniform(1)) < 5
        self.fire = uniform(1) < 1e-5
        assert cellvalue(mapmaximum(scalar(self.fire)), 1)[0] == 1

        # Actually burning
        self.burning = self.fire

        # Create two areas that differ in how fast they catch fire. I
        # use a window to make large patches. To make them more round
        # use a circular window, the larger the window the larger the
        # patches will be
        burnability = windowaverage(uniform(1), 50 * celllength()) < 0.5
        report(burnability, "burnability")

        # Assign probabilities for catching fire
        self.ignite_probability = ifthenelse(burnability, scalar(0.05), 0.01)

        # Assign probabilities for jump fire, I assume it is a linear
        # relation with self.ignite_probability, but lower
        self.spot_ignite_probability = self.ignite_probability / 50.0

        self.fire_age = scalar(0.0)

        self.report(self.ignite_probability, "ignite_probability")
        self.report(self.spot_ignite_probability, "spot_ignite_probability")

    def dynamic(self):

        # Find pixels where at least one neighbour is burning and that
        # are not yet burning or burnt down. This should be window4total
        # (NOT a Moore neighbourhood)
        cells_not_burning_surrounded_by_fire = (
            window4total(scalar(self.burning)) > scalar(0)
        ) & (~self.fire)

        # Select cells that catch new fire from direct neighbours
        new_fire = cells_not_burning_surrounded_by_fire & (
            uniform(1) < self.ignite_probability
        )

        # Find pixels that have not burned down or at fire and that have
        # fire pixels over a distance (jump dispersal). This should be
        # a round window preferable, diameter I do not know
        jump_cells = (windowtotal(scalar(self.burning), 5 * celllength()) > 0.5) & (
            ~self.fire
        )

        # Select cells that catch new fire by jumping fire
        new_fire_jump = jump_cells & (uniform(1) < self.spot_ignite_probability)

        # Update fire
        self.fire = (self.fire | new_fire) | new_fire_jump

        # Age of fire (in timesteps)
        self.fire_age = ifthenelse(self.fire, self.fire_age + 1.0, self.fire_age)

        # Burning cells
        self.burning = self.fire & (self.fire_age < 30.0)

        # State
        state = ifthenelse(
            self.burning, nominal(1), ifthenelse(self.fire, nominal(2), 3)
        )

        self.report(self.fire, "fire")
        self.report(self.burning, "burning")
        self.report(state, "state")


nr_time_steps = 5000  # timestep is about a minute? depends on cell size
fire_model = ForestFireModel()
DynamicFramework(fire_model, nr_time_steps).run()
