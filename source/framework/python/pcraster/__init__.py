"""
This module contains wrappers around LUE operations that behave as PCRaster operations as much
as possible. This makes it easier to port models that use the PCRaster Python package to use
the LUE Python package.

Currently, it is not possible to provide wrappers for all PCRaster operations. In some cases,
we can work around missing functionality in LUE by doing things slightly different. This may
involve a performance penalty. Over time these workarounds will be removed.

In case a PCRaster operation supports passing in a non-spatial value, but the LUE operation
does not, the wrappers will convert the non-spatial values into a spatial value automatically.

In case PCRaster contains an operation which LUE does not, then we try to work around it by using
other operations that produce the same results. If no work-around can be used, a
NotImplementedError exception is raised.

LUE does not support value scales. The wrappers in this module assume the cell representations
used are the PCRaster ones:

- uint8 for LDD and boolean values
- int32 for nominal and ordinal values
- float32 for scalar and directional values

LUE itself supports additional cell representations (int8, uint32, int64, uint64, float64).

LUE does not have the notion of non-spatial. This is relevant when passing two "non-spatial"
arguments to an operator (e.g.: ``a & b``). The regular Python rules are then in effect.
"""

import enum
import os
import pathlib
from dataclasses import dataclass

import numpy as np

import lue.framework as lfr


@dataclass(kw_only=True)
class BoundingBox:
    """
    Class for keeping track of the bounding box of a raster
    """

    north: float = 0.0
    west: float = 0.0
    south: float = 0.0
    east: float = 0.0


class Configuration(object):
    """
    Class for storing information that is required for LUE to be able to mimic PCRaster

    PCRaster makes use of a clone raster. This raster contains information about the shape of
    the underlying array and the size of the raster cells. This class fulfills the same role.
    """

    def __init__(
        self,
        *,
        bounding_box: BoundingBox | None = None,
        cell_size: float | None = None,
        array_shape: tuple[int, int] | None = None,
        partition_shape: tuple[int, int] | None = None,
    ):
        self.bounding_box = bounding_box
        self.cell_size = cell_size
        self.array_shape = array_shape
        self.partition_shape = partition_shape
        self.pathname = ""
        self.timeseries_files: dict[str, list[str]] = {}


"""
Global variable that contains information required by some of the LUE operations used in the
wrappers
"""
configuration = Configuration()


def setclone(pathname):
    if not pathlib.Path(pathname).exists():
        raise RuntimeError(f"setclone: File '{pathname}' does not exist")

    raster_properties = lfr.probe_gdal(pathname)
    array_shape = raster_properties["shape"]

    geo_transform = raster_properties["geo_transform"]
    cell_shape = geo_transform[1], -geo_transform[5]
    assert cell_shape[0] == cell_shape[1]
    cell_size = cell_shape[0]

    nr_rows, nr_cols = array_shape

    west = geo_transform[0]
    north = geo_transform[3]
    east = west + nr_cols * cell_size
    south = north - nr_rows * cell_size
    bounding_box = BoundingBox(north=north, west=west, south=south, east=east)

    configuration.bounding_box = bounding_box
    configuration.cell_size = cell_size
    configuration.array_shape = tuple(array_shape)

    partition_shape = os.getenv("LUE_PARTITION_SHAPE", default=None)

    if partition_shape is not None:
        partition_shape = tuple(int(extent) for extent in partition_shape.split(","))
        partition_shape = partition_shape[0], partition_shape[1]

    configuration.partition_shape = partition_shape
    configuration.pathname = pathname
    configuration.timeseries_files = {}


def clone():
    if not hasattr(Configuration, "cellSize"):

        def cellSize(self):
            return self.cell_size

        setattr(Configuration, "cellSize", cellSize)
    if not hasattr(Configuration, "nrRows"):

        def nrRows(self):
            return self.array_shape[0]

        setattr(Configuration, "nrRows", nrRows)
    if not hasattr(Configuration, "nrCols"):

        def nrCols(self):
            return self.array_shape[1]

        setattr(Configuration, "nrCols", nrCols)

    return configuration


def setglobaloption(option):
    print(f"lue.pcraster: discarding global option {option}\n")


