#include "lue/domain.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_domain_class(
    py::module& module)
{

    py::class_<Domain, hdf5::Group>(
        module,
        "Domain",
        "Domain docstring...")

        ;

}

}  // namespace lue
