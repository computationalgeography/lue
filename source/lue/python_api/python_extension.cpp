#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace python {

void               init_array          (py::module& module);
void               init_dataset        (py::module& module);
void               init_domain         (py::module& module);
void               init_file           (py::module& module);
void               init_group          (py::module& module);
void               init_identifier     (py::module& module);
void               init_item           (py::module& module);
void               init_phenomenon     (py::module& module);
void               init_property       (py::module& module);
void               init_property_set   (py::module& module);
void               init_universe       (py::module& module);


PYBIND11_PLUGIN(_lue)
{
    py::module module("_lue", R"(
Python extension implementing most of the LUE Python package

The classes are documented in alphabethical order. When creating or opening a LUE dataset, the order of the main classes that have to be used is more or less as follows:

- Dataset
- Universe
- Phenomenon
- PropertySet
- Domain
    - TimeDomain
    - SpaceDomain
- Property
    - Array
        - Shape
        - Dataset
)");

    // Order matters!
    init_array(module);
    init_identifier(module);
    init_file(module);
    init_group(module);
    init_item(module);
    init_dataset(module);
    init_domain(module);
    init_universe(module);
    init_phenomenon(module);
    init_property_set(module);
    init_property(module);

    return module.ptr();
}

}  // namespace python
}  // namespace lue
