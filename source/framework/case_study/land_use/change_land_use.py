#!/usr/bin/env python
# coding: utf-8
import gdal
import matplotlib.pyplot as plt
import scipy.ndimage.filters as filters
import scipy.signal as signal
import numpy as np
import math
import sys

gdal.UseExceptions()


# Require operations:
# Arithmetic
# - [*] operator+(array, array), add(array, array)
# - [*] operator-(array, array), subtract(array, array)
# - [*] operator/(array, array), divide(array, array)
# - [*] operator*(array, array), multiply(array, array)

# Comparison
# - [*] operator==(array, scalar), equal_to(array, scalar)
# - [*] operator!=(array, scalar), not_equal_to(array, scalar)
# - [*] operator<(array, array), less_than(array, array)
# - [*] operator>(array, array), greater_than(array, array)
# - [*] operator>=(array, array), greater_than_equal_to(array, array)
# - [*] operator<=(array, array), less_than_equal_to(array, array)
# - [*] close_to(array, scalar), close_to(array, array)

# Logical
# - [*] operator&&(array<bool>, array<bool>), logical_and(array<bool>, array<bool>)
# - [*] operator||(array<bool>, array<bool>), logical_inclusive_or(array<bool>, array<bool>)
# - [*] operator!(array<bool>), logical_not(array<bool>)

# Aggregate
# - [*] bool all(raster<bool>)
# - [*] bool none(raster<bool>)
# - [*] bool any(raster<bool>)

# Mathematical
# - [*] array<count> sum(array<bool>) → count number of True cells
# - [*] array = sqrt(array)
# - [*] array = power(array, scalar)

# Trigonometric
# - [*] array = cos(array)
# - [*] array = sin(array)
# - [*] array = atan2(array, array)
# - [*] array = atan(array)

# Random
# - [*] array<float> uniform<float>(array, 0, 1)
# - [*] array<int> uniform<int>(array, 1, nr_land_use_types)

# Focal
# - [*] window_total, with support for skipping the focal cell
# - [*] slope(array, cell_with, cell_height)

# Misc
# - [*] where(array<bool>, true_cells, false_cells)
# - [*] array_like(array, fill_value) → create new array distributed as input
# - [*] array<float> mesh_grid(ranges)
# - [*] array = minimum(array)
# - [*] array = maximum(array)

# Skip:
# - [ ] array<float> map_range(array<float>, min, max) → map_range values to range [min, max]


def count_if(
        array):

    counts = dict(zip(*np.unique(array, return_counts=True)))

    return counts[True] if True in counts else 0


def map_range(
        array):
    # Scale array so all values are stretched within [0, 1]

    min = np.min(array)
    max = np.max(array)

    assert min != max, array

    array = (array - min) / (max - min)

    assert np.isclose(np.min(array), 0), np.min(array)
    assert np.isclose(np.max(array), 1), np.max(array)

    return array


def square_kernel(
        radius,
        dtype=np.float64):

    size = 2 * radius + 1

    return np.ones(shape=(size, size), dtype=dtype)


def convolve(
        array,
        kernel):

    return signal.fftconvolve(array, kernel, mode="same")


# def window_average(
#         array,
#         kernel_radius):
#
#     kernel = square_kernel(kernel_radius, np.float64)
#     kernel /= kernel.size
#
#     return convolve(array, kernel)


def window_total(
        array,
        kernel_radius,
        skip_center_cell=False):

    kernel = square_kernel(kernel_radius, np.float64)

    if skip_center_cell:
        kernel[1][1] = 0

    return convolve(array, kernel)


def slope(
        array,
        cell_width,
        cell_height):

    def function(
        data,
        cell_width,
        cell_height):

        rise = ((data[6] + (2 * data[7]) + data[8]) -
                (data[0] + (2 * data[1]) + data[2])) / (8 * cell_height)
        run = ((data[2] + (2 * data[5]) + data[8]) -
                (data[0] + (2 * data[3]) + data[6])) / (8 * cell_width)
        dist = np.sqrt(np.square(rise) + np.square(run))

        return np.arctan(dist) * 180 / np.pi

    return filters.generic_filter(
        array, function, size=3, mode="nearest",
        extra_arguments=(cell_width, cell_height))


