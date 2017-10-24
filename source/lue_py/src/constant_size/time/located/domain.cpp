#include "lue/constant_size/time/located/domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace located {

void init_domain(
    py::module& module)
{

    py::class_<Domain, constant_size::Domain>(
        module,
        "Domain",
        "Domain docstring...")

        ;

}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
