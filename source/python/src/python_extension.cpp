#include "lue/configure.hpp"
#ifdef LUE_DATA_MODEL_WITH_PYTHON_API
#include "lue/py/data_model/submodule.hpp"
#endif
#ifdef LUE_FRAMEWORK_WITH_PYTHON_API
#include "lue/py/framework/submodule.hpp"
#endif
#include <pybind11/pybind11.h>
#include <boost/algorithm/string/join.hpp>
#include <fmt/format.h>


namespace py = pybind11;


namespace lue {

PYBIND11_MODULE(lue, module)
{
    std::vector<std::string> automodules;

#ifdef LUE_DATA_MODEL_WITH_PYTHON_API
    automodules.push_back("data_model");
#endif

#ifdef LUE_FRAMEWORK_WITH_PYTHON_API
    automodules.push_back("framework");
#endif

    for(std::string& item: automodules)
    {
        item = fmt::format(".. automodule:: lue.{}", item);
    }

    module.doc() =
        fmt::format(R"(
    :mod:`lue` --- Scientific Database and Environmental Modelling Framework
    ========================================================================

    The :mod:`lue` package provides functionality for ...

    .. automodule:: lue.data_model
    .. automodule:: lue.framework
)", boost::algorithm::join(automodules, "\n    "));

    module.attr("__version__") = py::str(BuildOptions::version);
    module.attr("lue_version") = py::str(BuildOptions::version);
    module.attr("git_short_sha1") = py::str(BuildOptions::git_short_sha1);

#ifdef LUE_DATA_MODEL_WITH_PYTHON_API
    data_model::init_submodule(module);
#endif

#ifdef LUE_FRAMEWORK_WITH_PYTHON_API
    framework::init_submodule(module);
#endif
}

}  // namespace lue
