#include "shape.hpp"
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework/io/gdal.hpp"
#include "lue/framework.hpp"
#include "lue/gdal.hpp"
#include <pybind11/stl.h>
#include <format>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename Element>
        auto from_gdal(std::string const& name, StaticShape<2> const& partition_shape) -> pybind11::object
        {
            // TODO: Even though a partitioned raster is returned, here we grab the layered partitioned array.
            // Return the raster once LUE fully supports passing around rasters.
            return pybind11::cast(lue::from_gdal<Element>(name, partition_shape).cells());
        }


        template<typename Element>
        auto from_gdal(
            std::string const& name, Hyperslab<2> const& hyperslab, StaticShape<2> const& partition_shape)
            -> pybind11::object
        {
            // TODO: Even though a partitioned raster is returned, here we grab the layered partitioned array.
            // Return the raster once LUE fully supports passing around rasters.
            return pybind11::cast(lue::from_gdal<Element>(name, hyperslab, partition_shape).cells());
        }


        auto hyperslab_from_gdal(
            std::string const& name,
            Hyperslab<2> const& hyperslab,
            std::optional<pybind11::tuple> const& partition_shape) -> pybind11::object
        {
            gdal::DatasetPtr dataset{gdal::open_dataset(name, GDALAccess::GA_ReadOnly)};
            StaticShape<2> static_partition_shape{};

            if (partition_shape)
            {
                auto const raster_shape{gdal::shape(*dataset)};
                StaticShape<2> static_array_shape{raster_shape[0], raster_shape[1]};
                DynamicShape const dynamic_partition_shape{tuple_to_shape(*partition_shape)};

                if (static_array_shape.size() != dynamic_partition_shape.size())
                {
                    throw std::runtime_error(
                        std::format(
                            "Rank of array shape and partition shape must be equal ({} != {})",
                            static_array_shape.size(),
                            dynamic_partition_shape.size()));
                }

                static_partition_shape = dynamic_shape_to_static_shape<2>(dynamic_partition_shape);
            }
            else
            {
                static_partition_shape = default_partition_shape(hyperslab.counts());
            }

            pybind11::object result;
            GDALDataType const data_type{gdal::data_type(*dataset)};

            if constexpr (lue::arithmetic_element_supported<std::uint8_t>)
            {
                if (data_type == GDT_Byte)
                {
                    result = framework::from_gdal<uint8_t>(name, hyperslab, static_partition_shape);
                }
            }

#if LUE_GDAL_SUPPORTS_8BIT_SIGNED_INTEGERS == 1
            if constexpr (lue::arithmetic_element_supported<std::int8_t>)
            {
                if (data_type == GDT_Int8)
                {
                    result = framework::from_gdal<int8_t>(name, hyperslab, static_partition_shape);
                }
            }
#endif

            if constexpr (lue::arithmetic_element_supported<std::uint32_t>)
            {
                if (data_type == GDT_UInt32)
                {
                    result = framework::from_gdal<uint32_t>(name, hyperslab, static_partition_shape);
                }
            }

            if constexpr (lue::arithmetic_element_supported<std::int32_t>)
            {
                if (data_type == GDT_Int32)
                {
                    result = framework::from_gdal<int32_t>(name, hyperslab, static_partition_shape);
                }
            }

#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS == 1
            if constexpr (lue::arithmetic_element_supported<std::uint64_t>)
            {
                if (data_type == GDT_UInt64)
                {
                    result = framework::from_gdal<uint64_t>(name, hyperslab, static_partition_shape);
                }
            }

            if constexpr (lue::arithmetic_element_supported<std::int64_t>)
            {
                if (data_type == GDT_Int64)
                {
                    result = framework::from_gdal<int64_t>(name, hyperslab, static_partition_shape);
                }
            }
#endif

            if constexpr (lue::arithmetic_element_supported<float>)
            {
                if (data_type == GDT_Float32)
                {
                    result = framework::from_gdal<float>(name, hyperslab, static_partition_shape);
                }
            }

            if constexpr (lue::arithmetic_element_supported<double>)
            {
                if (data_type == GDT_Float64)
                {
                    result = framework::from_gdal<double>(name, hyperslab, static_partition_shape);
                }
            }

            if (!result)
            {
                throw std::runtime_error(
                    std::format("Unsupported datatype: {}", "TODO"));  // as_string(data_type)));
            }

            return result;
        }


        auto array_from_gdal(std::string const& name, std::optional<pybind11::tuple> const& partition_shape)
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
                    throw std::runtime_error(
                        std::format(
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
                    std::format("Unsupported datatype: {}", "TODO"));  // as_string(data_type)));
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
            properties["shape"] = raster_shape;
            properties["geo_transform"] = geo_transform;

            return properties;
        }

    }  // Anonymous namespace


    void bind_from_gdal(pybind11::module& module)
    {
        module.def(
            "from_gdal",
            array_from_gdal,
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
            "from_gdal",
            hyperslab_from_gdal,
            R"(
    Create new array, filled with values read using GDAL

    :param str name: Name of dataset to read
    :param hyperslab: Hyperslab of cells to read
    :param tuple partition_shape: Shape of the array partitions. When not
        passed in, a default shape will be used which might not result in the
        best performance and scalability.
    :rtype: PartitionedArray specialization
)",
            "name"_a,
            pybind11::kw_only(),
            "hyperslab"_a,
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
