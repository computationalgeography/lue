#include "lue/framework/api/cxx/io.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework/io/gdal.hpp"
#include "lue/framework.hpp"
#include "lue/gdal.hpp"
// #include <any>
#include <format>
#include <optional>


namespace lue {

    auto write(
        [[maybe_unused]] auto const& field,
        [[maybe_unused]] std::string const& name,
        [[maybe_unused]] std::string const& clone_name) -> hpx::future<void>
    {
        api::detail::unsupported_overload("write", field, name, clone_name);

        return hpx::make_ready_future();
    }


    auto write([[maybe_unused]] auto const& field, [[maybe_unused]] std::string const& name)
        -> hpx::future<void>
    {
        api::detail::unsupported_overload("write", field, name);

        return hpx::make_ready_future();
    }


    namespace api {
        namespace {

            template<typename Element>
            auto from_gdal(std::string const& name, Shape<Count, 2> const& partition_shape)
                -> std::optional<Field::Variant>
            {
                std::optional<Field::Variant> result{};

                if constexpr (lue::arithmetic_element_supported<Element>)
                {
                    result = lue::from_gdal<Element>(name, partition_shape);
                }

                return result;
            }


            auto from_gdal(
                std::string const& name,
                Shape<Count, 2> const& partition_shape,
                GDALDataType const data_type) -> Field
            {
                std::optional<Field::Variant> result{};

                if (data_type == GDT_Byte)
                {
                    result = from_gdal<std::uint8_t>(name, partition_shape);
                }
#if LUE_GDAL_SUPPORTS_8BIT_SIGNED_INTEGERS
                else if (data_type == GDT_Int8)
                {
                    result = from_gdal<std::int8_t>(name, partition_shape);
                }
#endif
                else if (data_type == GDT_UInt32)
                {
                    result = from_gdal<std::uint32_t>(name, partition_shape);
                }
                else if (data_type == GDT_Int32)
                {
                    result = from_gdal<std::int32_t>(name, partition_shape);
                }
#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
                else if (data_type == GDT_UInt64)
                {
                    result = from_gdal<std::uint64_t>(name, partition_shape);
                }
                else if (data_type == GDT_Int64)
                {
                    result = from_gdal<std::int64_t>(name, partition_shape);
                }
#endif
                else if (data_type == GDT_Float32)
                {
                    result = from_gdal<float>(name, partition_shape);
                }
                else if (data_type == GDT_Float64)
                {
                    result = from_gdal<double>(name, partition_shape);
                }

                if (!result)
                {
                    throw std::runtime_error(
                        std::format("Unsupported datatype: {}", "TODO"));  // as_string(data_type)));
                }

                return std::move(*result);
            }

        }  // Anonymous namespace


        /*!
            @brief      Read a raster from a data set using the GDAL API
        */
        auto from_gdal(std::string const& name, Shape<Count, 2> const& partition_shape) -> Field
        {
            gdal::DatasetPtr dataset{gdal::open_dataset(name, GDALAccess::GA_ReadOnly)};
            GDALDataType const data_type{gdal::data_type(*dataset)};

            return from_gdal(name, partition_shape, data_type);
        }


        /*!
            @overload
        */
        auto from_gdal(std::string const& name) -> Field
        {
            gdal::DatasetPtr dataset{gdal::open_dataset(name, GDALAccess::GA_ReadOnly)};
            GDALDataType const data_type{gdal::data_type(*dataset)};
            auto const raster_shape{gdal::shape(*dataset)};
            auto const partition_shape{
                default_partition_shape(Shape<Count, 2>{raster_shape[0], raster_shape[1]})};

            return from_gdal(name, partition_shape, data_type);
        }


        auto to_gdal(Field const& field, std::string const& name, std::string const& clone_name)
            -> hpx::future<void>
        {
            return std::visit(
                overload{[&name, &clone_name](auto const& field) -> hpx::future<void> {
                    return write(field, name, clone_name);
                }},
                field.variant());
        }


        auto to_gdal(Field const& field, std::string const& name) -> hpx::future<void>
        {
            return std::visit(
                overload{[&name](auto const& field) -> hpx::future<void> { return write(field, name); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue
