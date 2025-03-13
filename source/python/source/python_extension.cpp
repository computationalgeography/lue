#include "lue/configure.hpp"
#include "lue/git.hpp"
#include "lue/version.hpp"
#ifdef LUE_BUILD_DATA_MODEL
#ifdef LUE_DATA_MODEL_WITH_PYTHON_API
#include "lue/py/data_model/submodule.hpp"
#endif
#endif
#ifdef LUE_BUILD_FRAMEWORK
#ifdef LUE_FRAMEWORK_WITH_PYTHON_API
#include "lue/py/framework/submodule.hpp"
#include "lue/py/framework_x/submodule.hpp"
#ifdef LUE_FRAMEWORK_WITH_IMAGE_LAND
#include "lue/py/image_land/submodule.hpp"
#endif
#endif
#endif
#ifdef LUE_BUILD_QUALITY_ASSURANCE
#ifdef LUE_QUALITY_ASSURANCE_WITH_PYTHON_API
#include "lue/py/qa/submodule.hpp"
#endif
#endif
#include <boost/algorithm/string/join.hpp>
#include <format>


namespace lue {

    PYBIND11_MODULE(lue_py, module)
    {
        std::vector<std::string> automodules;

#ifdef LUE_BUILD_DATA_MODEL
#ifdef LUE_DATA_MODEL_WITH_PYTHON_API
        automodules.emplace_back("data_model");
#endif
#endif

#ifdef LUE_BUILD_FRAMEWORK
#ifdef LUE_FRAMEWORK_WITH_PYTHON_API
        automodules.emplace_back("framework");
        automodules.emplace_back("framework_x");
#ifdef LUE_FRAMEWORK_WITH_IMAGE_LAND
        automodules.emplace_back("image_land");
#endif
#endif
#endif

#ifdef LUE_QUALITY_ASSURANCE_WITH_PYTHON_API
        automodules.emplace_back("qa");
#endif

        for (std::string& item : automodules)
        {
            item = std::format(".. automodule:: lue.{}", item);
        }

        module.doc() = std::format(
            R"(
    :mod:`lue` --- Scientific Database and Environmental Modelling Framework
    ========================================================================

    The :mod:`lue` package provides functionality for ...

    {}
)",
            boost::algorithm::join(automodules, "\n    "));

        module.attr("__version__") = pybind11::str(version());
        module.attr("lue_version") = pybind11::str(version());
        module.attr("git_short_sha1") = pybind11::str(short_sha1());

#ifdef LUE_BUILD_DATA_MODEL
#ifdef LUE_DATA_MODEL_WITH_PYTHON_API
        data_model::init_submodule(module);
#endif
#endif

#ifdef LUE_BUILD_FRAMEWORK
#ifdef LUE_FRAMEWORK_WITH_PYTHON_API
        framework::init_submodule(module);
        api::init_framework_x(module);
#ifdef LUE_FRAMEWORK_WITH_IMAGE_LAND
        image_land::init_submodule(module);
#endif
#endif
#endif

#ifdef LUE_BUILD_QUALITY_ASSURANCE
#ifdef LUE_QUALITY_ASSURANCE_WITH_PYTHON_API
        qa::init_submodule(module);
#endif
#endif
    }

}  // namespace lue
