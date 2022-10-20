#include "lue/configure.hpp"
#include "lue/git.hpp"
#include "lue/py/data_model/submodule.hpp"


namespace py = pybind11;


namespace lue::data_model {

    PYBIND11_MODULE(lue_py_data_model, module)
    {
        module.doc() =
            R"(
    :mod:`lue_py_data_model` --- LUE Scientific Database
    ====================================================
)";
        module.attr("__version__") = py::str(BuildOptions::version);
        module.attr("lue_version") = py::str(BuildOptions::version);
        module.attr("git_short_sha1") = py::str(Git::short_sha1);

        init_submodule(module);
    }

}  // namespace lue::data_model
