#include "shape.hpp"
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework/io/raster.hpp"
#include "lue/framework.hpp"
#include "lue/gdal.hpp"
#include <fmt/format.h>
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename Element>
        auto from_gdal(std::string const& name, StaticShape<2> const& partition_shape) -> pybind11::object
        {
            return pybind11::cast(read<Element>(name, partition_shape));
        }


        auto from_gdal_py(std::string const& name, std::optional<pybind11::tuple> const& partition_shape)
            -> pybind11::object
        {
            gdal::DatasetPtr dataset{gdal::open_dataset(name, GDALAccess::GA_ReadOnly)};
            auto const raster_shape{gdal::shape(*dataset)};
            StaticShape<2> static_array_shape{raster_shape[0], raster_shape[1]};
            StaticShape<2> static_partition_shape{};

            if (partition_shape)
            {
                DynamicShape const dynamic_partition_shape{tuple_to_shape(*partition_shape)};

                if (static_array_shape.size() != dynamic_partition_shape.size())
                {
                    throw std::runtime_error(fmt::format(
                        "Rank of array shape and partition shape must be equal ({} != {})",
                        static_array_shape.size(),
                        dynamic_partition_shape.size()));
                }

                static_partition_shape = dynamic_shape_to_static_shape<2>(dynamic_partition_shape);
            }
            else
            {
                static_partition_shape = default_partition_shape(static_array_shape);
            }

            pybind11::object result;
            GDALDataType const data_type{gdal::data_type(*dataset)};

            if constexpr (lue::arithmetic_element_supported<std::uint8_t>)
            {
                if (data_type == GDT_Byte)
                {
                    result = from_gdal<uint8_t>(name, static_partition_shape);
                }
            }

#if LUE_GDAL_SUPPORTS_8BIT_SIGNED_INTEGERS == 1
            if constexpr (lue::arithmetic_element_supported<std::int8_t>)
            {
                if (data_type == GDT_Int8)
                {
                    result = from_gdal<int8_t>(name, static_partition_shape);
                }
            }
#endif

            if constexpr (lue::arithmetic_element_supported<std::uint32_t>)
            {
                if (data_type == GDT_UInt32)
                {
                    result = from_gdal<uint32_t>(name, static_partition_shape);
                }
            }

            if constexpr (lue::arithmetic_element_supported<std::int32_t>)
            {
                if (data_type == GDT_Int32)
                {
                    result = from_gdal<int32_t>(name, static_partition_shape);
                }
            }

#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS == 1
            if constexpr (lue::arithmetic_element_supported<std::uint64_t>)
            {
                if (data_type == GDT_UInt64)
                {
                    result = from_gdal<uint64_t>(name, static_partition_shape);
                }
            }

            if constexpr (lue::arithmetic_element_supported<std::int64_t>)
            {
                if (data_type == GDT_Int64)
                {
                    result = from_gdal<int64_t>(name, static_partition_shape);
                }
            }
#endif

            if constexpr (lue::arithmetic_element_supported<float>)
            {
                if (data_type == GDT_Float32)
                {
                    result = from_gdal<float>(name, static_partition_shape);
                }
            }

            if constexpr (lue::arithmetic_element_supported<double>)
            {
                if (data_type == GDT_Float64)
                {
                    result = from_gdal<double>(name, static_partition_shape);
                }
            }

            if (!result)
            {
                throw std::runtime_error(
                    fmt::format("Unsupported datatype: {}", "TODO"));  // as_string(data_type)));
            }

            return result;
        }


        auto probe_gdal(std::string const& name) -> pybind11::dict
        {
            // Open GDAL dataset and return its properties
            gdal::DatasetPtr dataset{gdal::open_dataset(name, GDALAccess::GA_ReadOnly)};
            auto const raster_shape{gdal::shape(*dataset)};
            auto const geo_transform{gdal::geo_transform(*dataset)};

            pybind11::dict properties{};
            properties("shape"_a = raster_shape);
            properties("geo_transform"_a = geo_transform);

            return properties;
        }

    }  // Anonymous namespace


    void bind_from_gdal(pybind11::module& module)
    {
        module.def(
            "from_gdal",
            from_gdal_py,
            R"(
    Create new array, filled with values read using GDAL

    :param str name: Name of dataset to read
    :param tuple partition_shape: Shape of the array partitions. When not
        passed in, a default shape will be used which might not result in the
        best performance and scalability.
    :rtype: PartitionedArray specialization
)",
            "name"_a,
            pybind11::kw_only(),
            "partition_shape"_a = std::optional<pybind11::tuple>{},
            pybind11::return_value_policy::move);

        module.def(
            "probe_gdal",
            probe_gdal,
            R"(
    Open raster using GDAL and return its properties

    :param str name: Name of dataset to probe
    :rtype: Dictionary with information found
)",
            "name"_a);
    }

}  // namespace lue::framework
