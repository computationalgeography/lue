#include "lue/constant_size/time/omnipresent/space_point.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

void init_space_point_class(
    py::module& module)
{
    py::class_<SpacePoint, constant_size::SameShape>(
        module,
        "SpacePoint",
        "SpacePoint...")

        ;

}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
