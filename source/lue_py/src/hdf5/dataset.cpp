#include "lue/hdf5/dataset.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace hdf5 {

void init_dataset_class(
        py::module& module)
{

    py::class_<Dataset>(
        module,
        "Dataset",
        R"(
    Class representing an HDF5 dataset
)")

        .def_property_readonly(
            "shape",
            &Dataset::shape,
            "shape docstring...")

        ;

}

}  // namespace hdf5
}  // namespace lue
