#include "lue/framework/api/cxx/io.hpp"
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::api {

    void bind_io(pybind11::module& module)
    {
        module.def(
            "from_gdal",
            [](
                std::string const& name,
                std::optional<Shape<Count, 2>> const& partition_shape) -> Field
            {
                return partition_shape ? from_gdal(name, *partition_shape)
                                       : from_gdal(name);
            },
            "array_shape"_a,
            pybind11::kw_only(),
            "partition_shape"_a = std::optional<Shape<Count, 2>>{},
            pybind11::return_value_policy::move);

        module.def(
            "to_gdal",
            to_gdal,
            "field"_a,
            "name"_a);
    }

}  // namespace lue::api
