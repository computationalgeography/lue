#include "../python_extension.hpp"
#include "lue/object/identity/object_tracker.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_object_tracker(
    py::module& module)
{

    py::class_<ObjectTracker, hdf5::Group>(
        module,
        "ObjectTracker",
        R"(
    ObjectTracker docstring...
)")

        .def_property_readonly(
            "active_set_index",
            py::overload_cast<>(&ObjectTracker::active_set_index),
            R"(
    active_set_index docstring...
)")

        .def_property_readonly(
            "active_object_index",
            py::overload_cast<>(&ObjectTracker::active_object_index),
            R"(
    active_object_index docstring...
)")

        .def_property_readonly(
            "active_object_id",
            py::overload_cast<>(&ObjectTracker::active_object_id),
            R"(
    active_object_id docstring...
)")

        ;

}

}  // namespace lue
