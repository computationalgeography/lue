#include "lue/hdf5/group.hpp"
#include "submodule.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue::hdf5 {

    void init_group(py::module& module)
    {

        py::class_<Group, PrimaryDataObject>(
            module,
            "Group",
            R"(
    Group docstring...
    )")

            ;
    }

}  // namespace lue::hdf5
