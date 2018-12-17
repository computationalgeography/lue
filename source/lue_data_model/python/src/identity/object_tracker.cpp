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
    A class for storing IDs of objects in active sets and indices of
    information related to those objects and stored elsewhere

    In the data model we can store object-related information that
    changes through time. This information is stored as compact as
    possible in value arrays. For each location in time, an object
    tracker contains information about which objects were active and
    where information about these objects is stored in the value arrays.
)")

        .def_property_readonly(
            "active_set_index",
            py::overload_cast<>(&ObjectTracker::active_set_index),
            R"(
    Return active set indices

    :rtype: ActiveSetIndex
)")

        .def_property_readonly(
            "active_object_index",
            py::overload_cast<>(&ObjectTracker::active_object_index),
            R"(
    Return active object indices

    :rtype: ActiveObjectIndex
)")

        .def_property_readonly(
            "active_object_id",
            py::overload_cast<>(&ObjectTracker::active_object_id),
            R"(
    Return active object IDs

    :rtype: ActiveObjectID
)")

        ;

}

}  // namespace lue
