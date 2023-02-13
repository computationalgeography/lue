"""
The code in this module makes it easier to port models that use the PCRaster Python package to
use the LUE Python package.
"""
import numpy as np
import lue.framework as lfr


class Configuration(object):
    def __init__(self, partition_shape=None):
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


def non_spatial_to_spatial(template, fill_value):
    return lfr.create_array(
        template.shape, configuration.partition_shape, np.dtype(fill_value), fill_value
    )


def div(expression1, expression2):
    return expression1 / expression2


def abs(expression):
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
        material = non_spatial_to_spatial(template=ldd, fill_value=material)

    if is_non_spatial(transportcapacity):
        transportcapacity = non_spatial_to_spatial(
            template=ldd, fill_value=transportcapacity
        )

    return lfr.accu_fraction(ldd, material, transportcapacity)


def accufractionflux(ldd, material, transportcapacity):
    return accufraction(ldd, material, transportcapacity)[0]


def accufractionstate(ldd, material, transportcapacity):
    return accufraction(ldd, material, transportcapacity)[1]


def accuthreshold(ldd, material, threshold):
    assert is_spatial(ldd), type(ldd)

    if is_non_spatial(material):
        material = non_spatial_to_spatial(template=ldd, fill_value=material)

    if is_non_spatial(threshold):
        threshold = non_spatial_to_spatial(template=ldd, fill_value=threshold)

    return lfr.accu_threshold3(ldd, material, threshold)


def accuthresholdflux(ldd, material, threshold):
    return accuthreshold(ldd, material, threshold)[0]


def accuthresholdstate(ldd, material, threshold):
    return accuthreshold(ldd, material, threshold)[1]


# Hier verder
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
