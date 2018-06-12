#include "lue/hdf5/identifier.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace hdf5 {

void init_identifier_class(
    py::module& module)
{

    py::class_<Identifier>(
        module,
        "Identifier",
        R"(
    Identifier docstring...
)")

        .def_property_readonly(
            "id",
            &Identifier::operator hid_t,
            R"(
    Return identifier of HDF5 object

    This identifier can be passed into other HDF5 APIs
)")

        .def_property_readonly(
            "name",
            &Identifier::name,
            R"(
    Return name of HDF5 object within the dataset
)")

        .def_property_readonly(
            "pathname",
            &Identifier::pathname,
            R"(
    Return pathname of HDF5 object within the dataset

    This name can be passed into other HDF5 APIs
)")

        ;

}

}  // namespace hdf5
}  // namespace lue