arithmetic_element_types = [np.uint8, np.int32, np.float32]
boolean_element_type = np.uint8
flow_direction_element_type = np.uint8
floating_point_element_types = [np.float32]


def numpy_scalar_type(expression):
    element_type_by_type = {
        getattr(lfr, "PartitionedArray<uint8, 2>"): np.uint8,
        getattr(lfr, "PartitionedArray<int32, 2>"): np.int32,
        getattr(lfr, "PartitionedArray<float32, 2>"): np.float32,
        getattr(lfr, "Scalar<uint8>"): np.uint8,
        getattr(lfr, "Scalar<int32>"): np.int32,
        getattr(lfr, "Scalar<float32>"): np.float32,
        bool: np.uint8,
        int: np.int32,
        float: np.float32,
        np.uint8: np.uint8,
        np.int32: np.int32,
        np.float32: np.float32,
        np.float64: np.float32,  # Yes, downcasting to PCRaster's cell representation
    }

    return element_type_by_type[type(expression)]


VALUESCALE = enum.Enum(
    "VALUESCALE",
    ["Boolean", "Directional", "Ldd", "Nominal", "Ordinal", "Scalar"],
)
(
    Boolean,
    Directional,
    Ldd,
    Nominal,
    Ordinal,
    Scalar,
) = VALUESCALE


def pcr2numpy(array, no_data_value):
    return lfr.to_numpy(array, no_data_value)


def numpy2pcr(data_type, array, no_data_value):
    assert (
        (data_type in [Boolean, Ldd] and array.dtype == np.uint8)
        or (data_type in [Nominal, Ordinal] and array.dtype == np.int32)
        or (data_type == Scalar and array.dtype == np.float32)
    ), f"{data_type} vs {array.dtype}"

    return lfr.from_numpy(
        array,
        partition_shape=configuration.partition_shape,
        no_data_value=no_data_value,
    )


def lue_is_uint8_raster(argument):
    """
    Return whether `argument` is an asynchronous LUE uint8 raster
    """
    return isinstance(
        argument, getattr(lfr, "PartitionedArray<uint8, 2>")
    )  # Boolean, LDD


def lue_is_int32_raster(argument):
    """
    Return whether `argument` is an asynchronous LUE int32 raster
    """
    return isinstance(
        argument, getattr(lfr, "PartitionedArray<int32, 2>")
    )  # Nominal, ordinal


def lue_is_float32_raster(argument):
    """
    Return whether `argument` is an asynchronous LUE float32 raster
    """
    return isinstance(
        argument, getattr(lfr, "PartitionedArray<float32, 2>")
    )  # Scalar, directional


def lue_is_uint8_scalar(argument):
    """
    Return whether `argument` is a LUE asynchronous scalar uint8 value
    """
    return isinstance(argument, getattr(lfr, "Scalar<uint8>"))  # Boolean


def lue_is_int32_scalar(argument):
    """
    Return whether `argument` is a LUE asynchronous scalar int32 value
    """
    return isinstance(argument, getattr(lfr, "Scalar<int32>"))  # Nominal, ordinal


def lue_is_float32_scalar(argument):
    """
    Return whether `argument` is a LUE asynchronous scalar float32 value
    """
    return isinstance(argument, getattr(lfr, "Scalar<float32>"))  # Scalar


def is_boolean(expression):
    """
    Return whether `expression`'s value scale is boolean
    """
    return lue_is_uint8_raster(expression) or lue_is_uint8_scalar(expression)


def is_ldd(expression):
    """
    Return whether `expression`'s value scale is LDD
    """
    return lue_is_uint8_raster(expression)  # or lue_is_uint8_scalar(expression)


def is_nominal(expression):
    """
    Return whether `expression`'s value scale is nominal
    """
    return lue_is_int32_raster(expression) or lue_is_int32_scalar(expression)


def is_ordinal(expression):
    """
    Return whether `expression`'s value scale is ordinal
    """
    return lue_is_int32_raster(expression) or lue_is_int32_scalar(expression)


def is_scalar(expression):
    """
    Return whether `expression`'s value scale is scalar
    """
    return lue_is_float32_raster(expression) or lue_is_float32_scalar(expression)


def is_directional(expression):
    """
    Return whether `expression`'s value scale is directional
    """
    return lue_is_float32_raster(expression)  # or lue_is_float32_scalar(expression)


