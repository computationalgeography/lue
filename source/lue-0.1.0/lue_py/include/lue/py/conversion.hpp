#pragma once
#include "lue/hdf5/datatype.hpp"
#include <pybind11/pybind11.h>
#include <tuple>


namespace lue {

std::tuple<hid_t, hid_t>
                   numpy_type_to_hdf5_types(
                                        int const type_id);

pybind11::object   hdf5_type_id_to_numpy_dtype(
                                        hdf5::Datatype const datatype);

}  // namespace lue
