#include "python_extension.hpp"
#include "lue/object/dataset.hpp"
#include "lue/py/hdf5/file.hpp"
#include <pybind11/pybind11.h>
#include <boost/algorithm/string/join.hpp>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace {

static std::string formal_string_representation(
    Dataset const& dataset)
{
    return fmt::format(
        "Dataset(name='{}', flags='{}')",
        dataset.pathname(),
        hdf5::intent_to_python_mode(dataset.intent()));
}


static std::string informal_string_representation(
    Dataset const& dataset)
{
    return fmt::format(
        "{}\n"
        "    universes: [{}]\n"
        "    phenomena: [{}]",
        formal_string_representation(dataset),
        boost::algorithm::join(dataset.universes().names(), ", "),
        boost::algorithm::join(dataset.phenomena().names(), ", "));
}

}  // Anonymous namespace


void init_dataset(
        py::module& module)
{

    py::class_<Dataset, hdf5::File>(
        module,
        "Dataset",
        R"(
    A class representing a scientific database stored in a file

    A LUE dataset contains collections of :class:`universes <Universes>`
    and :class:`phenomena <Phenomena>`.

    New datasets can be created using :func:`create_dataset`. Existing
    datasets can be opened with :func:`open_dataset`.

    .. note::

        A LUE dataset is not similar to an HDF5 dataset. An HDF5 dataset
        represents a multidimensional array in an HDF5 file. A LUE dataset
        is an HDF5 file containing many HDF5 objects, including HDF5
        datasets.
)")

        .def(
            "__repr__",
            [](Dataset const& dataset) {
                return formal_string_representation(dataset);
            }
        )

        .def(
            "__str__",
            [](Dataset const& dataset) {
                return informal_string_representation(dataset);
            }
        )

        .def(
            "add_phenomenon",
            &Dataset::add_phenomenon,
            R"(
    Add new phenomenon to dataset

    :param str name: Name of phenomenon to create
    :raises RuntimeError: In case the phenomenon cannot be created
    :rtype: Phenomenon
)",
            "name"_a,
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "phenomena",
            py::overload_cast<>(&Dataset::phenomena),
            R"(
    Return phenomena collection

    :rtype: Phenomena
)",
            py::return_value_policy::reference_internal)

        .def(
            "add_universe",
            &Dataset::add_universe,
            R"(
    Add new universe to dataset

    :param str name: Name of universe to create
    :raises RuntimeError: In case the universe cannot be created
    :rtype: Universe
)",
            "name"_a,
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "universes",
            py::overload_cast<>(&Dataset::universes),
            R"(
    Return universes collection

    :rtype: Universes
)",
            py::return_value_policy::reference_internal)

        .def(
            "__getattr__",
            [](
                Dataset& dataset,
                std::string const& name)
            {
                py::object result = py::none();

                if(dataset.universes().contains(name)) {
                    result = py::cast(&dataset.universes()[name]);
                }
                else if(dataset.phenomena().contains(name)) {
                    result = py::cast(&dataset.phenomena()[name]);
                }
                else {
                    // TODO We are throwing a KeyError here. Should be
                    // an AttributeError, but pybind11 does not seem to
                    // support that yet.
                    //
                    // Python message:
                    // AttributeError: 'x' object has no attribute 'y'
                    // Ours is a little bit different:
                    throw pybind11::key_error(fmt::format(
                        "Dataset does not contain universe or phenomenon '{}'",
                        name));
                }

                return result;
            },
            py::return_value_policy::reference_internal)

        ;


    module.def(
        "open_dataset",
        [](
            std::string const& name,
            std::string const& mode)
        {
            return Dataset(name, hdf5::python_mode_to_hdf5_flag(mode));
        },
        R"(
    Open existing LUE dataset

    :param str name: Name of dataset to open
    :param str mode: String that specifies the mode in which the dataset
        is opened. The available modes are:

        ========= =======
        Character Meaning
        ========= =======
        r         Open dataset for reading (default)
        w         Open dataset for writing, truncating the dataset first
        ========= =======
    :rtype: lue.Dataset

    Updated datasets can be validated using :func:`lue.validate`.
)",
        "name"_a,
        "mode"_a="r",
        py::return_value_policy::move);

    module.def(
        "create_dataset",
        &create_dataset,
        R"(
    Create new LUE dataset

    :param str name: Name of dataset to create. If a file with this name
        already exists it will be overwritten.
    :rtype: lue.Dataset

    Newly created datasets can be validated using :func:`lue.validate`.
)",
        "name"_a,
        py::return_value_policy::move)

        ;

}

}  // namespace lue
