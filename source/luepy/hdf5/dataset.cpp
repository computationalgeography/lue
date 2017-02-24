#include "lue/hdf5/dataset.h"
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

        ;

}

}  // namespace hdf5
}  // namespace lue