def is_spatial(argument):
    """
    Return whether `argument` is spatial

    A spatial is represented by a LUE asynchronous raster
    """
    return (
        lue_is_uint8_raster(argument)
        or lue_is_int32_raster(argument)
        or lue_is_float32_raster(argument)
    )


def is_non_spatial(argument):
    """
    Return whether `argument` is non-spatial

    A non-spatial is represented by a LUE asynchronous scalar
    """
    return (
        lue_is_uint8_scalar(argument)
        or lue_is_int32_scalar(argument)
        or lue_is_float32_scalar(argument)
        # or lue_is_value(argument)
    )


def lue_is_value(argument):
    """
    Return whether `argument` is a numeric value
    """
    return isinstance(
        argument,
        (
            int,
            float,
            np.int8,
            np.int32,
            np.int64,
            np.uint8,
            np.uint32,
            np.uint64,
            np.float32,
            np.float64,
        ),
    )


def non_spatial_to_spatial(fill_value, template=None):
    if lue_is_value(fill_value):
        fill_value = lfr.create_scalar(numpy_scalar_type(fill_value), fill_value)

    if template is None:
        array_shape = configuration.array_shape
    else:
        array_shape = template.shape

    partition_shape = configuration.partition_shape

    return lfr.create_array(array_shape, fill_value, partition_shape=partition_shape)


def harmonize_types(expression1, expression2):
    # If both arguments are values, fix the element type of the first one
    # TODO Could determine the largest one and use that
    if lue_is_value(expression1) and lue_is_value(expression2):
        expression1 = lfr.create_scalar(numpy_scalar_type(expression1), expression1)

    # If one of the arguments is a value, fix its element type to the element type of the other argument
    to_scalar_1 = (
        is_spatial(expression2) or is_non_spatial(expression2)
    ) and lue_is_value(expression1)
    to_scalar_2 = (
        is_spatial(expression1) or is_non_spatial(expression1)
    ) and lue_is_value(expression2)

    if to_scalar_2:
        expression2 = lfr.create_scalar(
            np.dtype(numpy_scalar_type(expression1)), expression2
        )

    if to_scalar_1:
        expression1 = lfr.create_scalar(
            np.dtype(numpy_scalar_type(expression2)), expression1
        )

    assert not lue_is_value(expression1), expression1
    assert not lue_is_value(expression2), expression2
    assert (
        expression1.dtype == expression2.dtype
    ), f"{expression1.dtype} != {expression2.dtype}"

    return expression1, expression2


def read_if_necessary(*args) -> tuple:
    return tuple(map(lambda arg: readmap(arg) if isinstance(arg, str) else arg, args))


def translate_window_length(pcraster_window_length):
    """
    Translate PCRaster window lengths in real distance units, to LUE window lengths, in whole
    number of cells

    The assumption here is that @a pcraster_window_length passed in:
    - is dividable by the cell size
    - corresponds with an odd number of cells (odd_number * cell_size)
    - is positive
    """
    assert lue_is_value(pcraster_window_length), pcraster_window_length

    window_length = pcraster_window_length / configuration.cell_size

    assert window_length % 1 == 0, window_length  # Must not be fractional
    assert window_length % 2 == 1, window_length  # Must be odd
    assert window_length > 0, window_length  # Must be positive

    window_length = int(window_length)

    # assert window_length > 1, "Window lengths should be larger than one cell size"

    return window_length


def readmap(pathname):
    return lfr.from_gdal(pathname, partition_shape=configuration.partition_shape)


def report(expression, pathname):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=expression)

    lfr.to_gdal(expression, pathname, clone_name=configuration.pathname)


def div(expression1, expression2):
    return expression1 / expression2


def abs(expression):
    # TODO if is_non_spatial(expression):
    # TODO     expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return lfr.abs(expression)


def accucapacity(flow_direction, inflow, capacity):
    flow_direction = ldd(flow_direction)
    inflow = scalar(inflow)
    capacity = scalar(capacity)

    return lfr.accu_capacity(flow_direction, inflow, capacity)


def accucapacityflux(flow_direction, inflow, capacity):
    return accucapacity(flow_direction, inflow, capacity)[0]