def write_grid(
        array,
        name,
        time_step):

    figure, ax = plt.subplots(figsize=(5, 5))

    if np.issubdtype(array.dtype, np.bool_):
        cmap="Set1"
    elif np.issubdtype(array.dtype, np.integer):
        cmap="tab10"
    elif np.issubdtype(array.dtype, np.floating):
        cmap="gist_earth"

    plt.imshow(array, interpolation="none", cmap=cmap)

    if not np.issubdtype(array.dtype, np.bool_):
        plt.colorbar(orientation="vertical")

    ax.set_title("{}_{}".format(name, time_step))
    ax.get_xaxis().set_visible(False)
    ax.get_yaxis().set_visible(False)
    ax.set_frame_on(False)

    figure.savefig("{}_{}.pdf".format(name, time_step))


gdal_type_by_dtype = {
    np.dtype("bool"): gdal.GDT_Int32,
    np.dtype("uint8"): gdal.GDT_Int32,
    np.dtype("int32"): gdal.GDT_Int32,
    np.dtype("float32"): gdal.GDT_Float32,
    np.dtype("float64"): gdal.GDT_Float64,
}


pcr_value_scale_by_dtype = {
    np.dtype("bool"): "VS_NOMINAL",
    np.dtype("uint8"): "VS_NOMINAL",
    np.dtype("int32"): "VS_NOMINAL",
    np.dtype("float32"): "VS_SCALAR",
    np.dtype("float64"): "VS_SCALAR",
}


def write_raster(
        array,
        name,
        time_step=None):

    name = "{}_{}".format(name, time_step) if time_step is not None else name

    nr_rows, nr_cols = array.shape
    nr_bands = 1

    # driver = gdal.GetDriverByName("GTiff")
    # dataset = driver.Create(
    #     "{}.tif".format(name),
    #     nr_cols, nr_rows, nr_bands,
    #     gdal_type_by_dtype[array.dtype])

    if array.dtype == np.bool:
        array = array.astype(np.int32)  # CSF
    elif array.dtype == np.float64:
        array = array.astype(np.float32)  # CSF

    driver = gdal.GetDriverByName("PCRaster")
    dataset = driver.Create(
        "{}.map".format(name),
        nr_cols, nr_rows, nr_bands,
        gdal_type_by_dtype[array.dtype],
        options=[
            "PCRASTER_VALUESCALE={}".format(
                pcr_value_scale_by_dtype[array.dtype])])

    # dataset.SetGeoTransform(geo_transform)
    # dataset.SetProjection(projection)

    dataset.GetRasterBand(1).WriteArray(array)


