#pragma once
#include "lue/gdal.hpp"
#include "lue/object/dataset.hpp"
#include "lue/utility/metadata.hpp"


namespace lue::utility {

    auto gdal_data_type_to_memory_data_type(GDALDataType data_type) -> hdf5::Datatype;

    auto memory_data_type_to_gdal_data_type(hdf5::Datatype const& data_type) -> GDALDataType;

    auto translate_lue_dataset_to_raster(
        data_model::Dataset& dataset, std::string const& raster_name, Metadata const& metadata) -> void;

    auto try_open_gdal_raster_dataset_for_read(std::string const& dataset_name) -> gdal::DatasetPtr;

    auto translate_gdal_raster_dataset_to_lue(
        std::vector<std::string> const& gdal_dataset_names,
        std::string const& lue_dataset_name,
        bool add,
        Metadata const& metadata) -> void;

}  // namespace lue::utility


/// // // This is nicer than the GDALDatasetDeleter below, but doesn't work
/// // // (gcc 4.9.3).

/// // auto gdal_dataset_deleter =
/// //     [](::GDALDataset* dataset) {
/// //         if(dataset) {
/// //             ::GDALClose(dataset);
/// //         }
/// //     };
/// //
/// // using GDALDatasetDeleter = decltype(gdal_dataset_deleter);

/// struct GDALDatasetDeleter
/// {
///         void operator()(::GDALDataset* dataset) const
///         {
///             if (dataset != nullptr)
///             {
///                 ::GDALClose(dataset);
///             }
///         }
/// };

/// /*!
///     @brief      Unique pointer to a dataset
/// */
/// using GDALDatasetPtr = std::unique_ptr<::GDALDataset, GDALDatasetDeleter>;


/// // GDALDatasetPtr     try_open_gdal_raster_dataset_for_read(
/// //                                         std::string const& dataset_name);

/// // GDALDatasetPtr     try_open_gdal_raster_stack_dataset_for_read(
/// //                                         std::string const& dataset_name);

/// // void               translate_gdal_raster_dataset_to_lue(
/// //                                         std::vector<std::string> const&
/// //                                             gdal_dataset_names,
/// //                                         std::string const& lue_dataset_name,
/// //                                         bool const add,
/// //                                         Metadata const& metadata);

/// // void               translate_gdal_raster_stack_dataset_to_lue(
/// //                                         std::vector<std::string> const&
/// //                                             gdal_dataset_names,
/// //                                         std::string const& lue_dataset_name,
/// //                                         Metadata const& metadata);

/// void translate_lue_dataset_to_raster(
///     data_model::Dataset& dataset, std::string const& raster_name, Metadata const& metadata);

/// // void               translate_lue_dataset_to_shapefile(
/// //                                         Dataset& dataset,
/// //                                         std::string const& shapefile_name,
/// //                                         Metadata const& metadata);

/// GDALDriver* gdal_driver_by_name(std::string const& name);

/// GDALDatasetPtr create_gdal_dataset(std::string const& driver_name, std::string const& dataset_name);


/// GDALDatasetPtr create_dataset(
///     GDALDriver& driver,
///     std::string const& dataset_name,
///     hdf5::Shape const& shape,
///     data_model::Count nr_bands,
///     GDALDataType datatype);

/// GDALDatasetPtr create_dataset(
///     std::string const& driver_name,
///     std::string const& dataset_name,
///     hdf5::Shape const& shape,
///     data_model::Count nr_bands,
///     GDALDataType datatype);

/// GDALDatasetPtr create_dataset(
///     GDALDriver& driver,
///     std::string const& dataset_name);

/// GDALDatasetPtr create_dataset(
///     GDALDriver& driver,
///     std::string const& dataset_name,
///     hdf5::Shape const& shape,
///     data_model::Count nr_bands,
///     hdf5::Datatype const& datatype);

/// GDALDatasetPtr create_dataset(
///     std::string const& driver_name,
///     std::string const& dataset_name,
///     hdf5::Shape const& shape,
///     data_model::Count nr_bands,
///     hdf5::Datatype const& datatype);