def accucapacitystate(flow_direction, inflow, capacity):
    return accucapacity(flow_direction, inflow, capacity)[1]


def accuflux(flow_direction, inflow):
    flow_direction = ldd(flow_direction)
    inflow = scalar(inflow)

    return lfr.accu(flow_direction, inflow)


def accufraction(flow_direction, inflow, fraction):
    flow_direction = ldd(flow_direction)
    inflow = scalar(inflow)
    threshold = scalar(fraction)

    return lfr.accu_fraction(flow_direction, inflow, fraction)


def accufractionflux(flow_direction, inflow, fraction):
    return accufraction(flow_direction, inflow, fraction)[0]


def accufractionstate(flow_direction, inflow, fraction):
    return accufraction(flow_direction, inflow, fraction)[1]


def accuthreshold(flow_direction, inflow, threshold):
    flow_direction = ldd(flow_direction)
    inflow = scalar(inflow)
    threshold = scalar(threshold)

    return lfr.accu_threshold(flow_direction, inflow, threshold)


def accuthresholdflux(flow_direction, inflow, threshold):
    return accuthreshold(flow_direction, inflow, threshold)[0]


def accuthresholdstate(flow_direction, inflow, threshold):
    return accuthreshold(flow_direction, inflow, threshold)[1]


def accutriggerflux(flow_direction, material, transporttrigger):
    raise NotImplementedError("accutriggerflux")


def accutriggerstate(flow_direction, material, transporttrigger):
    raise NotImplementedError("accutriggerstate")


def accutraveltimeflux(flow_direction, material, transporttraveltime):
    raise NotImplementedError("accutraveltimeflux")


def accutraveltimestate(flow_direction, material, transporttraveltime):
    raise NotImplementedError("accutraveltimestate")


def accutraveltimefractionflux(flow_direction, material, transporttraveltime):
    raise NotImplementedError("accutraveltimefractionflux")


def accutraveltimefractionstate(flow_direction, material, transporttraveltime):
    raise NotImplementedError("accutraveltimefractionstate")


def acos(expression):
    return lfr.acos(expression)


def pcrand(expression1, expression2):
    return expression1 & expression2


def argorder(*args):
    raise NotImplementedError("argorder")


def argorderwithid(*args):
    raise NotImplementedError("argorderwithid")


def argorderaddarealimited(*args):
    raise NotImplementedError("argorderaddarealimited")


def argorderwithidaddarealimited(*args):
    raise NotImplementedError("argorderwithidaddarealimited")


def argorderarealimited(*args):
    raise NotImplementedError("argorderarealimited")


def argorderwithidarealimited(*args):
    raise NotImplementedError("argorderwithidarealimited")


def areaarea(areaclass):
    return lfr.zonal_area(areaclass)


def areaaverage(expression, areaclass):
    return lfr.zonal_mean(expression, areaclass)


def areadiversity(expression, areaclass):
    return lfr.zonal_diversity(expression, areaclass)


def areamajority(expression, areaclass):
    return lfr.zonal_majority(expression, areaclass)


def areamaximum(expression, areaclass):
    return lfr.zonal_maximum(expression, areaclass)


def areaminimum(expression, areaclass):
    return lfr.zonal_minimum(expression, areaclass)


def areanormal(areaclass):
    return lfr.zonal_normal(areaclass, np.float32)


def areaorder(expression, areaclass):
    raise NotImplementedError("areaorder")


def areatotal(expression, areaclass):
    return lfr.zonal_sum(expression, areaclass)


def areauniform(areaclass):
    return lfr.zonal_uniform(areaclass, np.float32)


def asin(expression):
    return lfr.asin(expression)


def aspect(dem):
    return lfr.aspect(dem)


def atan(expression):
    return lfr.atan(expression)


def boolean(expression):
    expression = read_if_necessary(expression)[0]

    if is_boolean(expression):
        return expression
    elif is_spatial(expression):
        # TODO where doesn't handle dypes correctly
        # https://github.com/computationalgeography/lue/issues/687
        # return lfr.where(expression != 0, np.uint8(1), np.uint8(0))
        return lfr.cast(lfr.where(expression != 0, np.uint8(1), np.uint8(0)), np.uint8)
    elif is_non_spatial(expression):
        return expression != 0
    elif lue_is_value(expression):
        return lfr.create_scalar(np.uint8, expression != 0)

    raise RuntimeError("Unsupported argument: {}".format(expression))


