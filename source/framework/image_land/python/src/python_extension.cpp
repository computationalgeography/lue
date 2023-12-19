#include "lue/git.hpp"
#include "lue/py/image_land/submodule.hpp"
#include "lue/version.hpp"


namespace lue::image_land {

    PYBIND11_MODULE(lue_py_image_land, module)
    {
        module.doc() =
            R"(
    :mod:`lue_py_image_land` --- IMAGE-LAND model functionality
    ===========================================================
)";
        module.attr("__version__") = pybind11::str(version());
        module.attr("lue_version") = pybind11::str(version());
        module.attr("git_short_sha1") = pybind11::str(short_sha1());

        init_submodule(module);
    }

}  // namespace lue::image_land
