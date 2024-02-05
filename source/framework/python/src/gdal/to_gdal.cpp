#include "lue/framework/io/raster.hpp"
#include "lue/gdal.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {

    void bind_to_gdal(pybind11::module& module)
    {
        module.def("to_gdal", write<std::uint8_t>, "array"_a, "name"_a, "clone_name"_a = "");
        module.def("to_gdal", write<std::uint32_t>, "array"_a, "name"_a, "clone_name"_a = "");
        module.def("to_gdal", write<std::int32_t>, "array"_a, "name"_a, "clone_name"_a = "");
        if constexpr (gdal::supports_64bit_integers)
        {
            module.def("to_gdal", write<std::uint64_t>, "array"_a, "name"_a, "clone_name"_a = "");
            module.def("to_gdal", write<std::int64_t>, "array"_a, "name"_a, "clone_name"_a = "");
        }
        module.def("to_gdal", write<float>, "array"_a, "name"_a, "clone_name"_a = "");
        module.def("to_gdal", write<double>, "array"_a, "name"_a, "clone_name"_a = "");
    }

}  // namespace lue::framework
