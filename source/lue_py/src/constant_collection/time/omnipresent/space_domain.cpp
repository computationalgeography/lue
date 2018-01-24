#include "lue/constant_collection/time/omnipresent/space_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

void init_space_domain_class(
    py::module& module)
{

    py::class_<SpaceDomain, constant_collection::SpaceDomain>(
        module,
        "SpaceDomain",
        "SpaceDomain docstring...")

        ;

}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
