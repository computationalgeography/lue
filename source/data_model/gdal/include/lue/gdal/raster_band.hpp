#pragma once
#include "lue/gdal/define.hpp"
#include "lue/gdal/export.hpp"
#include <gdal_priv.h>


namespace lue::gdal {

    /*!
        @brief      Type for pointers to GDAL raster bands

        Driver pointers are owned by the dataset and must not be deleted.
    */
    using RasterBandPtr = GDALRasterBand*;


    LUE_GDAL_EXPORT auto data_type(GDALRasterBand& band) -> GDALDataType;

    LUE_GDAL_EXPORT auto block_shape(GDALRasterBand& band) -> Shape;

    LUE_GDAL_EXPORT auto shape(GDALRasterBand& band) -> Shape;

    LUE_GDAL_EXPORT auto read_block(GDALRasterBand& band, Offset const& block_offset, void* buffer) -> void;

    LUE_GDAL_EXPORT auto write_block(GDALRasterBand& band, Offset const& block_offset, void* buffer) -> void;

    LUE_GDAL_EXPORT auto read(
        GDALRasterBand& band,
        Offset const& offset,
        Shape const& raster_shape,
        GDALDataType data_type,
        void* buffer) -> void;

    LUE_GDAL_EXPORT auto read(GDALRasterBand& band, Shape const& shape, GDALDataType data_type, void* buffer)
        -> void;

    LUE_GDAL_EXPORT auto write(
        GDALRasterBand& band,
        Offset const& offset,
        Shape const& raster_shape,
        GDALDataType data_type,
        void* buffer) -> void;

    LUE_GDAL_EXPORT auto write(GDALRasterBand& band, Shape const& shape, GDALDataType data_type, void* buffer)
        -> void;


    namespace detail {

        template<typename Element>
        auto no_data_value(GDALRasterBand& band, int* success) -> Element
        {
            return band.GetNoDataValue(success);
        }


#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
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
            return band.SetNoDataValue(static_cast<NoDataValue>(value));
        }


#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
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
