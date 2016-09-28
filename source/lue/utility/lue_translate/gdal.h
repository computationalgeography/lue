#pragma once
#include <memory>
#include <string>
#include <gdal_priv.h>
#include "lue/utility/lue_translate/time/month_time_point.h"


namespace lue {

// This is nicer than the GDALDatasetDeleter below, but doesn't work
// (gcc 4.9.3).
//
// auto gdal_dataset_deleter =
//     [](::GDALDataset* dataset) {
//         if(dataset) {
//             ::GDALClose(dataset);
//         }
//     };
// 
// using GDALDatasetDeleter = decltype(gdal_dataset_deleter);

struct GDALDatasetDeleter
{
    void operator()(::GDALDataset* dataset) const
    {
        if(dataset) {
            ::GDALClose(dataset);
        }
    }
};

using GDALDatasetPtr = std::unique_ptr<::GDALDataset, GDALDatasetDeleter>;


GDALDatasetPtr     try_open_gdal_raster_dataset_for_read(
                                        std::string const& input_dataset_name);

GDALDatasetPtr     try_open_gdal_feature_dataset_for_read(
                                        std::string const& input_dataset_name);

GDALDatasetPtr     try_open_gdal_raster_stack_for_read(
                                        std::string const& input_dataset_name);

GDALDatasetPtr     try_open_gdal_feature_stack_for_read(
                                        std::string const& input_dataset_name);

GDALDatasetPtr     try_open_gdal_track_dataset_for_read(
                                        std::string const& input_dataset_name);

void               translate_gdal_raster_to_lue(
                                        ::GDALDataset& dataset,
                                        std::string const& gdal_dataset_name,
                                        std::string const& lue_dataset_name);

void               translate_gdal_raster_stack_to_lue(
                                        ::GDALDataset& dataset,
                                        std::string const& gdal_stack_name,
                                        MonthTimePoint const& start_time_point,
                                        MonthDuration const& duration,
                                        std::string const& lue_dataset_name);

void               translate_gdal_feature_stack_to_lue(
                                        ::GDALDataset& dataset,
                                        std::string const& gdal_stack_name,
                                        MonthTimePoint const& start_time_point,
                                        MonthDuration const& duration,
                                        std::string const& lue_dataset_name);

void               translate_gdal_track_to_lue(
                                        ::GDALDataset& dataset,
                                        std::string const& gdal_dataset_name,
                                        std::string const& lue_dataset_name);

} // namespace lue
