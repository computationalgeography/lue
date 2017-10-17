#include "lue/constant_size/space_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {

void init_space_domain_class(
    py::module& module)
{

    py::class_<SpaceDomain, lue::SpaceDomain>(
        module,
        "SpaceDomain",
        "SpaceDomain docstring...")

        ;

}

}  // namespace constant_size
}  // namespace lue
