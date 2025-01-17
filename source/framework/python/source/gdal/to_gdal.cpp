#include "lue/framework/io/raster.hpp"
#include "lue/framework.hpp"
#include "lue/gdal.hpp"
#include "lue/py/bind.hpp"


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
                        module.def("to_gdal", write<Element>, "array"_a, "name"_a, "clone_name"_a = "");
                    }
                }
        };

    }  // Anonymous namespace

    void bind_to_gdal(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework
