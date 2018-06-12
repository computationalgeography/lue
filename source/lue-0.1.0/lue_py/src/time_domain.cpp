#include "lue/time_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_time_domain(
    py::module& module)
{

    py::class_<TimeDomain, hdf5::Group>(
        module,
        "TimeDomain",
        "TimeDomain docstring...")

        ;

}

}  // namespace lue