def catchment(*args):
    raise NotImplementedError("catchment")


def catchmenttotal(amount, flow_direction):
    # TODO This assumes accuthreshold can handle negative values
    # https://github.com/computationalgeography/lue/issues/673
    return accuthreshold(flow_direction, amount, 0)[1]


def cellarea(*args):
    return configuration.cell_size**2


def celllength(*args):
    return configuration.cell_size


def cellvalue(*args):
    raise NotImplementedError("cellvalue")


def clump(expression):
    return lfr.clump(expression, lfr.Connectivity.diagonal)


def cos(expression):
    return lfr.cos(expression)


def cover(expression1, expression2, *expressions):
    expression1, expression2 = harmonize_types(expression1, expression2)

    # TODO Should where support scalar condition?
    if is_non_spatial(expression1):
        expression1 = non_spatial_to_spatial(expression1)

    assert is_spatial(expression1), expression1

    result = lfr.where(lfr.valid(expression1), expression1, expression2)

    for expression in expressions:
        result = lfr.where(lfr.valid(result), result, expression)

    return result


def defined(expression):
    expression = read_if_necessary(expression)[0]

    return lfr.valid(expression)


def directional(expression):
    expression = read_if_necessary(expression)[0]

    if is_directional(expression):
        return expression
    elif is_spatial(expression) or is_non_spatial(expression):
        return lfr.cast(expression, np.float32)
    elif lue_is_value(expression):
        return lfr.create_scalar(np.float32, expression)

    raise RuntimeError("Unsupported argument: {}".format(expression))


def downstream(flow_direction, expression):
    flow_direction = read_if_necessary(flow_direction)[0]

    return lfr.downstream(flow_direction, expression)


def downstreamdist(flow_direction):
    flow_direction = read_if_necessary(flow_direction)[0]

    return lfr.downstream_distance(flow_direction, configuration.cell_size, np.float32)


def dynwavestate(*args):
    raise NotImplementedError("dynwavestate")


def dynwaveflux(*args):
    raise NotImplementedError("dynwaveflux")


def lookuppotential(*args):
    raise NotImplementedError("lookuppotential")


def lookupstate(*args):
    raise NotImplementedError("lookupstate")


def dynamicwave(*args):
    raise NotImplementedError("dynamicwave")


def eq(expression1, expression2):
    return expression1 == expression2


def exp(power):
    return lfr.exp(power)


def extentofview(*args):
    raise NotImplementedError("extentofview")


def fac(*args):
    raise NotImplementedError("fac")


def ge(expression1, expression2):
    return expression1 >= expression2


def gt(expression1, expression2):
    return expression1 > expression2


def horizontan(*args):
    raise NotImplementedError("horizontan")


def idiv(*args):
    raise NotImplementedError("idiv")


def ifthen(condition, expression):
    # TODO Should where support scalar condition?
    if not is_spatial(condition):
        if lue_is_value(condition):
            condition = lfr.create_scalar(np.uint8, condition)
        condition = non_spatial_to_spatial(condition)

    if lue_is_value(expression):
        expression = lfr.create_scalar(
            np.dtype(numpy_scalar_type(expression)), expression
        )

    return lfr.where(condition, expression)


def ifthenelse(condition, expression1, expression2):
    # TODO Should where support scalar condition?
    if not is_spatial(condition):
        if lue_is_value(condition):
            condition = lfr.create_scalar(np.uint8, condition)
        condition = non_spatial_to_spatial(condition)

    if lue_is_value(expression1):
        expression1 = lfr.create_scalar(
            np.dtype(numpy_scalar_type(expression1)), expression1
        )

    if lue_is_value(expression2):
        expression2 = lfr.create_scalar(
            np.dtype(numpy_scalar_type(expression2)), expression2
        )

    return lfr.where(condition, expression1, expression2)


def influencesimplegauss(*args):
    raise NotImplementedError("influencesimplegauss")


def inversedistance(*args):
    raise NotImplementedError("inversedistance")


