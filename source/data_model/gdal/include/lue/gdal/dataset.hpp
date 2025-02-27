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


    LUE_GDAL_EXPORT auto open_dataset(std::string const& name, GDALAccess open_mode) -> DatasetPtr;

    LUE_GDAL_EXPORT auto try_open_dataset(std::string const& name, GDALAccess open_mode) -> DatasetPtr;

    LUE_GDAL_EXPORT auto create_dataset(
        GDALDriver& driver,
        std::string const& dataset_name,
        Shape const& shape,
        Count nr_bands,
        GDALDataType data_type) -> DatasetPtr;

    LUE_GDAL_EXPORT auto create_dataset(
        std::string const& driver_name,
        std::string const& dataset_name,
        Shape const& shape,
        Count nr_bands,
        GDALDataType data_type) -> DatasetPtr;

    LUE_GDAL_EXPORT auto create_dataset(std::string const& driver_name, std::string const& dataset_name)
        -> DatasetPtr;

    LUE_GDAL_EXPORT auto create_copy(std::string const& name, GDALDataset& clone_dataset) -> DatasetPtr;

    LUE_GDAL_EXPORT auto delete_dataset(GDALDriver& driver, std::string const& dataset_name) -> void;

    LUE_GDAL_EXPORT auto data_type(std::string const& name) -> GDALDataType;

    LUE_GDAL_EXPORT auto data_type(GDALDataset& dataset) -> GDALDataType;

    LUE_GDAL_EXPORT auto nr_raster_bands(GDALDataset& dataset) -> Count;

    LUE_GDAL_EXPORT auto raster_band(GDALDataset& dataset) -> RasterBandPtr;

    LUE_GDAL_EXPORT auto raster_band(GDALDataset& dataset, Count band_nr) -> RasterBandPtr;

    LUE_GDAL_EXPORT auto shape(GDALDataset& dataset) -> Shape;

    LUE_GDAL_EXPORT auto geo_transform(GDALDataset& dataset) -> GeoTransform;

    LUE_GDAL_EXPORT auto set_geo_transform(GDALDataset& dataset, GeoTransform const& geo_transform) -> void;

}  // namespace lue::gdal
