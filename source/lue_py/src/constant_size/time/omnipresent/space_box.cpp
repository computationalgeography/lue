#include "lue/constant_size/time/omnipresent/space_box.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

void init_space_box_class(
    py::module& module)
{
    py::class_<SpaceBox, same_shape::Value>(
        module,
        "SpaceBox",
        "SpaceBox...")

        // .def(
        //     "reserve",
        //     &lue::constant_size::time::omnipresent::SpaceBox::reserve,
        //     "reserve docstring...",
        //     py::return_value_policy::reference_internal)

        ;

}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
