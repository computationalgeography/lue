#include "lue/dataset.hpp"
#include "lue/py/hdf5/file.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_dataset_class(
        py::module& module)
{

    py::class_<Dataset, hdf5::File>(
        module,
        "Dataset",
        R"(
    LUE dataset representing a scientific database

    A LUE dataset can contain collections of universes, phenomena, and/or
    property sets.

    It is not possible to create Dataset instances from scratch:
    a constructor is not defined. New datasets can be created with the
    free function create_dataset(). Existing datasets can be opened
    with open_dataset().

    A LUE dataset is not similar to an HDF5 dataset. An HDF5 dataset
    represents a multidimensional array in an HDF5 file. The HDF5 file
    is managed by the LUE dataset.
)")

        .def(
            "__repr__",
            [](Dataset const& dataset) {
                // TODO Access flag to string.
                return "Dataset("
                    "name='" + dataset.pathname() + "', " +
                    "flags='" + "TODO" + "')";
            }
        )

        .def(
            "add_phenomenon",
            &Dataset::add_phenomenon,
            R"(
    Add new phenomenon to dataset

    :param str name: Name of phenomenon to create
    :raises RuntimeError: In case the phenomenon cannot be created
)",
            "name"_a,
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "phenomena",
            py::overload_cast<>(&Dataset::phenomena),
            R"(
    Return phenomena collection

    :rtype: lue.Phenomena
)",
            py::return_value_policy::reference_internal)

        .def(
            "add_universe",
            &Dataset::add_universe,
            R"(
    Add new universe to dataset

    :param str name: Name of universe to create
    :raises RuntimeError: In case the universe cannot be created
)",
            "name"_a,
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "universes",
            // &Dataset::universes,
            py::overload_cast<>(&Dataset::universes),
            R"(
    Return universes collection

    :rtype: lue.Universes
)",
            py::return_value_policy::reference_internal)

        ;


    module.def(
        "open_dataset",
        // &open_dataset,
        [](
            std::string const& name,
            std::string const& mode)
        {
            return Dataset(name, hdf5::python_mode_to_hdf5_flag(mode));
        },
        R"(
    Open existing LUE dataset
)",
        "name"_a,
        "mode"_a="r",
        py::return_value_policy::move);

    module.def(
        "create_dataset",
        &create_dataset,
        R"(
    Create new LUE dataset

    If the dataset already exists it is overwritten
)",
        "name"_a,
        py::return_value_policy::move)

        ;

}

}  // namespace lue