class ChangeLandUse(object):

    def __init__(self,
            environment):

        self.environment = environment


    def __call__(self):

        sys.stdout.write("["); sys.stdout.flush()

        self.initialize()

        for time_step in range(self.environment.nr_time_steps):

            self.simulate(time_step)

            sys.stdout.write("."); sys.stdout.flush()

        sys.stdout.write("]\n"); sys.stdout.flush()


    def initialize(self):

        data = self.environment

        self.land_use = np.copy(data.initial_land_use)

        # Keep track of cells changed during the simulation
        self.cells_changed = np.full_like(self.land_use, False, dtype=np.bool)

        self.abbandoned = len(data.land_use_types)

        write_raster(data.elevation, "elevation")
        write_raster(data.slope, "slope")


    def simulate(self,
            time_step):

        data = self.environment

        original_land_use = np.copy(self.land_use)
        cells_changed_this_time_step = \
            np.full_like(self.land_use, False, dtype=np.bool)

        for land_use_type in data.active_land_use_types:

            suitability = self.suitability(
                time_step, land_use_type, self.land_use)

            write_raster(
                suitability, "suitability-{}".format(land_use_type),
                time_step+1)

            total_demand = self.total_demand(time_step, land_use_type)
            potential_yield = self.potential_yield(time_step, land_use_type)
            total_yield = self.total_yield(
                land_use_type, potential_yield, self.land_use)

            # Cells are converted to or removed from this land-use
            # until the total yield equals meets the total demand
            if total_demand > total_yield:

                # Consider suitabilities of those cells that have
                # not been converted during the current time step.
                # Mark all other suitabilities with 0.
                suitability = np.where(
                    np.logical_not(cells_changed_this_time_step),
                    suitability, 0)

                # Expand area occupied with current land-use type.
                while total_demand > total_yield:

                    # Consider suitabilities of those cells that
                    # do not already contain the current land-use
                    # type. Scale suitabilities to [0, 1].
                    # Mark all other suitabilities with 0.
                    suitability = np.where(
                        self.land_use != land_use_type, suitability, 0)
                    suitability = map_range(suitability)

                    self.land_use = self.expand_land_use_type(
                        self.land_use, land_use_type, suitability)
                    total_yield = self.total_yield(
                        land_use_type, potential_yield, self.land_use)

            elif total_demand < total_yield:

                # Consider suitabilities of those cells that have
                # not been converted during the current time step.
                # Mark all other suitabilities with 1.
                suitability = np.where(
                    np.logical_not(cells_changed_this_time_step),
                    suitability, 1)

                # Contract area occupied with current land-use type
                while total_demand < total_yield:

                    # Consider suitabilities of those cells that
                    # do already contain the current land-use type.
                    # Scale suitabilities to [0, 1].
                    # Mark all other suitabilities with 1.
                    suitability = np.where(
                        self.land_use == land_use_type, suitability, 1)
                    suitability = map_range(suitability)

                    self.land_use = self.contract_land_use_type(
                        self.land_use, land_use_type, suitability)
                    total_yield = self.total_yield(
                        land_use_type, potential_yield, self.land_use)

            cells_changed_this_time_step = \
                np.logical_or(
                    cells_changed_this_time_step,
                    self.land_use != original_land_use)

        self.cells_changed = \
            np.logical_or(self.cells_changed, cells_changed_this_time_step)

        write_raster(
            cells_changed_this_time_step, "changed_this_time_step",
            time_step+1)
        write_raster(self.cells_changed, "changed", time_step+1)
        write_raster(self.land_use, "land_use", time_step+1)


    def suitability(self,
            time_step,
            land_use_type,
            land_use):

        data = self.environment
        suitability = np.zeros_like(land_use, dtype=np.float64)

        for suitability_factor in data.suitability_factors[land_use_type]:

            suitability += \
                suitability_factor.weight * \
                suitability_factor(time_step, land_use_type, land_use)

        # Suitability indicating the aggregated appropriateness of each
        # location for the current land-use type at the current time-step
        # (є[0, 1])
        assert np.all(0 <= suitability)
        assert np.all(suitability <= 1)

        return suitability


    def total_demand(self,
            time_step,
            land_use_type):

        data = self.environment

        # Number of inhabitants at current time-step
        nr_inhabitants = data.population_density[time_step] * data.area

        # Demand per capita of products for current land-use type at current
        # time-step (kg / capita / year)
        demand_per_capita = data.demand_per_capita[land_use_type][time_step]

        # Extent to which the food demands are met by the domestic supply
        # at current time-step (-)
        self_sufficiency_ratio = \
            data.self_sufficiency_ratio[land_use_type][time_step]

        # Demand for products from current land-use type at current
        # time-step (kg / year)
        return nr_inhabitants * demand_per_capita * self_sufficiency_ratio


    def potential_yield(self,
            time_step,
            land_use_type):

        data = self.environment

        # Maximum possible product yield of products from current
        # land-use type at current time-step (kg / km² / year)
        max_potential_product_yield = \
            data.max_potential_product_yield[land_use_type][time_step]

        # Fraction of the maximum potential yield that can be reached
        # in a cell (є[0, 1], raster)
        actual_yield_fraction = data.actual_yield_fraction[land_use_type]

        # Yield of products from current land-use type at current time-step
        # if the cell would be occupied by that land-use type (kg / km² /
        # year, raster)
        return max_potential_product_yield * actual_yield_fraction


    def current_yield(self,
            land_use_type,
            potential_yield,
            land_use):

        # Current yield of products from current land-use type at current
        # time-step is potential yield in cells that contain that
        # land-use type (kg / km² / year)
        return np.where(land_use == land_use_type, potential_yield, 0)


    def total_yield(self,
            land_use_type,
            potential_yield,
            land_use):

        data = self.environment

        current_yield = self.current_yield(
            land_use_type, potential_yield, land_use)

        # Total yield is sum of current yield times the area of each
        # contributing cell (kg / year)
        return \
            np.sum(current_yield) * \
            np.sum(np.where(current_yield > 0, data.cell_area, 0))


    def expand_land_use_type(self,
            land_use,
            land_use_type,
            suitability):

        # When the land-use type expands, it allocates
        # new cells of this type at locations with the
        # highest suitability.

        # Convert all cells with a suitability higher than some cutoff value
        cutoff = 0.999
        nr_cells_to_convert = count_if(suitability > cutoff)
        assert nr_cells_to_convert > 0

        nr_cells_containing_land_use_before = \
            count_if(land_use == land_use_type)

        land_use = np.where(suitability > cutoff, land_use_type, land_use)

        nr_cells_containing_land_use_after = \
            count_if(land_use == land_use_type)

        nr_cells_converted = \
            nr_cells_containing_land_use_after - \
            nr_cells_containing_land_use_before

        assert nr_cells_converted > 0
        assert nr_cells_converted == nr_cells_to_convert, nr_cells_converted

        return land_use


    def contract_land_use_type(self,
            land_use,
            land_use_type,
            suitability):

        # When the land-use type contracts, it removes cells
        # of this type at locations with the lowest suitability.

        # Convert all cells with a suitability lower than some cutoff value
        cutoff = 0.001
        nr_cells_to_convert = count_if(suitability < cutoff)

        assert nr_cells_to_convert > 0

        nr_cells_containing_land_use_before = \
            count_if(land_use == land_use_type)

        land_use = np.where(suitability < cutoff, self.abbandoned, land_use)

        nr_cells_containing_land_use_after = \
            count_if(land_use == land_use_type)

        nr_cells_converted = \
            nr_cells_containing_land_use_before - \
            nr_cells_containing_land_use_after

        assert nr_cells_converted > 0
        assert nr_cells_converted == nr_cells_to_convert, nr_cells_converted

        return land_use


