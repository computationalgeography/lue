#include "../python_extension.hpp"
#include "lue/info/identity/active_object_id.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_active_object_id(
    py::module& module)
{

    py::class_<ActiveObjectID, same_shape::constant_shape::Value>(
        module,
        "ActiveObjectID",
        R"(
    A class for storing the IDs of active objects

    For each location in time, :class:`ActiveObjectID` instances store
    the set of IDs of the objects that are active. They are stored one
    set after the other. The index of the first object ID of each active
    set is stored in an :class:`ActiveSetIndex` instance.

    Use this class when object arrays of different objects
    are stored in the same value array. This is the case
    when storing object arrays whose shape iѕ the same per
    object (:class:`same_shape.constant_shape.Value` and
    :class:`same_shape.variable_shape.Value`).

    You never have to create an :class:`ActiveObjectID` instance
    yourself. :class:`ObjectTracker` instances provide one.
)")

        .def_property_readonly(
            "nr_ids",
            &ActiveObjectID::nr_ids,
            R"(
    Return the total number of object IDs stored
)")

        ;

}

}  // namespace lue
