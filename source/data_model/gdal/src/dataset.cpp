#include "lue/gdal/dataset.hpp"
#include "lue/gdal/driver.hpp"
#include <fmt/format.h>
#include <cassert>
#include <stdexcept>


namespace lue::gdal {

    /*!
        @brief      Open dataset @name
        @param      open_mode Open mode to use: `GDALAccess::GA_ReadOnly` or `GDALAccess::GA_Update`
        @exception  std::runtime_error In case the dataset cannot be opened
    */
    auto open_dataset(std::string const& name, GDALAccess open_mode) -> DatasetPtr
    {
        register_gdal_drivers();

#ifndef NDEBUG
        CPLPushErrorHandler(CPLQuietErrorHandler);
#endif

        DatasetPtr dataset_ptr{static_cast<GDALDataset*>(::GDALOpen(name.c_str(), open_mode)), gdal_close};

#ifndef NDEBUG
        CPLPopErrorHandler();
#endif

        if (!dataset_ptr)
        {
            throw std::runtime_error("Raster " + name + " cannot be opened");
        }

        return dataset_ptr;
    }


    /*!
        @brief      Create a new dataset, called @name
        @param      shape Shape of the raster bands in the dataset
        @param      nr_bands Number of bands to add to the dataset
        @param      data_type Type of the elements in the raster band(s)
        @exception  std::runtime_error In case the dataset cannot be created
    */
    auto create_dataset(std::string const& name, Shape const& shape, Count nr_bands, GDALDataType data_type)
        -> DatasetPtr
    {
        register_gdal_drivers();

#ifndef NDEBUG
        CPLPushErrorHandler(CPLQuietErrorHandler);
#endif

        // TODO let GDAL pick the driver and/or use extension(?)
        DriverPtr driver{gdal::driver("GTiff")};

        DatasetPtr dataset_ptr{
            driver->Create(name.c_str(), shape[1], shape[0], nr_bands, data_type, nullptr), gdal_close};

#ifndef NDEBUG
        CPLPopErrorHandler();
#endif

        if (!dataset_ptr)
        {
            throw std::runtime_error("Raster " + name + " cannot be created");
        }

        return dataset_ptr;
    }


    /*!
        @brief      Create a copy of a dataset
        @param      name Name of the new dataset
        @param      clone_dataset Dataset to copy
        @exception  std::runtime_error In case the dataset cannot be created
    */
    auto create_copy(std::string const& name, DatasetPtr& clone_dataset) -> DatasetPtr
    {
        assert(clone_dataset);

        register_gdal_drivers();

#ifndef NDEBUG
        CPLPushErrorHandler(CPLQuietErrorHandler);
#endif

        // TODO let GDAL pick the driver and/or use extension(?)
        DriverPtr driver{gdal::driver("GTiff")};

        DatasetPtr dataset_ptr{
            driver->CreateCopy(name.c_str(), clone_dataset.get(), FALSE, nullptr, nullptr, nullptr),
            gdal_close};

#ifndef NDEBUG
        CPLPopErrorHandler();
#endif

        if (!dataset_ptr)
        {
            throw std::runtime_error("Raster " + name + " cannot be created");
        }

#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 2, 0)
        // The statistics of the clone raster are written to the new raster, which is
        // very inconvenient
        dataset_ptr->ClearStatistics();
#endif

        return dataset_ptr;
    }


    /*!
        @brief      Return a raster band from the @a dataset passed in
        @param      band_nr Number of band to return. Must be larger than zero.
        @exception  std::runtime_error In case the band cannot be obtained
    */
    auto raster_band(::GDALDataset& dataset, Count const band_nr) -> RasterBandPtr
    {
        RasterBandPtr band_ptr{dataset.GetRasterBand(band_nr)};

        if (band_ptr == nullptr)
        {
            throw std::runtime_error(fmt::format("Band {} cannot be obtained", band_nr));
        }

        return band_ptr;
    }


    /*!
        @overload
        @brief      Return the first raster band.
    */
    auto raster_band(::GDALDataset& dataset) -> RasterBandPtr
    {
        return raster_band(dataset, 1);
    }


    /*!
        @overload
        @brief      Return the datatype of the elements in the dataset
    */
    auto data_type(GDALDataset& dataset) -> GDALDataType
    {
        RasterBandPtr band_ptr{raster_band(dataset)};

        return data_type(*band_ptr);
    }


    /*!
        @overload
        @brief      Return the datatype of the elements in the dataset named @a name
    */
    auto data_type(std::string const& name) -> GDALDataType
    {
        DatasetPtr dataset_ptr{open_dataset(name, GDALAccess::GA_ReadOnly)};

        return data_type(*dataset_ptr);
    }


    /*!
        @brief      Return the shape of the rasters in the @a dataset
    */
    auto shape(GDALDataset& dataset) -> Shape
    {
        RasterBandPtr band_ptr{raster_band(dataset)};

        return {band_ptr->GetYSize(), band_ptr->GetXSize()};
    }

}  // namespace lue::gdal
