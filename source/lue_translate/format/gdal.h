#pragma once
#include "lue_translate/metadata.h"
#include <memory>
#include <gdal_priv.h>


namespace lue {
namespace utility {

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

/*!
    @brief      Unique pointer to a dataset
*/
using GDALDatasetPtr = std::unique_ptr<::GDALDataset, GDALDatasetDeleter>;


GDALDatasetPtr     try_open_gdal_raster_dataset_for_read(
                                        std::string const& dataset_name);

void               translate_gdal_raster_dataset_to_lue(
                                        ::GDALDataset& dataset,
                                        std::string const& lue_dataset_name,
                                        Metadata const& metadata);

}  // namespace utility
}  // namespace lue
