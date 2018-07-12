#include "lue/object/identity/object_tracker.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_object_tracker(
    py::module& module)
{

    py::class_<ObjectTracker>(
        module,
        "ObjectTracker",
        R"(
    ObjectTracker docstring...
)")

        .def_property_readonly("id",
            py::overload_cast<>(&ObjectTracker::id),
            R"(
    id docstring...
)")

        ;

}

}  // namespace lue