def kinematic(
    flow_direction,
    current_outflow,
    inflow,
    alpha,
    beta,
    nr_time_slices,
    time_step_duration,
    channel_length,
):
    flow_direction = ldd(flow_direction)
    current_outflow = scalar(current_outflow)
    inflow = scalar(inflow)
    alpha = scalar(alpha)
    beta = scalar(beta)
    del nr_time_slices
    time_step_duration = scalar(time_step_duration)
    channel_length = scalar(channel_length)

    return lfr.kinematic_wave(
        flow_direction,
        current_outflow,
        inflow,
        alpha,
        beta,
        time_step_duration,
        channel_length,
    )


def kinwavestate(*args):
    raise NotImplementedError("kinwavestate")


def kinwaveflux(*args):
    raise NotImplementedError("kinwaveflux")


def ldd(expression):
    expression = read_if_necessary(expression)[0]

    if is_ldd(expression):
        return expression
    elif is_spatial(expression) or is_non_spatial(expression):
        return lfr.cast(expression, np.uint8)
    elif lue_is_value(expression):
        return lfr.create_scalar(np.uint8, expression)

    raise RuntimeError("Unsupported argument: {}".format(expression))


def lddcreate(elevation, outflowdepth, corevolume, corearea, catchmentprecipitation):
    elevation = read_if_necessary(elevation)[0]

    return lfr.d8_flow_direction(elevation)


def lddcreatedem(*args):
    raise NotImplementedError("lddcreatedem")


def ldddist(*args):
    raise NotImplementedError("ldddist")


def lddmask(*args):
    raise NotImplementedError("lddmask")


def lddrepair(*args):
    raise NotImplementedError("lddrepair")


def le(expression1, expression2):
    return expression1 <= expression2


def ln(expression):
    return lfr.log(expression)


def log10(expression):
    return lfr.log10(expression)


def lookup(*args):
    raise NotImplementedError("lookup")


def lookuplinear(*args):
    raise NotImplementedError("lookuplinear")


def lookupmapstack(*args):
    raise NotImplementedError("lookupmapstack")


def lt(expression1, expression2):
    return expression1 < expression2


def maparea(*args):
    raise NotImplementedError("maparea")


def mapmaximum(expression):
    return lfr.maximum(expression)


def mapminimum(expression):
    return lfr.minimum(expression)


def mapnormal():
    return lfr.normal(dtype=np.float32, mean=0, stddev=1)


def maptotal(expression):
    return lfr.sum(expression)


def mapuniform():
    return lfr.uniform(dtype=np.float32, min_value=0, max_value=1)


def markwhilesumle(*args):
    raise NotImplementedError("markwhilesumle")


def markwhilesumge(*args):
    raise NotImplementedError("markwhilesumge")


def max(*args):
    if len(args) == 1:
        return args[0]
    elif len(args) > 2:
        return max(args[0], max(*args[1:]))
    else:
        expression1, expression2 = harmonize_types(*args)

        # TODO Support non-spatial condition(?)
        condition = expression1 >= expression2

        if is_non_spatial(condition):
            condition = non_spatial_to_spatial(condition)

        return lfr.where(condition, expression1, expression2)


def min(*args):
    if len(args) == 1:
        return args[0]
    elif len(args) > 2:
        return min(args[0], min(*args[1:]))
    else:
        expression1, expression2 = harmonize_types(*args)

        # TODO Support non-spatial condition(?)
        condition = expression1 <= expression2

        if is_non_spatial(condition):
            condition = non_spatial_to_spatial(condition)

        return lfr.where(condition, expression1, expression2)


def mod(expression1, expression2):
    return expression1 % expression2


def ne(expression1, expression2):
    return expression1 != expression2


def nodirection(*args):
    raise NotImplementedError("nodirection")


def nominal(expression):
    expression = read_if_necessary(expression)[0]

    if is_nominal(expression):
        return expression
    elif is_spatial(expression) or is_non_spatial(expression):
        return lfr.cast(expression, np.int32)
    elif lue_is_value(expression):
        return lfr.create_scalar(np.int32, expression)

    raise RuntimeError("Unsupported argument: {}".format(expression))


