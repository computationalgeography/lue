#include "lue/configure.hpp"
#include "lue/git.hpp"
#include "lue/version.hpp"

#include <pybind11/pybind11.h>


namespace lue {

    PYBIND11_MODULE(lue_py_x, module)
    {
        module.attr("__version__") = pybind11::str(version());
        module.attr("lue_version") = pybind11::str(version());
        module.attr("git_short_sha1") = pybind11::str(short_sha1());
    }

}  // namespace lue
