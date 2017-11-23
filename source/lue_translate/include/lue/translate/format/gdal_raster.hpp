#pragma once
#include "lue/translate/format/gdal_block.hpp"
#include "lue/utility/progress_indicator.hpp"
#include "lue/hl/raster.hpp"
#include "lue/hdf5/datatype.hpp"
#include <gdal_priv.h>
#include <memory>
#include <string>


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

GDALDatasetPtr     open_gdal_raster_dataset_for_read(
                                        std::string const& dataset_name);


class GDALRaster
{

public:

    class Band
    {

    public:

                   Band                (GDALRasterBand* band);

                   Band                (Band const&)=delete;

                   Band                (Band&&)=default;

                   ~Band               ()=default;

        Band&      operator=           (Band const&)=delete;

        Band&      operator=           (Band&&)=default;

        hdf5::Datatype datatype        () const;

        GDALBlock  blocks              () const;

        void       write               (hl::Raster::Band& raster_band,
                                        ProgressIndicator& progress_indicator);

    private:

        GDALRasterBand* _band;

    };


                   GDALRaster          (std::string const& dataset_name);

                   GDALRaster          (GDALDatasetPtr dataset);

                   GDALRaster          (GDALRaster const& other)=default;

                   GDALRaster          (GDALRaster&& other)=default;

                   ~GDALRaster         ()=default;

   GDALRaster&     operator=           (GDALRaster const& other)=default;

   GDALRaster&     operator=           (GDALRaster&& other)=default;

   Band            band                (int nr) const;

   hl::RasterDomain const&
                   domain              () const;

   hl::RasterDiscretization const&
                   discretization      () const;

   std::size_t     nr_bands            () const;

private:

   void            init                ();

   GDALDatasetPtr  _dataset;

   // std::string     _dataset_name;

   hl::RasterDomain _domain;

   hl::RasterDiscretization _discretization;

};

}  // namespace utility
}  // namespace lue
