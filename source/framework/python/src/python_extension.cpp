#include "lue/git.hpp"
#include "lue/py/framework/submodule.hpp"
#include "lue/version.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    PYBIND11_MODULE(lue_py_framework, module)
    {
        module.doc() =
            R"(
    :mod:`lue_py_framework` --- Environmental Modelling Framework
    =============================================================
)";
        module.attr("__version__") = pybind11::str(version());
        module.attr("lue_version") = pybind11::str(version());
        module.attr("git_short_sha1") = pybind11::str(short_sha1());

        init_submodule(module);
    }

}  // namespace lue::framework