class SuitabilityFactor(object):

    def __init__(self,
            weight):

        self.weight = weight


    def __call__(self,
            time_step,
            land_use_type,
            land_use):

        return self.calculate(time_step, land_use_type, land_use)


    def calculate(self,
            time_step,
            land_use_type,
            land_use):

        raise NotImplementedError("SuitabilityFactor.calculate")


class Autocorrelation(SuitabilityFactor):

    def __init__(self,
            weight):

        SuitabilityFactor.__init__(self, weight)


    def calculate(self,
            time_step,
            land_use_type,
            land_use):

        # Count the number of occurrences of value in the neighborhood
        # of each cell. The more occurrences, the better. Scale
        #occurrences between 0(no occurrences) and 1(8 occurrences).

        cells_to_count = land_use == land_use_type
        suitability = \
            window_total(cells_to_count, 1, skip_center_cell=True) / 8

        suitability = np.clip(suitability, 0, 1)
        assert np.all(0 <= suitability)
        assert np.all(suitability <= 1)

        return suitability


class Randomness(SuitabilityFactor):

    def __init__(self,
            weight):

        SuitabilityFactor.__init__(self, weight)


    def calculate(self,
            time_step,
            land_use_type,
            land_use):

        return map_range(np.random.uniform(size=land_use.shape))


