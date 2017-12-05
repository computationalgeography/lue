#include "lue/constant_size/time/omnipresent/space_box.hpp"
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
    py::class_<SpaceBox, constant::same_shape::Collection>(
        module,
        "SpaceBox",
        "SpaceBox...")

        ;

}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
