#include "lue/cxx_api/constant_size/time/omnipresent/property.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {
namespace constant_size {
namespace time {
namespace omnipresent {

void init_property_class(
    py::module& module)
{

    py::class_<
        lue::constant_size::time::omnipresent::Property,
        lue::constant_size::Property>(
            module,
            "Property",
            "Property docstring...")
    ;

}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace python
}  // namespace lue
