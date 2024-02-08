#pragma once
#include "lue/gdal/define.hpp"
#include "lue/gdal/raster_band.hpp"
#include <gdal_priv.h>
#include <memory>


namespace lue::gdal {


    /*!
        @brief      Type for pointers to GDAL datasets

        Dataset pointers are owned by you and must be deleted after use.
    */
    using DatasetPtr = std::shared_ptr<GDALDataset>;


    /*!
        @brief      Deleter to use when creating shared pointers to GDAL datasets
    */
    inline auto const gdal_close = [](GDALDataset* dataset)
    {
        if (dataset != nullptr)
        {
            GDALClose(dataset);
        }
    };

    using GDALDatasetClose = decltype(gdal_close);

    static_assert(std::is_move_constructible_v<GDALDatasetClose>);


    auto open_dataset(std::string const& name, GDALAccess open_mode) -> DatasetPtr;

    auto try_open_dataset(std::string const& name, GDALAccess open_mode) -> DatasetPtr;

    auto create_dataset(
        GDALDriver& driver,
        std::string const& dataset_name,
        Shape const& shape,
        Count nr_bands,
        GDALDataType data_type) -> DatasetPtr;

    auto create_dataset(
        std::string const& driver_name,
        std::string const& dataset_name,
        Shape const& shape,
        Count nr_bands,
        GDALDataType data_type) -> DatasetPtr;

    auto create_dataset(std::string const& driver_name, std::string const& dataset_name) -> DatasetPtr;

    auto create_copy(std::string const& name, GDALDataset& clone_dataset) -> DatasetPtr;

    auto delete_dataset(GDALDriver& driver, std::string const& dataset_name) -> void;

    auto data_type(std::string const& name) -> GDALDataType;

    auto data_type(GDALDataset& dataset) -> GDALDataType;

    auto nr_raster_bands(GDALDataset& dataset) -> Count;

    auto raster_band(GDALDataset& dataset) -> RasterBandPtr;

    auto raster_band(GDALDataset& dataset, Count band_nr) -> RasterBandPtr;

    auto shape(GDALDataset& dataset) -> Shape;

    auto geo_transform(GDALDataset& dataset) -> GeoTransform;

    auto set_geo_transform(GDALDataset& dataset, GeoTransform const& geo_transform) -> void;

}  // namespace lue::gdal
