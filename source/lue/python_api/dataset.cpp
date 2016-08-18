#include "lue/cxx_api/dataset.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

void init_dataset(
        py::module& module)
{

    py::class_<Dataset>(module, "Dataset", py::base<hdf5::File>(),
        "Dataset class"
        "\n"
        "A LUE dataset can contain collections of universes,\n"
        "phenomena, or property sets")
        .def("add_phenomenon", &Dataset::add_phenomenon,
            "add_phenomenon docstring...",
            py::return_value_policy::reference_internal)
        .def_property_readonly("phenomena", &Dataset::phenomena,
            "phenomena docstring...",
            py::return_value_policy::reference_internal)
        .def("add_universe", &Dataset::add_universe,
            "add_universe docstring...",
            py::return_value_policy::reference_internal)
        .def_property_readonly("universes", &Dataset::universes,
            "universes docstring...",
            py::return_value_policy::reference_internal)
    ;


    module.def("open_dataset", &open_dataset,
        "name"_a,
        "Open existing LUE dataset\n",
        py::return_value_policy::move);
    module.def("create_dataset", &create_dataset,
        "name"_a,
        "Create new LUE dataset\n"
        "\n"
        "If the dataset already exists it is overwritten",
        py::return_value_policy::move);

}

}  // namespace python
}  // namespace lue