def normal(expression):
    # TODO Support scalar input(?)
    if is_non_spatial(expression) or lue_is_value(expression):
        if lue_is_value(expression):
            expression = lfr.create_scalar(np.uint8, expression)

        expression = non_spatial_to_spatial(fill_value=expression)

    return lfr.where(expression, lfr.normal(expression, np.float32, 0, 1))


def pcrnot(expression):
    return ~expression


def pcror(expression1, expression2):
    return expression1 | expression2


def order(*args):
    raise NotImplementedError("order")


def ordinal(expression):
    expression = read_if_necessary(expression)[0]

    if is_ordinal(expression):
        return expression
    elif is_spatial(expression) or is_non_spatial(expression):
        return lfr.cast(expression, np.int32)
    elif lue_is_value(expression):
        return lfr.create_scalar(np.int32, expression)

    raise RuntimeError("Unsupported argument: {}".format(expression))


def path(*args):
    raise NotImplementedError("path")


def pit(*args):
    raise NotImplementedError("pit")


def plancurv(*args):
    raise NotImplementedError("plancurv")


def pred(*args):
    raise NotImplementedError("pred")


def profcurv(*args):
    raise NotImplementedError("profcurv")


def rounddown(expression):
    return lfr.floor(expression)


def roundoff(expression):
    return lfr.round(expression)


def roundup(expression):
    return lfr.ceil(expression)


def scalar(expression):
    expression = read_if_necessary(expression)[0]

    if is_scalar(expression):
        return expression
    elif is_spatial(expression) or is_non_spatial(expression):
        return lfr.cast(expression, np.float32)
    elif lue_is_value(expression):
        return lfr.create_scalar(np.float32, expression)

    raise RuntimeError("Unsupported argument: {}".format(expression))


def shift(*args):
    raise NotImplementedError("shift")


def shift0(*args):
    raise NotImplementedError("shift0")


def sin(expression):
    return lfr.sin(expression)


def slope(dem):
    return lfr.slope(dem, configuration.cell_size)


def slopelength(*args):
    raise NotImplementedError("slopelenght")


def spatial(expression):
    if not is_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=expression)

    return expression


def spread(*args):
    raise NotImplementedError("spread")


def spreadldd(*args):
    raise NotImplementedError("spreadldd")


def spreadlddzone(*args):
    raise NotImplementedError("spreadlddzone")


def spreadmax(*args):
    raise NotImplementedError("spreadmax")


def spreadmaxzone(*args):
    raise NotImplementedError("spreadmaxzone")


def spreadzone(*args):
    raise NotImplementedError("spreadzone")


def sqr(expression):
    return expression * expression


def sqrt(expression):
    return lfr.sqrt(expression)


def streamorder(*args):
    raise NotImplementedError("streamorder")


def subcatchment(*args):
    raise NotImplementedError("subcatchment")


def succ(*args):
    raise NotImplementedError("succ")


def tan(expression):
    return lfr.tan(expression)


def time(*args):
    raise NotImplementedError("time")


def timeinputboolean(*args):
    raise NotImplementedError("timeinputboolean")


def timeinputnominal(*args):
    raise NotImplementedError("timeinputnominal")


def timeinputordinal(*args):
    raise NotImplementedError("timeinputordinal")


def timeinputscalar(timeseries_pathname: str, id_expression, timestep: int):
    if timeseries_pathname in configuration.timeseries_files:
        lines = configuration.timeseries_files[timeseries_pathname]
    else:
        with open(timeseries_pathname) as tss:
            # Remove trailing characters
            lines = [line.rstrip() for line in tss]
            # Remove empty lines caused by ^M
            lines = [line for line in lines if line]
            nr_rows_to_skip = int(lines[1]) + 2
            lines = lines[nr_rows_to_skip:]
            configuration.timeseries_files[timeseries_pathname] = lines

    line = lines[timestep - 1]
    values = line.split()[1:]
    lookup_table = {id_ + 1: float(values[id_]) for id_ in range(len(values))}

    if isinstance(id_expression, int):
        return lookup_table[id_expression]
    else:
        return lfr.reclassify(id_expression, lookup_table, dtype=np.float32)


def timeinputdirectional(*args):
    raise NotImplementedError("timeinputdirectional")


def timeinputldd(*args):
    raise NotImplementedError("timeinputldd")