class SuitableArea(SuitabilityFactor):

    def __init__(self,
            weight,
            selection):

        SuitabilityFactor.__init__(self, weight)
        self.selection = selection.astype(np.float64)


    def calculate(self,
            time_step,
            land_use_type,
            land_use):

        suitability = self.selection

        assert np.all(0 <= suitability)
        assert np.all(suitability <= 1)

        assert np.any(suitability == 0)
        assert np.any(suitability == 1)

        return suitability


class Environment(object):

    def __init__(self):
        self.nr_time_steps = 100

        nr_rows = 500
        nr_cols = 500
        nr_cells = nr_rows * nr_cols
        cell_size = 0.1  # km: 100m
        self.cell_area = cell_size * cell_size  # km²
        self.area = nr_cells * self.cell_area  # km²

        nr_land_use_types = 6
        self.initial_land_use = np.random.randint(
            low=1, high=nr_land_use_types+1, size=(nr_rows, nr_cols),
            dtype=np.int32)
        self.land_use_types = range(1, nr_land_use_types+1)
        self.active_land_use_types = [1]

        y, x = np.mgrid[-1.0:1.0:2.0 / nr_rows, -1.0:1.0:2.0 / nr_cols]
        self.elevation = \
            3.0 * y * (3.0 * x**2 - y**2) / 4.0 + \
            0.5 * np.cos(
                6.0 * math.pi * np.sqrt(x**2 + y**2) + np.arctan2(x, y))
        self.slope = slope(self.elevation, cell_size, cell_size)

        # Suitabilityfactors for the case of urban settlements
        self.suitability_factors = {
                1: [
                        Autocorrelation(weight=0.3),
                        Randomness(
                            weight=0.1),
                        SuitableArea(weight=0.6, selection=self.slope < 10)
                    ]
            }

        for factors in self.suitability_factors.values():
            assert np.isclose(
                np.sum([factor.weight for factor in factors]), 1.0)

        # https://data.worldbank.org/indicator/EN.POP.DNST, 2015
        self.population_density = \
            np.empty(shape=(self.nr_time_steps), dtype=np.float64)
        self.population_density[0] = 60000  # per km²
        for time_step in range(1, self.nr_time_steps):
            # https://ourworldindata.org/world-population-growth
            # Rate of growth per year ~ 1%
            self.population_density[time_step] = \
                self.population_density[time_step-1] * 1.01

        self.demand_per_capita = {}
        self.self_sufficiency_ratio = {}
        self.max_potential_product_yield = {}
        self.actual_yield_fraction = {}

        # Iterate over all active land-use types
        for land_use_type in self.active_land_use_types:

            # Demand -------------------------------------------------------------------
            self.demand_per_capita[land_use_type] = [
                # FIXME vary this(?)
                100 for time_step in range(self.nr_time_steps)]   # kg / capita / year

            self.self_sufficiency_ratio[land_use_type] = [
                # FIXME vary this(?)
                0.8 for time_step in range(self.nr_time_steps)]   # -

            # Yield --------------------------------------------------------------------
            self.max_potential_product_yield[land_use_type] = [
                # FIXME vary this(?)
                1e5 for time_step in range(self.nr_time_steps)]   # kg / km² / year

            # FIXME vary this based on physical factors (slope, water, ...)
            self.actual_yield_fraction[land_use_type] = \
                np.full_like(self.initial_land_use, 0.5, dtype=np.float64)


environment = Environment()

ChangeLandUse(environment)()


# def window_count(
#         array,
#         kernel_radius,
#         land_use_type):
#
#     def function(
#             array,
#             land_use_type):
#
#         return np.count_nonzero(array == land_use_type)
#
#     kernel_size = 2 * kernel_radius + 1
#
#     return filters.generic_filter(
#         array, function, size=kernel_size,
#         mode="constant", cval=0, origin=0,
#         extra_arguments=(land_use_type))
