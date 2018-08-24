#include "lue/hdf5/validate.hpp"
#include <pybind11/stl_bind.h>


namespace py = pybind11;


namespace lue {
namespace hdf5 {

void init_validate(
    py::module& module)
{

    py::class_<Issue>(
        module,
        "Issue",
        R"(
    Details of an issue found during validation of a dataset
)")

        .def_property_readonly(
            "message",
            &Issue::message,
            R"(
    Description of the issue
)")

        ;


    py::bind_vector<std::vector<Issue>>(
        module,
        "VectorIssue")

        ;


    py::class_<Issues>(
        module,
        "Issues",
        R"(
    Collection of issues found during validation of a dataset
)")

        .def_property_readonly(
            "errors_found",
            &Issues::errors_found,
            R"(
    Return whether errors where found

    :rtype: bool
)")

        .def_property_readonly(
            "warnings_found",
            &Issues::warnings_found,
            R"(
    Return whether warnings where found

    :rtype: bool
)")

        .def_property_readonly(
            "errors",
            &Issues::errors,
            R"(
    Return collection of errors

    :rtype: Collection of Issue instances
)")

        .def_property_readonly(
            "warnings",
            &Issues::warnings,
            R"(
    Return collection of warnings

    :rtype: Collection of Issue instances
)")

        ;

}

}  // namespace hdf5
}  // namespace lue