def timeinput(*args):
    raise NotImplementedError("timeinput")


def timeinputmodulo(*args):
    raise NotImplementedError("timeinputmodulo")


def timeinputsparse(*args):
    raise NotImplementedError("timeinputsparse")


def timeoutput(*args):
    raise NotImplementedError("timeoutput")


def timeslice(*args):
    raise NotImplementedError("timeslice")


def transient(*args):
    raise NotImplementedError("transient")


def uniform(expression):
    # TODO Support scalar input(?)
    if is_non_spatial(expression) or lue_is_value(expression):
        if lue_is_value(expression):
            expression = lfr.create_scalar(np.uint8, expression)

        expression = non_spatial_to_spatial(fill_value=expression)

    return lfr.where(expression, lfr.uniform(expression, np.float32, 0, 1))


def uniqueid(expression):
    # TODO Support scalar input(?)
    if is_non_spatial(expression) or lue_is_value(expression):
        if lue_is_value(expression):
            expression = lfr.create_scalar(np.uint8, expression)

        expression = non_spatial_to_spatial(fill_value=expression)

    return lfr.where(expression, lfr.unique_id(expression, dtype=np.int32), 0)


def upstream(flow_direction, material):
    flow_direction, material = read_if_necessary(flow_direction, material)

    if is_non_spatial(material) or lue_is_value(material):
        # TODO Support non-spatial material
        material = non_spatial_to_spatial(fill_value=np.float32(material))

    return lfr.upstream(flow_direction, material)


def view(*args):
    raise NotImplementedError("view")


def window4total(expression):
    # fmt:off
    window = np.array(
        [
            [ 0, 1, 0, ],
            [ 1, 1, 1, ],
            [ 0, 1, 0, ],
        ],
        dtype=np.uint8,
    )
    # fmt:on
    return lfr.focal_sum(expression, window)


def windowaverage(expression, window_length):
    window_length = translate_window_length(window_length)
    window = np.full((window_length, window_length), 1, dtype=np.uint8)

    return lfr.focal_mean(expression, window)


def windowdiversity(expression, window_length):
    window_length = translate_window_length(window_length)
    window = np.full((window_length, window_length), 1, dtype=np.uint8)

    return lfr.focal_diversity(expression, window)


def windowhighpass(expression, window_length):
    window_length = translate_window_length(window_length)
    window = np.full((window_length, window_length), 1, dtype=np.uint8)

    return lfr.focal_high_pass(expression, window)


def windowmajority(expression, window_length):
    window_length = translate_window_length(window_length)
    window = np.full((window_length, window_length), 1, dtype=np.uint8)

    return lfr.focal_majority(expression, window)


def windowmaximum(expression, window_length):
    window_length = translate_window_length(window_length)
    window = np.full((window_length, window_length), 1, dtype=np.uint8)

    return lfr.focal_maximum(expression, window)


def windowminimum(expression, window_length):
    window_length = translate_window_length(window_length)
    window = np.full((window_length, window_length), 1, dtype=np.uint8)

    return lfr.focal_minimum(expression, window)


def windowtotal(expression, window_length):
    window_length = translate_window_length(window_length)
    window = np.full((window_length, window_length), 1, dtype=np.uint8)

    return lfr.focal_sum(expression, window)


def xcoordinate(expression):
    # TODO Support non-spatial input?
    if is_non_spatial(expression) or lue_is_value(expression):
        expression = non_spatial_to_spatial(expression)

    return (
        configuration.bounding_box.west
        + 0.5 * configuration.cell_size
        + (
            lfr.cast(lfr.cell_index(defined(expression), 1), np.float32)
            * configuration.cell_size
        )
    )


def pcrxor(expression1, expression2):
    return expression1 ^ expression2


def ycoordinate(expression):
    # TODO Support non-spatial input?
    if is_non_spatial(expression) or lue_is_value(expression):
        expression = non_spatial_to_spatial(expression)

    return (
        configuration.bounding_box.south
        + 0.5 * configuration.cell_size
        + (
            (
                -lfr.cast(lfr.cell_index(defined(expression), 0), np.float32)
                + (configuration.array_shape[0] - 1)
            )
            * configuration.cell_size
        )
    )
