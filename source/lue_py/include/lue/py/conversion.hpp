#pragma once
#include "lue/hdf5/datatype.hpp"
#include "lue/hdf5/shape.hpp"
#include <pybind11/pybind11.h>
#include <tuple>


namespace lue {

std::tuple<hid_t, hid_t>
                   numpy_type_to_hdf5_types(
                                        int const type_id);

hdf5::Datatype     numpy_type_to_memory_datatype(
                                        pybind11::handle const&
                                            numpy_type_id_object);

pybind11::object   hdf5_type_id_to_numpy_dtype(
                                        hdf5::Datatype const datatype);

hdf5::Shape        tuple_to_shape      (pybind11::tuple const& tuple);

}  // namespace lue
