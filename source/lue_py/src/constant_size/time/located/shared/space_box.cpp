#include "lue/constant_size/time/located/shared/space_box.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

void init_space_box_class(
    py::module& module)
{
    py::class_<SpaceBox, constant_shape::same_shape::Value>(
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

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
