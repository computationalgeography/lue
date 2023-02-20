#include "lue/hdf5/primary_data_object.hpp"
#include "submodule.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue::hdf5 {

    void init_primary_data_object(py::module& module)
    {

        py::class_<PrimaryDataObject>(
            module,
            "PrimaryDataObject",
            R"(
    PrimaryDataObject docstring...
    )")

            .def_property_readonly(
                "id",
                // Select non-const overload
                py::overload_cast<>(&PrimaryDataObject::id),
                R"(
    Return identifier

    :rtype: lue.hdf5.Identifier
    )",
                py::return_value_policy::reference_internal)

            ;
    }

}  // namespace lue::hdf5
