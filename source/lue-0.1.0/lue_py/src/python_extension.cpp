#include "lue/configure.hpp"
#include <hdf5.h>
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {

void init_array_class(py::module& module);
void init_clock(py::module& module);
void init_dataset_class(py::module& module);
void init_domain_class(py::module& module);
void init_validate(py::module& module);
// void init_item(py::module& module);
void init_phenomenon_class(py::module& module);
void init_property_class(py::module& module);
void init_property_set_class(py::module& module);
void init_space_domain_class(py::module& module);
void init_time_domain(py::module& module);
void init_universe_class(py::module& module);

namespace constant_collection {

void init_submodule(py::module& module);

}  // namespace constant_collection


namespace hdf5 {

void init_submodule(py::module& module);

}  // namespace hdf5


PYBIND11_MODULE(lue, module)
{
    module.doc() =
        R"(
    Python extension implementing most of the LUE Python package

    The classes are documented in alphabethical order. When creating or opening
    a LUE dataset, the order of the main classes that have to be used is more
    or less as follows:

    - :py:class:`Dataset`
    - :py:class:`Universe`
    - :py:class:`Phenomenon`
    - :py:class:`PropertySet`
    - :py:class:`Domain`
        - :py:class:`TimeDomain`
        - :py:class:`SpaceDomain`
    - :py:class:`Property`
        - :py:class:`Array`

    .. automodule:: lue.hdf5

    .. automodule:: lue.constant_collection
)";
    module.attr("__version__") = py::str(LUE_VERSION);


    // Prevent the dump of HDF5 error/warning messages. Our messages should
    // be clear enough.
#ifndef NDEBUG
    H5Eset_auto1(NULL, NULL);
#endif


    // Order matters!

    hdf5::init_submodule(module);

    init_array_class(module);
    init_clock(module);
    // init_item(module);
    // init_domain(module);

    init_validate(module);

    init_domain_class(module);
    init_space_domain_class(module);
    init_time_domain(module);
    init_property_class(module);
    init_property_set_class(module);
    init_phenomenon_class(module);
    init_universe_class(module);
    init_dataset_class(module);

    constant_collection::init_submodule(module);


    module.ptr();
}

}  // namespace lue