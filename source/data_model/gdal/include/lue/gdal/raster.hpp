#pragma once
#include "lue/gdal/blocks.hpp"
#include "lue/gdal/dataset.hpp"
#include "lue/gdal/raster_band.hpp"


namespace lue::gdal {

    class Raster
    {

        public:

            class Band
            {

                public:

                    explicit Band(RasterBandPtr band);

                    Band(Band const&) = delete;

                    Band(Band&&) = default;

                    ~Band() = default;

                    auto operator=(Band const&) -> Band& = delete;

                    auto operator=(Band&&) -> Band& = default;

                    [[nodiscard]] auto data_type() const -> GDALDataType;

                    [[nodiscard]] auto blocks() const -> Blocks;

                    auto read_block(Offset const& block_offset, void* buffer) const -> void;

                    auto read(void* buffer) const -> void;

                    auto write_block(Offset const& block_offset, void* buffer) -> void;

                    // void       write               (hl::Raster::Band& raster_band,
                    //                                 ProgressIndicator& progress_indicator);

                private:

                    // template<
                    //     typename T>
                    // void       write               (hl::Raster::Band& raster_band,
                    //                                 ProgressIndicator& progress_indicator);

                    RasterBandPtr _band;
            };


            explicit Raster(std::string const& dataset_name);

            explicit Raster(DatasetPtr dataset_ptr);

            Raster(Raster const&) = delete;

            Raster(Raster&&) = default;

            ~Raster() = default;

            auto operator=(Raster const&) -> Raster& = delete;

            auto operator=(Raster&&) -> Raster& = default;

            [[nodiscard]] auto shape() const -> Shape;

            [[nodiscard]] auto geo_transform() const -> GeoTransform;

            auto set_geo_transform(GeoTransform const& geo_transform) -> void;

            [[nodiscard]] auto band(Count band_nr) const -> Band;

            [[nodiscard]] auto nr_bands() const -> Count;

        private:

            DatasetPtr _dataset_ptr;
    };

}  // namespace lue::gdal
