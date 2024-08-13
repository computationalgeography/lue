#include "lue/framework/api/cxx/io.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::api {

    void bind_io(pybind11::module& module)
    {
        module.def(
            "from_gdal",
            [](std::string const& name, std::optional<Shape<Count, 2>> const& partition_shape) -> Field
            { return partition_shape ? from_gdal(name, *partition_shape) : from_gdal(name); },
            "array_shape"_a,
            pybind11::kw_only(),
            "partition_shape"_a = std::optional<Shape<Count, 2>>{},
            pybind11::return_value_policy::move);

        module.def(
            "to_gdal",
            [](Field const& field,
               std::string const& name,
               std::optional<std::string> const& clone_name) -> hpx::future<void>
            { return clone_name ? to_gdal(field, name, *clone_name) : to_gdal(field, name); },
            "field"_a,
            "name"_a,
            pybind11::kw_only(),
            "clone_name"_a = std::optional<std::string>{});
    }

}  // namespace lue::api
