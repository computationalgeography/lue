#include "lue/framework/io/gdal.hpp"
#include "lue/framework.hpp"
#include "lue/gdal.hpp"
#include "lue/py/bind.hpp"
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    if constexpr (
                        (!std::is_same_v<Element, std::int8_t> || gdal::supports_8bit_signed_integers) &&
                        (!(std::is_same_v<Element, std::uint64_t> || std::is_same_v<Element, std::int64_t>) ||
                         gdal::supports_64bit_integers))
                    {
                        // If not one of the types not supported by older versions of GDAL OR using a GDAL
                        // version that supports them ...
                        module.def(
                            "to_gdal",
                            to_gdal<Element>,
                            R"(
    Write array to a dataset using GDAL

    :param array: Array to write
    :param name: Name of dataset to write
    :param clone_name: Name of existing dataset to copy geo transform and
        spatial reference from
    :param dict options: Creation options (see GDAL driver documentation for
        a list)

    The GTiff driver will be used to do the writing.
)",
                            "array"_a,
                            "name"_a,
                            pybind11::kw_only(),
                            "clone_name"_a = "",
                            "options"_a = std::map<std::string, std::string>{});
                    }
                }
        };

    }  // Anonymous namespace

    void bind_to_gdal(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework
