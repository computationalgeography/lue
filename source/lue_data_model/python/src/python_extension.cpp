#include "lue/configure.hpp"
#include <hdf5.h>
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {

void init_array(py::module& module);
void init_clock(py::module& module);
void init_dataset(py::module& module);

void init_active_object_id(py::module& module);
void init_active_object_index(py::module& module);
void init_active_set_index(py::module& module);
void init_object_id(py::module& module);
void init_object_tracker(py::module& module);

void init_validate(py::module& module);
void init_phenomenon(py::module& module);
void init_property_group(py::module& module);
void init_property_set(py::module& module);
void init_space_domain(py::module& module);
void init_stationary_space_box(py::module& module);
void init_stationary_space_point(py::module& module);
void init_time_box(py::module& module);
void init_time_cell(py::module& module);
void init_time_domain(py::module& module);
void init_time_point(py::module& module);
void init_universe(py::module& module);
void init_value_group(py::module& module);


namespace hdf5 {

void init_submodule(py::module& module);

}  // namespace hdf5


namespace different_shape {

void init_submodule(py::module& module);

}  // namespace different_shape


namespace same_shape {

void init_submodule(py::module& module);

}  // namespace same_shape


#ifdef LUE_BUILD_TEST
namespace test {

void init_submodule(py::module& module);

}  // namespace test
#endif


PYBIND11_MODULE(lue, module)
{
    module.doc() =
        R"(
    :mod:`lue` --- Manipulating LUE from Python
    ===========================================

    The :mod:`lue` package provides functionality for manipulating LUE
    datasets from Python. These are the entry points for creating new
    and opening existing datasets:

    ===================== =========================
    Create new dataset    :func:`lue.create_dataset`
    Open existing dataset :func:`lue.open_dataset`
    ===================== =========================

    .. automodule:: lue.same_shape
    .. automodule:: lue.different_shape
    .. automodule:: lue.dtype
    .. automodule:: lue.hdf5
)";
    module.attr("__version__") = py::str(LUE_VERSION);


    // Prevent the dump of HDF5 error/warning messages. Our messages should
    // be clear enough.
#ifndef NDEBUG
    H5Eset_auto1(NULL, NULL);
#endif


    // Order matters!

    hdf5::init_submodule(module);

    init_array(module);
    init_clock(module);
    /// // init_item(module);
    /// // init_domain(module);

    init_property_group(module);
    init_value_group(module);

    different_shape::init_submodule(module);
    same_shape::init_submodule(module);

    init_time_box(module);
    init_time_cell(module);
    init_time_point(module);
    init_stationary_space_box(module);
    init_stationary_space_point(module);

    init_active_object_id(module);
    init_active_object_index(module);
    init_active_set_index(module);
    init_object_id(module);
    init_object_tracker(module);

    init_property_set(module);
    init_space_domain(module);
    init_time_domain(module);

    init_validate(module);

    /// init_domain_class(module);
    /// init_time_domain(module);
    /// init_property_class(module);
    /// init_property_set_class(module);
    init_phenomenon(module);
    init_universe(module);
    init_dataset(module);

    /// constant_collection::init_submodule(module);

#ifdef LUE_BUILD_TEST
    test::init_submodule(module);
#endif

    // module.ptr();
}

}  // namespace lue
