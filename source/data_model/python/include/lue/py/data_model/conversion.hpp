#pragma once
#include "lue/hdf5/datatype.hpp"
#include "lue/hdf5/shape.hpp"
#include <pybind11/numpy.h>
#include <tuple>


namespace lue {

    hdf5::Datatype numpy_type_to_memory_datatype(pybind11::dtype const& dtype);

    pybind11::dtype hdf5_type_id_to_numpy_dtype(hdf5::Datatype const datatype);

    hdf5::Shape tuple_to_shape(pybind11::tuple const& tuple);

}  // namespace lue
