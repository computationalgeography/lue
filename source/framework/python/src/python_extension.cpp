#include "lue/configure.hpp"
#include "lue/py/framework/submodule.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace framework {

PYBIND11_MODULE(lue_py_framework, module)
{
    module.doc() =
        R"(
    :mod:`lue_py_framework` --- Environmental Modelling Framework
    ==========================================================
)";
    module.attr("__version__") = py::str(BuildOptions::version);
    module.attr("lue_version") = py::str(BuildOptions::version);
    module.attr("git_short_sha1") = py::str(BuildOptions::git_short_sha1);

    init_submodule(module);
}

}  // namespace framework
}  // namespace lue
