#include "python_extension.hpp"
#include "lue/configure.hpp"  // LUE_BUILD_TEST
#include "different_shape/submodule.hpp"
#include "same_shape/submodule.hpp"

#ifdef LUE_BUILD_TEST
#include "test/submodule.hpp"
#endif

#include "hdf5/submodule.hpp"
#include <hdf5.h>
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace data_model {

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
    module.attr("__version__") = py::str(BuildOptions::version);
    module.attr("lue_version") = py::str(BuildOptions::version);
    module.attr("git_short_sha1") = py::str(BuildOptions::git_short_sha1);

    // Prevent the dump of HDF5 error/warning messages. Our messages should
    // be clear enough.
#ifndef NDEBUG
    H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);
#endif

    // Order matters!

    hdf5::init_submodule(module);

    init_array(module);
    init_epoch(module);
    init_clock(module);
    /// // init_item(module);
    /// // init_domain(module);

    init_property_group(module);
    init_value_group(module);

    different_shape::init_submodule(module);
    same_shape::init_submodule(module);

    init_location_in_time(module);
    init_mobile_space_box(module);
    init_mobile_space_point(module);
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

}  // namespace data_model
}  // namespace lue
