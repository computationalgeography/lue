#pragma once
#include "lue/gdal/blocks.hpp"
#include "lue/gdal/dataset.hpp"
#include "lue/gdal/raster_band.hpp"


namespace lue::gdal {

    /*!
        @brief      Utility class for performing I/O to a raster and its bands
    */
    class LUE_GDAL_EXPORT Raster
    {

        public:

            /*!
                @brief      Utility class for performing I/O to a raster band
            */
            class LUE_GDAL_EXPORT Band
            {

                public:

                    explicit Band(RasterBandPtr band);

                    Band(Band const&) = delete;

                    Band(Band&&) = default;

                    ~Band() = default;

                    auto operator=(Band const&) -> Band& = delete;

                    auto operator=(Band&&) -> Band& = default;

                    auto data_type() -> GDALDataType;

                    auto block_shape() -> Shape;

                    auto shape() -> Shape;


                    /*!
                        @sa         gdal::no_data_value
                    */
                    template<typename Element>
                    auto no_data_value() -> std::tuple<Element, bool>
                    {
                        return gdal::no_data_value<Element>(*_band_ptr);
                    }


                    /*!
                        @sa         gdal::set_no_data_value
                    */
                    template<typename Element>
                    auto set_no_data_value(Element const value) -> void
                    {
                        gdal::set_no_data_value<Element>(*_band_ptr, value);
                    }


                    auto read_block(Offset const& block_offset, void* buffer) -> void;

                    auto write_block(Offset const& block_offset, void* buffer) -> void;

                    auto read(void* buffer) -> void;

                    auto write(void* buffer) -> void;

                    auto write(Offset const& offset, Shape const& shape, GDALDataType data_type, void* buffer)
                        -> void;

                    auto write(Shape const& shape, GDALDataType data_type, void* buffer) -> void;


                private:

                    RasterBandPtr _band_ptr;
            };


            explicit Raster(DatasetPtr dataset_ptr);

            Raster(Raster const&) = delete;

            Raster(Raster&&) = default;

            ~Raster() = default;

            auto operator=(Raster const&) -> Raster& = delete;

            auto operator=(Raster&&) -> Raster& = default;

            auto data_type() -> GDALDataType;

            auto shape() -> Shape;

            auto geo_transform() -> GeoTransform;

            auto set_geo_transform(GeoTransform const& geo_transform) -> void;

            auto band(Count band_nr) -> Band;

            auto nr_bands() -> Count;

        private:

            DatasetPtr _dataset_ptr;
    };

}  // namespace lue::gdal
