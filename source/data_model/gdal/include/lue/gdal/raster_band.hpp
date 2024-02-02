#pragma once
#include "lue/gdal/define.hpp"
#include <gdal_priv.h>


namespace lue::gdal {

    /*!
        @brief      Type for pointers to GDAL raster bands

        Driver pointers are owned by the dataset and must not be deleted.
    */
    using RasterBandPtr = GDALRasterBand*;


    auto data_type(GDALRasterBand& band) -> GDALDataType;

    auto block_size(GDALRasterBand& band) -> Shape;

    auto band_size(GDALRasterBand& band) -> Shape;


    namespace detail {

        template<typename Element>
        auto no_data_value(GDALRasterBand& band, int* success) -> Element
        {
            return band.GetNoDataValue(success);
        }


#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 5, 0)
        // "An explicit specialization of a function template is inline only if it is declared
        // with the inline specifier (or defined as deleted), it doesn't matter if the primary
        // template is inline."

        template<>
        inline auto no_data_value(GDALRasterBand& band, int* success) -> std::int64_t
        {
            return band.GetNoDataValueAsInt64(success);
        }


        template<>
        inline auto no_data_value(GDALRasterBand& band, int* success) -> std::uint64_t
        {
            return band.GetNoDataValueAsUInt64(success);
        }
#endif


        template<typename Element>
        auto set_no_data_value(GDALRasterBand& band, Element const value) -> CPLErr
        {
            return band.SetNoDataValue(static_cast<double>(value));
        }


#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 5, 0)
        // "An explicit specialization of a function template is inline only if it is declared
        // with the inline specifier (or defined as deleted), it doesn't matter if the primary
        // template is inline."

        template<>
        inline auto set_no_data_value(GDALRasterBand& band, std::int64_t const value) -> CPLErr
        {
            return band.SetNoDataValueAsInt64(value);
        }


        template<>
        inline auto set_no_data_value(GDALRasterBand& band, std::uint64_t const value) -> CPLErr
        {
            return band.SetNoDataValueAsUInt64(value);
        }
#endif

    }  // namespace detail


    /*!
        @brief      Return the no-data value from the raster band passed in
        @param      .
        @return     Tuple with the no-data value and a value to test whether a no-data value
                    is actually associated with the layer. Don't use the no-data value if the
                    boolean is false.
    */
    template<typename Element>
    auto no_data_value(GDALRasterBand& band) -> std::tuple<Element, bool>
    {
        int success{};
        Element value{detail::no_data_value<Element>(band, &success)};

        return std::make_tuple(value, success != 0);
    }


    /*!
        @brief      Associate a no-data value with a band
        @return     Whether or not setting the no-data value succeeded. If this value is false,
                    it may be that the driver does not support setting a no-data value. Depending
                    on the context this may or may not be an error.
    */
    template<typename Element>
    auto set_no_data_value(GDALRasterBand& band, Element const value) -> bool
    {
        return detail::set_no_data_value(band, value) == CE_None;
    }

}  // namespace lue::gdal
