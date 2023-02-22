#pragma once
#include "lue/framework/core/shape.hpp"
#include "lue/framework/io/gdal_type_traits.hpp"


namespace lue {

    // This pointer is shared and the object pointed to must be closed once the last
    // pointer instance goes out of scope
    using GDALDatasetPtr = std::shared_ptr<::GDALDataset>;

    inline auto gdal_close = [](::GDALDataset* dataset)
    {
        if (dataset)
        {
            ::GDALClose(dataset);
        }
    };

    using GDALDatasetClose = decltype(gdal_close);

    static_assert(std::is_move_constructible_v<GDALDatasetClose>);

    // The object pointed to is owned by the dataset and must not be deleted
    using GDALBandPtr = ::GDALRasterBand*;

    using GDALDriverPtr = ::GDALDriver*;

    GDALDatasetPtr open_dataset(std::string const& name, GDALAccess const open_mode);

    GDALDatasetPtr create_copy(std::string const& name, GDALDatasetPtr& clone_dataset);

    GDALDatasetPtr create(
        std::string const& name,
        Shape<Count, 2> const& shape,
        Count const nr_bands,
        GDALDataType const data_type);

    GDALBandPtr get_raster_band(GDALDataset& dataset);

    GDALDataType data_type(GDALRasterBand& band);

    GDALDataType data_type(GDALDataset& dataset);

    GDALDataType data_type(std::string const& name);


    namespace detail {

        template<typename Element>
        Element no_data_value(::GDALRasterBand& band, int* success)
        {
            return band.GetNoDataValue(success);
        }


#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 5, 0)
        // "An explicit specialization of a function template is inline only if it is declared
        // with the inline specifier (or defined as deleted), it doesn't matter if the primary
        // template is inline."
        template<>
        inline std::int64_t no_data_value(::GDALRasterBand& band, int* success)
        {
            return band.GetNoDataValueAsInt64(success);
        }


        template<>
        inline std::uint64_t no_data_value(::GDALRasterBand& band, int* success)
        {
            return band.GetNoDataValueAsUInt64(success);
        }
#endif


        template<typename Element>
        CPLErr set_no_data_value(::GDALRasterBand& band, Element const value)
        {
            return band.SetNoDataValue(static_cast<double>(value));
        }


#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 5, 0)
        // "An explicit specialization of a function template is inline only if it is declared
        // with the inline specifier (or defined as deleted), it doesn't matter if the primary
        // template is inline."
        template<>
        inline CPLErr set_no_data_value(::GDALRasterBand& band, std::int64_t const value)
        {
            return band.SetNoDataValueAsInt64(value);
        }


        template<>
        inline CPLErr set_no_data_value(::GDALRasterBand& band, std::uint64_t const value)
        {
            return band.SetNoDataValueAsUInt64(value);
        }
#endif

    }  // namespace detail


    template<typename Element>
    bool set_no_data_value(::GDALRasterBand& band, Element const value)
    {
        // Return true if set, and false if not. If not set, it may be that the driver does
        // not support setting a no-data value. Depending on the context, this may not be an error.
        return detail::set_no_data_value(band, value) == CE_None;
    }

}  // namespace lue
