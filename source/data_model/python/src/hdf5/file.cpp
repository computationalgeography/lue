#include "submodule.hpp"
#include "lue/py/hdf5/file.hpp"
#include "lue/hdf5/file.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace hdf5 {

unsigned int python_mode_to_hdf5_flag(
    std::string const& mode)
{
    unsigned int flags = 0;

    if(mode == "r") {
        flags = H5F_ACC_RDONLY;
    }
    else if(mode == "w") {
        flags = H5F_ACC_RDWR;
    }
    else {
        throw py::value_error(
            "mode string must begin with one of 'r' or 'w', not '" +
            mode + "'");
    }

    return flags;
}


std::string intent_to_python_mode(
    unsigned int const intent)
{
    std::string mode;

    if(intent == H5F_ACC_RDONLY) {
        mode = "r";
    }
    else if(intent == H5F_ACC_RDWR) {
        mode = "w";
    }
    else {
        throw py::value_error(
            "intent expected to be H5F_ACC_RDONLY or H5F_ACC_RDWR");
    }

    return mode;
}


void init_file(
        py::module& module)
{

    py::class_<File, Group>(
        module,
        "File",
        R"(
    Class representing an HDF5 file
)")

        .def_property_readonly(
            "hdf5_version",
            &File::hdf5_version,
            R"(
    Return version of HDF5 used to create the dataset

    :rtype: str
)"
        )

        .def_property_readonly(
            "pathname",
            &File::pathname,
            R"(
    Return pathname of HDF5 file
)")

        .def(
            "flush",
            &File::flush,
            R"(
    Flush all buffers associated with the file to disk

    This may be necessary when calling external processes from Python
    that have to read the file. Without flushing the buffers such
    processes may not see a valid HDF5 file.
)")
        ;

    module.def(
        "open_file",
        [](
            std::string const& name,
            std::string const& mode)
        {
            return File{name, python_mode_to_hdf5_flag(mode)};
        },
        R"(
    Open existing HDF5 file
)",
        "name"_a,
        "mode"_a="r",
        py::return_value_policy::move);

    module.def(
        "create_file",
        py::overload_cast<std::string const&>(&create_file),
        R"(
    Create new HDF5 file

    If the file already exists it is overwritten
)",
        "name"_a,
        py::return_value_policy::move);

    module.def(
        "remove_file",
        &remove_file,
        R"(
    Remove HDF5 file
)",
        "name"_a);

    module.def(
        "file_exists",
        &file_exists,
        R"(
    Return whether an HDF5 file exists
)",
        "name"_a);

}

}  // namespace hdf5
}  // namespace lue
