#include "../python_extension.hpp"
#include "lue/info/identity/active_set_index.hpp"
#include <pybind11/pybind11.h>
#include <fmt/format.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace {

static std::string formal_string_representation(
    ActiveSetIndex const& index)
{
    return fmt::format(
            "ActiveSetIndex(pathname='{}')",
            index.id().pathname()
        );
}


static std::string informal_string_representation(
    ActiveSetIndex const& index)
{
    return fmt::format(
            "{}\n"
            "   nr_indices: {}",
            formal_string_representation(index),
            index.nr_indices()
        );
}

}  // Anonymous namespace


void init_active_set_index(
    py::module& module)
{

    py::class_<ActiveSetIndex, same_shape::constant_shape::Value>(
        module,
        "ActiveSetIndex",
        R"(
    A class for storing indices to information about active sets of
    objects

    For each location in time, :class:`ActiveSetIndex` instances store the
    index of information about the set of objects that are active. Each
    index points to the first active object in each set. The number of
    objects in each active set can be calculated by taking the difference
    of two adjacent indices.

    The indices point to locations in :class:`ActiveObjectID` or
    :class:`ActiveObjectIndex` instances.

    You never have to create an :class:`ActiveSetIndex` instance
    yourself. :class:`ObjectTracker` instances provide one.
)")

        .def(
            "__repr__",
            [](ActiveSetIndex const& index) {
                return formal_string_representation(index);
            }
        )

        .def(
            "__str__",
            [](ActiveSetIndex const& index) {
                return informal_string_representation(index);
            }
        )

        .def_property_readonly(
            "nr_indices",
            &ActiveSetIndex::nr_indices)

        ;

}

}  // namespace lue
