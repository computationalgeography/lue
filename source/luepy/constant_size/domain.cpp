#include "lue/constant_size/domain.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {

void init_domain_class(
    py::module& module)
{

    py::class_<Domain, lue::Domain>(
        module,
        "Domain",
        "Domain docstring...")

        ;

}

}  // namespace constant_size
}  // namespace lue
