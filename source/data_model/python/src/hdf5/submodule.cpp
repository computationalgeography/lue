#include "submodule.hpp"
#include "lue/hdf5/configure.hpp"


namespace py = pybind11;


namespace lue {
namespace hdf5 {

void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "hdf5",
        R"(
    :mod:`lue.data_model.hdf5` --- Manipulating HDF5 from Python
    ============================================================

    The :mod:`lue.data_model.hdf5` package wraps the LUE C++ API that,
    in turn, is implemented on top of the HDF5 C API. In general you don't
    need to use this package. It exists because it is a side effect of
    implementing the rest of the :mod:`lue.data_model` package. It may
    be useful in case you don't need the functionality offered by the
    :mod:`lue.data_model` package, or if you need functionality that is
    not offered by this package yet.
)");

    submodule.attr("__version__") = py::str(BuildOptions::hdf5_version);
    submodule.attr("hdf5_version") = py::str(BuildOptions::hdf5_version);

    init_shape(submodule);
    init_identifier(submodule);
    init_group(submodule);
    init_file(submodule);
    init_dataset(submodule);
    init_validate(submodule);
}

}  // namespace hdf5
}  // namespace lue
