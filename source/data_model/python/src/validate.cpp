#include "python_extension.hpp"
#include "lue/validate.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace data_model {

void init_validate(
    py::module& module)
{

    module.def(
        "validate",
        [](std::string const& name) {
            hdf5::Issues issues;
            validate(name, issues);
            return issues;
        },
        "name"_a,
        R"(
    Check whether a file contains a valid LUE dataset

    :param str name: Name of file to check
    :return: Collection of issues found
    :rtype: hdf5.Issues

    See also: :func:`assert_is_valid`
)")
        ;

    module.def(
        "assert_is_valid",
        py::overload_cast<std::string const&, bool const>(&assert_is_valid),
        "name"_a,
        "fail_on_warning"_a=true,
        R"(
    Check whether a file contains a valid LUE dataset

    :param str name: Name of file to check
    :param bool fail_on_warning: Whether or not to treat warnings as errors
    :raises RuntimeError: If errors are found

    See also: :func:`validate`
)")
        ;

    module.def(
        "assert_is_valid",
        py::overload_cast<hdf5::File const&, bool const>(&assert_is_valid),
        "file"_a,
        "fail_on_warning"_a=true,
        R"(
    Check whether a file contains a valid LUE dataset

    :param hdf5.File file: File to check
    :param bool fail_on_warning: Whether or not to treat warnings as errors
    :raises RuntimeError: If errors are found

    See also: :func:`validate`
)")
        ;

}

}  // namespace data_model
}  // namespace lue
