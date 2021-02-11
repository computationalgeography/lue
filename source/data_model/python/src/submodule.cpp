#include "hl/submodule.hpp"
#include "lue/py/data_model/submodule.hpp"
#include "python_extension.hpp"
#include "lue/configure.hpp"  // LUE_BUILD_TEST
#include "different_shape/submodule.hpp"
#include "same_shape/submodule.hpp"

#ifdef LUE_BUILD_TEST
#include "test/submodule.hpp"
#endif

#include "hdf5/submodule.hpp"
#include <hdf5.h>


namespace py = pybind11;


namespace lue {
namespace data_model {
namespace {

}  // Anonymous namespace


// void init_local_operations(pybind11::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "data_model",
        R"(
    :mod:`lue.data_model` --- LUE Scientific Database
    =================================================

    The :mod:`lue.data_model` package provides functionality for
    manipulating LUE datasets from Python. These are the entry points
    for creating new and opening existing datasets:

    ===================== =====================================
    Create new dataset    :func:`lue.data_model.create_dataset`
    Open existing dataset :func:`lue.data_model.open_dataset`
    ===================== =====================================

    .. automodule:: lue.data_model.same_shape
    .. automodule:: lue.data_model.different_shape
    .. automodule:: lue.data_model.dtype
    .. automodule:: lue.data_model.hdf5
)");

    // Prevent the dump of HDF5 error/warning messages. Our messages should
    // be clear enough.
#ifndef NDEBUG
    H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);
#endif

    // Order matters!

    hdf5::init_submodule(submodule);

    init_array(submodule);
    init_epoch(submodule);
    init_clock(submodule);
    /// // init_item(submodule);
    /// // init_domain(submodule);

    init_property_group(submodule);
    init_value_group(submodule);

    different_shape::init_submodule(submodule);
    same_shape::init_submodule(submodule);

    init_location_in_time(submodule);
    init_mobile_space_box(submodule);
    init_mobile_space_point(submodule);
    init_time_box(submodule);
    init_time_cell(submodule);
    init_time_point(submodule);
    init_stationary_space_box(submodule);
    init_stationary_space_point(submodule);

    init_active_object_id(submodule);
    init_active_object_index(submodule);
    init_active_set_index(submodule);
    init_object_id(submodule);
    init_object_tracker(submodule);

    init_property_set(submodule);
    init_space_domain(submodule);
    init_time_domain(submodule);

    init_validate(submodule);

    /// init_domain_class(submodule);
    /// init_time_domain(submodule);
    /// init_property_class(submodule);
    /// init_property_set_class(submodule);
    init_phenomenon(submodule);
    init_universe(submodule);
    init_dataset(submodule);

    /// constant_collection::init_submodule(submodule);

#ifdef LUE_BUILD_TEST
    test::init_submodule(submodule);
#endif

    bind_hl_submodule(submodule);

    // submodule.ptr();
}

}  // namespace data_model
}  // namespace lue
