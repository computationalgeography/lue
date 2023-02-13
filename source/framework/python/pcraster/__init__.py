"""
The code in this module makes it easier to port models that use the PCRaster Python package to
use the LUE Python package.
"""
import numpy as np
import lue.framework as lfr


class Configuration(object):
    def __init__(self, array_shape=None, partition_shape=None):
        self.array_shape = array_shape
        self.partition_shape = partition_shape


configuration = Configuration()


def is_spatial(argument):
    return isinstance(
        argument,
        (
            lfr.PartitionedArray_int32_2,
            lfr.PartitionedArray_int64_2,
            lfr.PartitionedArray_uint8_2,
            lfr.PartitionedArray_uint32_2,
            lfr.PartitionedArray_uint64_2,
            lfr.PartitionedArray_float32_2,
            lfr.PartitionedArray_float64_2,
        ),
    )


def is_non_spatial(argument):
    return isinstance(
        argument,
        (
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

    if template is None:
        array_shape = configuration.array_shape
    else:
        array_shape = template.shape

    partition_shape = configuration.partition_shape

    return lfr.create_array(
        array_shape, partition_shape, np.dtype(fill_value), fill_value
    )


def div(expression1, expression2):
    return expression1 / expression2


def abs(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=expression)

    return lfr.abs(expression)


def accucapacityflux(ldd, material, transportcapacity):
    raise NotImplementedError("accucapacityflux")


def accucapacitystate(ldd, material, transportcapacity):
    raise NotImplementedError("accucapacitystate")


def accuflux(ldd, material):
    raise NotImplementedError("accuflux")


def accufraction(ldd, material, transportcapacity):
    assert is_spatial(ldd), type(ldd)

    if is_non_spatial(material):
        material = non_spatial_to_spatial(fill_value=material, template=ldd)

    if is_non_spatial(transportcapacity):
        transportcapacity = non_spatial_to_spatial(
            fill_value=transportcapacity, template=ldd
        )

    return lfr.accu_fraction(ldd, material, transportcapacity)


def accufractionflux(ldd, material, transportcapacity):
    return accufraction(ldd, material, transportcapacity)[0]


def accufractionstate(ldd, material, transportcapacity):
    return accufraction(ldd, material, transportcapacity)[1]


def accuthreshold(ldd, material, threshold):
    assert is_spatial(ldd), type(ldd)

    if is_non_spatial(material):
        material = non_spatial_to_spatial(fill_value=material, template=ldd)

    if is_non_spatial(threshold):
        threshold = non_spatial_to_spatial(fill_value=threshold, template=ldd)

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
        expression = non_spatial_to_spatial(fill_value=expression)

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
        expression = non_spatial_to_spatial(fill_value=expression)

    return lfr.asin(expression)


def aspect(dem):
    return lfr.aspect(dem)


def atan(expression):
    if is_non_spatial(expression):
        expression = non_spatial_to_spatial(fill_value=expression)

    return lfr.atan(expression)


def boolean(expression):
    raise NotImplementedError("boolean")

    # TODO Result in an int32 array, not uint8 array
    # if is_non_spatial(expression):
    #     expression = non_spatial_to_spatial(fill_value=expression)

    # return lfr.where(expression != 0, np.uint8(1), np.uint8(0))


def catchment(*args):
    raise NotImplementedError("catchment")


def catchmenttotal(*args):
    raise NotImplementedError("catchmenttotal")


def cellarea(*args):
    raise NotImplementedError("cellarea")


def celllength(*args):
    raise NotImplementedError("celllength")


def clump(*args):
    raise NotImplementedError("clump")


# def cos(*args):


def cover(*args):
    raise NotImplementedError("cover")


# def defined(*args):


def directional(*args):
    raise NotImplementedError("directional")


# def downstream(*args):


# def downstreamdist(*args):


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


# def eq(*args):


# def exp(*args):


def extentofview(*args):
    raise NotImplementedError("extentofview")


def fac(*args):
    raise NotImplementedError("fac")


# def ge(*args):


# def gt(*args):


def horizontan(*args):
    raise NotImplementedError("horizontan")


# ? def idiv(*args):


# def if then(*args):


# def if then else(*args):


def influencesimplegauss(*args):
    raise NotImplementedError("influencesimplegauss")


def inversedistance(*args):
    raise NotImplementedError("inversedistance")


# def kinematic(*args):
# def kinwavestate, kinwaveflux(*args):


def ldd(*args):
    raise NotImplementedError("ldd")


# def lddcreate(*args):


def lddcreatedem(*args):
    raise NotImplementedError("lddcreatedem")


def ldddist(*args):
    raise NotImplementedError("lddcreatedist")


def lddmask(*args):
    raise NotImplementedError("lddmask")


def lddrepair(*args):
    raise NotImplementedError("lddrepair")


# def le or <=(*args):


# def ln(*args):


# def log10(*args):


def lookup(*args):
    raise NotImplementedError("lookup")


def lookuplinear(*args):
    raise NotImplementedError("lookuplinear")


def lookupmapstack(*args):
    raise NotImplementedError("lookupmapstack")


# def lt or <(*args):


def maparea(*args):
    raise NotImplementedError("maparea")


# def mapmaximum(*args):


# def mapminimum(*args):


def mapnormal(*args):
    raise NotImplementedError("mapnormal")


# def maptotal(*args):


def mapuniform(*args):
    raise NotImplementedError("mapuniform")


def markwhilesumle(*args):
    raise NotImplementedError("markwhilesumle")


def markwhilesumge(*args):
    raise NotImplementedError("markwhilesumge")


def max(*args):
    raise NotImplementedError("max")


def min(*args):
    raise NotImplementedError("min")


def mod(*args):
    raise NotImplementedError("mod")


# def ne or !=(*args):


def nodirection(*args):
    raise NotImplementedError("nodirection")


# def nominal(*args):


def normal(*args):
    raise NotImplementedError("normal")


# def not, ~, pcrnot(*args):


# def or, |, pcror(*args):


def order(*args):
    raise NotImplementedError("order")


# def ordinal(*args):


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


def rounddown(*args):
    raise NotImplementedError("rounddown")


def roundoff(*args):
    raise NotImplementedError("roundoff")


def roundup(*args):
    raise NotImplementedError("roundup")


# def scalar(*args):


def shift(*args):
    raise NotImplementedError("shift")


def shift0(*args):
    raise NotImplementedError("shift0")


# def sin(*args):


# def slope(*args):


def slopelength(*args):
    raise NotImplementedError("slopelenght")


# def spatial(*args):


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


# def sqr(*args):
# def sqrt(*args):
# def streamorder(*args):
# def subcatchment(*args):
# def succ(*args):
# def tan(*args):
# def time(*args):
# def timeinput…(*args):
# def timeinput(*args):
# def timeinputmodulo(*args):
# def timeinputsparse(*args):
# def timeoutput(*args):
# def timeslice(*args):
# def transient(*args):
# def uniform(*args):
# def uniqueid(*args):
# def upstream(*args):
# def view(*args):
# def window4total(*args):
# def windowaverage(*args):
# def windowdiversity(*args):
# def windowhighpass(*args):
# def windowmajority(*args):
# def windowmaximum(*args):
# def windowminimum(*args):
# def windowtotal(*args):
# def xcoordinate(*args):
# def xor, ^, pcrxor(*args):
# def ycoordinate(*args):


### def setclone(*args):
###     pcr.setclone(*args)
###
###
### def readmap(filename):
###     return lfr.from_gdal(filename, partition_shape)
###
###
### def report(array, filename):
###     lfr.to_gdal(array, filename)
###
###
###
###
### def nominal(arg):
###     if type(arg) == int:
###         return arg
###     elif type(arg) == str:
###         return readmap(arg)
###     else:
###         raise NotImplementedError(arg)
###
###
### def scalar(arg):
###     if type(arg) == lfr.PartitionedArray_float32_2:
###         return arg
###     elif type(arg) == int or type(arg) == float:
###         return float(arg)
###     elif type(arg) == str:
###         return readmap(arg)
###     else:
###         raise NotImplementedError(arg)
###
###
### def cover(arg1, arg2):
###     return lfr.where(lfr.valid(arg1), arg1, arg2)
###
###
### def load(
###         name):
###
###     assert name in ["lue", "pcraster"]
###
###     if name == "lue":
###         framework = lfr
###         framework.setclone = setclone
###         framework.boolean = boolean
###         framework.nominal = nominal
###         framework.scalar = scalar
###         framework.cover = cover
###         framework.readmap = readmap
###         framework.report = report
###
###     elif name == "pcraster":
###         framework = pcr
###
###     return framework
