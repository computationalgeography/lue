#include "../python_extension.hpp"
#include "lue/info/identity/active_object_index.hpp"
#include <pybind11/pybind11.h>
#include <fmt/format.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace {

static std::string formal_string_representation(
    ActiveObjectIndex const& index)
{
    return fmt::format(
            "ActiveObjectIndex(pathname='{}')",
            index.id().pathname()
        );
}


static std::string informal_string_representation(
    ActiveObjectIndex const& index)
{
    return formal_string_representation(index);
}

}  // Anonymous namespace


void init_active_object_index(
    py::module& module)
{

    py::class_<ActiveObjectIndex, same_shape::constant_shape::Value>(
        module,
        "ActiveObjectIndex",
        R"(
    A class for storing the indices of object arrays in the value array
    of active objects

    For each location in time, :class:`ActiveObjectIndex` instances store the
    indices of the object arrays in the value array of the set of objects
    that are active. They are stored one set after the other. The index
    of the first object array index of each active set is stored in an
    :class:`ActiveSetIndex` instance.

    Use this class when, for each object, object arrays are stored in
    a seperate value array. This is the case when storing object arrays
    whose shape is different per object, but constant through time
    (:class:`different_shape.constant_shape.Value`).

    You never have to create an :class:`ActiveObjectIndex` instance
    yourself. :class:`ObjectTracker` instances provide one.
)")

        .def(
            "__repr__",
            [](ActiveObjectIndex const& index) {
                return formal_string_representation(index);
            }
        )

        .def(
            "__str__",
            [](ActiveObjectIndex const& index) {
                return informal_string_representation(index);
            }
        )

        ;

}

}  // namespace lue
