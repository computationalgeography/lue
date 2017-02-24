#include "lue/cxx_api/constant_size/time/omnipresent/space_box.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {
namespace constant_size {
namespace time {
namespace omnipresent {

void init_space_box_class(
    py::module& module)
{
    py::class_<
            lue::constant_size::time::omnipresent::SpaceBox,
            lue::constant_size::time::omnipresent::same_shape::Item>(
                module,
                "SpaceBox",
                "SpaceBox...")

        .def(
            "reserve_items",
            &lue::constant_size::time::omnipresent::SpaceBox::reserve_items,
            "reserve_items docstring...",
            py::return_value_policy::reference_internal)
    ;

}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace python
}  // namespace lue
