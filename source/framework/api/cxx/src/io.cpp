#include "lue/framework/api/cxx/io.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/gdal.hpp"
#include "lue/framework/io/raster.hpp"
#include "fmt/format.h"
#include <optional>


namespace lue {

    void write(
        [[maybe_unused]] auto const& field,
        [[maybe_unused]] std::string const& name)
    {
        api::detail::unsupported_overload("write", field, name);
    }


    namespace api {
        namespace {

            auto from_gdal(
                std::string const& name,
                Shape<Count, 2> const& partition_shape,
                GDALDataType const data_type) -> Field
            {
                std::optional<Field> result{};

                if (data_type == GDT_Byte)
                {
                    result = read<uint8_t>(name, partition_shape);
                }
                else if (data_type == GDT_UInt32)
                {
                    result = read<uint32_t>(name, partition_shape);
                }
                else if (data_type == GDT_Int32)
                {
                    result = read<int32_t>(name, partition_shape);
                }
#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
                else if (data_type == GDT_UInt64)
                {
                    result = read<uint64_t>(name, partition_shape);
                }
                else if (data_type == GDT_Int64)
                {
                    result = read<int64_t>(name, partition_shape);
                }
#endif
                else if (data_type == GDT_Float32)
                {
                    result = read<float>(name, partition_shape);
                }
                else if (data_type == GDT_Float64)
                {
                    result = read<double>(name, partition_shape);
                }

                if (!result)
                {
                    throw std::runtime_error(
                        fmt::format("Unsupported datatype: {}", "TODO"));  // as_string(data_type)));
                }

                return std::move(*result);
            }

        }  // Anonymous namespace


        /*!
            @brief      Read a raster from a data set using the GDAL API
        */
        auto from_gdal(
            std::string const& name,
            Shape<Count, 2> const& partition_shape) -> Field
        {
            gdal::DatasetPtr dataset{gdal::open_dataset(name, GDALAccess::GA_ReadOnly)};
            GDALDataType const data_type{gdal::data_type(*dataset)};

            return from_gdal(name, partition_shape, data_type);
        }


        /*!
            @overload
        */
        auto from_gdal(
            std::string const& name) -> Field
        {
            gdal::DatasetPtr dataset{gdal::open_dataset(name, GDALAccess::GA_ReadOnly)};
            GDALDataType const data_type{gdal::data_type(*dataset)};
            auto const raster_shape{gdal::shape(*dataset)};
            auto const partition_shape{
                default_partition_shape(Shape<Count, 2>{raster_shape[0], raster_shape[1]})};

            return from_gdal(name, partition_shape, data_type);
        }


        void to_gdal(Field const& field, std::string const& name)
        {
            return std::visit(
                overload{[&name](auto const& field) -> void {
                    write(field, name);
                }},
                field);
        }

    }  // namespace api
}  // namespace lue
