#include "lue/space_domain.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_space_domain_class(
    py::module& module)
{

    py::class_<SpaceDomain, hdf5::Group>(
        module,
        "SpaceDomain",
        "SpaceDomain docstring...")

        ;

}

}  // namespace lue
