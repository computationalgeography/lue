#include "lue/cxx_api/hdf5/group.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

void init_group(
        py::module& module)
{

    py::class_<hdf5::Group>(
        module,
        "Group",
        R"(
    Group docstring...
)")

        .def_property_readonly(
            "id",
            &hdf5::Group::id,
            R"(
    id docstring...
)",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace python
}  // namespace lue
