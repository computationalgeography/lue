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

LUE itself supports additional cell represenations (uint32, int64, uint64, float64).

LUE does not have the notion of non-spatial. This is relevant when passing two "non-spatial"
arguments to an operator (e.g.: ``a & b``). The regular Python rules are then in effect.
"""
import numpy as np

import lue.framework as lfr


class Configuration(object):
    """
    Class for storing information that is required for LUE to be able to mimic PCRaster

    PCRaster makes use of a clone raster. This raster contains information about the shape of
    the underlying array and the size of the raster cells. This class fulfills the same role.
    """

    def __init__(self, array_shape=None, partition_shape=None, cell_size=None):
        self.array_shape = array_shape
        self.partition_shape = partition_shape
        self.cell_size = cell_size


"""
Global variable that contains information required by some of the LUE operations used in the
wrappers
"""
configuration = Configuration()


def setclone(*args):
    raise NotImplementedError(
        "Please assign to this module's configuration variable. E.g.:\n"
        "lue.pcraster.configuration.array_shape = array_shape\n"
        "lue.pcraster.configuration.partition_shape = partition_shape\n"
        "lue.pcraster.configuration.cell_size = 10"
    )


def numpy_scalar_type(expression):
    element_type_by_type = {
        getattr(lfr, "PartitionedArray<uint8, 2>"): np.uint8,
        getattr(lfr, "PartitionedArray<int32, 2>"): np.int32,
        getattr(lfr, "PartitionedArray<float32, 2>"): np.float32,
        int: np.int32,
        float: np.float32,
        np.uint8: np.uint8,
        np.int32: np.int32,
        np.float32: np.float32,
        np.float64: np.float32,  # Yes, downcasting to PCRaster's cell representation
    }

    return element_type_by_type[type(expression)]


def is_spatial(argument):
    return isinstance(
        argument,
        (
            getattr(lfr, "PartitionedArray<uint8, 2>"),  # Boolean, LDD
            getattr(lfr, "PartitionedArray<int32, 2>"),  # Nominal, ordinal
            getattr(lfr, "PartitionedArray<float32, 2>"),  # Scalar, directional
        ),
    )


def is_boolean(expression):
    return isinstance(expression, (getattr(lfr, "PartitionedArray<uint8, 2>")))


def is_ldd(expression):
    return isinstance(expression, (getattr(lfr, "PartitionedArray<uint8, 2>")))


def is_nominal(expression):
    return isinstance(expression, (getattr(lfr, "PartitionedArray<int32, 2>")))


def is_ordinal(expression):
    return isinstance(expression, (getattr(lfr, "PartitionedArray<int32, 2>")))


def is_scalar(expression):
    return isinstance(expression, (getattr(lfr, "PartitionedArray<float32, 2>")))


def is_directional(expression):
    return isinstance(expression, (getattr(lfr, "PartitionedArray<float32, 2>")))


def is_non_spatial(argument):
    return isinstance(
        argument,
        (
            int,
            float,
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
    scalar_type = numpy_scalar_type(fill_value)

    if template is None:
        array_shape = configuration.array_shape
    else:
        array_shape = template.shape

    partition_shape = configuration.partition_shape

    return lfr.create_array(
        array_shape, scalar_type, fill_value, partition_shape=partition_shape
    )


def read_if_necessary(*args) -> tuple:
    return tuple(map(lambda arg: readmap(arg) if isinstance(arg, str) else arg, args))


def translate_window_length(pcraster_window_length):
    """
    Translate PCRaster window lengths in real distance units, to LUE window lengths, in whole
    number of cells

    The assumption here is that @a pcraster_window_length passed in:
    - is dividable by the cell size
    - corresponds with an odd number of cells (odd_number * cell_size)
    - is possitive
    """
    assert is_non_spatial(pcraster_window_length), pcraster_window_length

    window_length = pcraster_window_length / configuration.cell_size

    assert window_length % 1 == 0, window_length  # Must not be fractional
    assert window_length % 2 == 1, window_length  # Must be odd
    assert window_length > 0, window_length  # Must be positive

    window_length = int(window_length)

    return window_length


def readmap(pathname):
    return lfr.from_gdal(pathname, partition_shape=configuration.partition_shape)


def report(expression, pathname):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=expression)

    lfr.to_gdal(expression, pathname)


def div(expression1, expression2):
    return expression1 / expression2


def abs(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return lfr.abs(expression)


def accucapacityflux(ldd, material, transportcapacity):
    raise NotImplementedError("accucapacityflux")


def accucapacitystate(ldd, material, transportcapacity):
    raise NotImplementedError("accucapacitystate")


def accuflux(ldd, material):
    raise NotImplementedError("accuflux")


def accufraction(ldd, material, transportcapacity):
    ldd, material, transportcapacity = read_if_necessary(
        ldd, material, transportcapacity
    )

    assert is_spatial(ldd), type(ldd)

    if is_non_spatial(material):
        material = non_spatial_to_spatial(fill_value=np.float32(material), template=ldd)

    if is_non_spatial(transportcapacity):
        transportcapacity = non_spatial_to_spatial(
            fill_value=np.float32(transportcapacity), template=ldd
        )

    return lfr.accu_fraction(ldd, material, transportcapacity)


def accufractionflux(ldd, material, transportcapacity):
    return accufraction(ldd, material, transportcapacity)[0]


def accufractionstate(ldd, material, transportcapacity):
    return accufraction(ldd, material, transportcapacity)[1]


def accuthreshold(ldd, material, threshold):
    if isinstance(ldd, str):
        ldd = readmap(ldd)

    assert is_spatial(ldd), type(ldd)

    if is_non_spatial(material):
        material = non_spatial_to_spatial(fill_value=np.float32(material), template=ldd)

    if is_non_spatial(threshold):
        threshold = non_spatial_to_spatial(
            fill_value=np.float32(threshold), template=ldd
        )

    return lfr.accu_threshold3(ldd, material, threshold)


def accuthresholdflux(ldd, material, threshold):
    return accuthreshold(ldd, material, threshold)[0]


def accuthresholdstate(ldd, material, threshold):
    return accuthreshold(ldd, material, threshold)[1]


def accutriggerflux(ldd, material, transporttrigger):
    raise NotImplementedError("accutriggerflux")


def accutriggerstate(ldd, material, transporttrigger):
    raise NotImplementedError("accutriggerstate")


def accutraveltimeflux(ldd, material, transporttraveltime):
    raise NotImplementedError("accutraveltimeflux")


def accutraveltimestate(ldd, material, transporttraveltime):
    raise NotImplementedError("accutraveltimestate")


def accutraveltimefractionflux(ldd, material, transporttraveltime):
    raise NotImplementedError("accutraveltimefractionflux")


def accutraveltimefractionstate(ldd, material, transporttraveltime):
    raise NotImplementedError("accutraveltimefractionstate")


def acos(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

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
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return lfr.asin(expression)


def aspect(dem):
    return lfr.aspect(dem)


def atan(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return lfr.atan(expression)


def boolean(expression):
    expression = read_if_necessary(expression)[0]

    if is_spatial(expression):
        if is_boolean(expression):
            return expression
        else:
            return lfr.where(expression != 0, np.uint8(1), np.uint8(0))
    elif is_non_spatial(expression):
        return np.uint8(expression)

    raise RuntimeError("Unsupported argument: {}".format(expression))


def catchment(*args):
    raise NotImplementedError("catchment")


def catchmenttotal(*args):
    raise NotImplementedError("catchmenttotal")


def cellarea(*args):
    return configuration.cell_size**2


def celllength(*args):
    return configuration.cell_size


def clump(expression):
    return lfr.clump(expression, lfr.Connectivity.diagonal)


def cos(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return lfr.cos(expression)


def cover(expression1, expression2, *expressions):
    if not is_spatial(expression1):
        expression1 = non_spatial_to_spatial(fill_value=expression1)

    result = lfr.where(lfr.valid(expression1), expression1, expression2)

    for expression in expressions:
        result = lfr.where(lfr.valid(result), result, expression)

    return result


def defined(expression):
    if isinstance(expression, str):
        expression = readmap(expression)

    if not is_spatial(expression):
        return non_spatial_to_spatial(fill_value=np.uint8(1))

    return lfr.valid(expression)


def directional(expression):
    expression = read_if_necessary(expression)[0]

    if is_spatial(expression):
        if is_directional(expression):
            return expression
        else:
            return lfr.cast(expression, np.float32)
    elif is_non_spatial(expression):
        return np.float32(expression)

    raise RuntimeError("Unsupported argument: {}".format(expression))


def downstream(ldd, expression):
    if isinstance(ldd, str):
        ldd = readmap(ldd)

    return lfr.downstream(ldd, expression)


def downstreamdist(ldd):
    if isinstance(ldd, str):
        ldd = readmap(ldd)

    return lfr.downstream_distance(ldd, configuration.cell_size, np.float32)


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
    if is_non_spatial(power):
        power = non_spatial_to_spatial(fill_value=np.float32(power))

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
    if is_non_spatial(condition):
        assert condition < np.iinfo(np.uint8).max, condition
        condition = non_spatial_to_spatial(fill_value=np.uint8(condition))

    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=expression)

    return lfr.where(condition, expression)


def ifthenelse(condition, expression1, expression2):
    if is_non_spatial(condition):
        assert condition < np.iinfo(np.uint8).max, condition
        condition = non_spatial_to_spatial(fill_value=np.uint8(condition))

    if is_spatial(expression1):
        expression_dtype = expression1.dtype
    elif is_spatial(expression2):
        expression_dtype = expression2.dtype
    else:
        expression_dtype = np.dtype(numpy_scalar_type(expression1))

    if is_non_spatial(expression1):
        expression1 = non_spatial_to_spatial(
            fill_value=expression_dtype.type(expression1)
        )

    if is_non_spatial(expression2):
        expression2 = non_spatial_to_spatial(
            fill_value=expression_dtype.type(expression2)
        )

    return lfr.where(condition, expression1, expression2)


def influencesimplegauss(*args):
    raise NotImplementedError("influencesimplegauss")


def inversedistance(*args):
    raise NotImplementedError("inversedistance")


def kinematic(ldd, Qold, q, alpha, beta, nrTimeSlices, dT, dX):
    return lfr.kinematic_wave(ldd, Qold, q, alpha, beta, dT, dX)


def kinwavestate(*args):
    raise NotImplementedError("kinwavestate")


def kinwaveflux(*args):
    raise NotImplementedError("kinwaveflux")


def ldd(expression):
    expression = read_if_necessary(expression)[0]

    if is_spatial(expression):
        if is_ldd(expression):
            return expression
        else:
            return lfr.cast(expression, np.uint8)
    elif is_non_spatial(expression):
        return np.uint8(expression)

    raise RuntimeError("Unsupported argument: {}".format(expression))


def lddcreate(elevation, outflowdepth, corevolume, corearea, catchmentprecipitation):
    return lfr.d8_flow_direction(elevation)


def lddcreatedem(*args):
    raise NotImplementedError("lddcreatedem")


def ldddist(*args):
    raise NotImplementedError("lddcreatedist")


def lddmask(*args):
    raise NotImplementedError("lddmask")


def lddrepair(*args):
    raise NotImplementedError("lddrepair")


def le(expression1, expression2):
    return expression1 <= expression2


def ln(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return lfr.log(expression)


def log10(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

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
    return normal(1)


def maptotal(expression):
    return lfr.sum(expression)


def mapuniform():
    return uniform(1)


def markwhilesumle(*args):
    raise NotImplementedError("markwhilesumle")


def markwhilesumge(*args):
    raise NotImplementedError("markwhilesumge")


def max(*args):
    if len(args) == 1:
        return args[0]

    if len(args) != 2:
        raise NotImplementedError("max with more than 2 arguments")

    expression1, expression2 = args
    type1 = numpy_scalar_type(expression1)
    type2 = numpy_scalar_type(expression2)

    if is_non_spatial(expression1):
        expression1 = non_spatial_to_spatial(fill_value=type2(expression1))

    if is_non_spatial(expression2):
        expression2 = non_spatial_to_spatial(fill_value=type1(expression2))

    condition = lfr.greater_than_equal_to(expression1, expression2)

    return lfr.where(condition, expression1, expression2)


def min(*args):
    if len(args) == 1:
        return args[0]

    if len(args) != 2:
        raise NotImplementedError("min with more than 2 arguments")

    expression1, expression2 = args
    type1 = numpy_scalar_type(expression1)
    type2 = numpy_scalar_type(expression2)

    if is_non_spatial(expression1):
        expression1 = non_spatial_to_spatial(fill_value=type2(expression1))

    if is_non_spatial(expression2):
        expression2 = non_spatial_to_spatial(fill_value=type1(expression2))

    condition = lfr.less_than_equal_to(expression1, expression2)

    return lfr.where(condition, expression1, expression2)


def mod(*args):
    raise NotImplementedError("mod")


def ne(expression1, expression2):
    return expression1 != expression2


def nodirection(*args):
    raise NotImplementedError("nodirection")


def nominal(expression):
    expression = read_if_necessary(expression)[0]

    if is_spatial(expression):
        if is_nominal(expression):
            return expression
        else:
            return lfr.cast(expression, np.int32)
    elif is_non_spatial(expression):
        return np.int32(expression)

    raise RuntimeError("Unsupported argument: {}".format(expression))


def normal(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.uint8(expression))

    return lfr.where(expression, lfr.normal(expression, np.float32, 0, 1))


def pcrnot(expression):
    return ~expression


def pcror(expression1, expression2):
    return expression1 | expression2


def order(*args):
    raise NotImplementedError("order")


def ordinal(expression):
    expression = read_if_necessary(expression)[0]

    if is_spatial(expression):
        if is_ordinal(expression):
            return expression
        else:
            return lfr.cast(expression, np.int32)
    elif is_non_spatial(expression):
        return np.int32(expression)

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
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return lfr.floor(expression)


def roundoff(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return lfr.round(expression)


def roundup(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return lfr.ceil(expression)


def scalar(expression):
    expression = read_if_necessary(expression)[0]

    if is_spatial(expression):
        if is_scalar(expression):
            return expression
        else:
            return lfr.cast(expression, np.float32)
    elif is_non_spatial(expression):
        return np.float32(expression)

    raise RuntimeError("Unsupported argument: {}".format(expression))


def shift(*args):
    raise NotImplementedError("shift")


def shift0(*args):
    raise NotImplementedError("shift0")


def sin(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

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
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return expression * expression


def sqrt(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return lfr.log(expression)


def streamorder(*args):
    raise NotImplementedError("streamorder")


def subcatchment(*args):
    raise NotImplementedError("subcatchment")


def succ(*args):
    raise NotImplementedError("succ")


def tan(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.float32(expression))

    return lfr.tan(expression)


def time(*args):
    raise NotImplementedError("time")


def timeinputboolean(*args):
    raise NotImplementedError("timeinputboolean")


def timeinputnominal(*args):
    raise NotImplementedError("timeinputnominal")


def timeinputordinal(*args):
    raise NotImplementedError("timeinputordinal")


def timeinputscalar(*args):
    raise NotImplementedError("timeinputscalar")


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
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.uint8(expression))

    return lfr.where(expression, lfr.uniform(expression, np.float32, 0, 1))


def uniqueid(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=np.uint8(expression))

    return lfr.where(expression, lfr.unique_id(expression, dtype=np.int32), 0)


def upstream(ldd, material):
    ldd, material = read_if_necessary(ldd, material)

    if is_non_spatial(material):
        material = non_spatial_to_spatial(fill_value=np.float32(material))

    return lfr.upstream(ldd, material)


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


def xcoordinate(*args):
    raise NotImplementedError("xcoordinate")


def pcrxor(expression1, expression2):
    return expression1 ^ expression2


def ycoordinate(*args):
    raise NotImplementedError("ycoordinate")
