#pragma once
#include <hdf5.h>
#include <pybind11/pybind11.h>


namespace lue {
namespace python {

pybind11::object   hdf5_type_id_to_numpy_dtype(
                                        hid_t const type_id);

}  // namespace python
}  // namespace lue
