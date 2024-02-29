#include "lue/git.hpp"
#include "lue/py/qa/submodule.hpp"
#include "lue/version.hpp"


namespace lue::qa {

    PYBIND11_MODULE(_lue, module)
    {
        module.doc() =
            R"(
    :mod:`lue_py_qa` --- LUE Quality Assurance
    ==========================================
)";
        module.attr("__version__") = pybind11::str(version());
        module.attr("lue_version") = pybind11::str(version());
        module.attr("git_short_sha1") = pybind11::str(short_sha1());

        init_submodule(module);
    }

}  // namespace lue::qa
