#include "lue/configure.hpp"
#include "lue/git.hpp"
#include "lue/py/qa/submodule.hpp"


namespace py = pybind11;


namespace lue::qa {

    PYBIND11_MODULE(_lue, module)
    {
        module.doc() =
            R"(
    :mod:`lue_py_qa` --- LUE Quality Assurance
    ==========================================
)";
        module.attr("__version__") = py::str(BuildOptions::version);
        module.attr("lue_version") = py::str(BuildOptions::version);
        module.attr("git_short_sha1") = py::str(Git::short_sha1);

        init_submodule(module);
    }

}  // namespace lue::qa
