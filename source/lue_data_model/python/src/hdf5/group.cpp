#include "lue/hdf5/group.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace hdf5 {

void init_group(
    py::module& module)
{

    py::class_<Group>(
        module,
        "Group",
        R"(
    Group docstring...
)")

        .def_property_readonly(
            "id",
            // Select non-const overload
            py::overload_cast<>(&Group::id),
            R"(
    Return identifier

    :rtype: lue.hdf5.Identifier
)",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace hdf5
}  // namespace lue
