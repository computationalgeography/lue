#include "lue/framework/io/gdal.hpp"
#include <stdexcept>


namespace lue {

    void register_gdal_drivers()
    {
        static bool drivers_registered{false};

        if(!drivers_registered)
        {
            ::GDALAllRegister();
            drivers_registered = true;
        }
    }


    GDALDatasetPtr open_dataset(
        std::string const& name,
        GDALAccess const open_mode)
    {
        register_gdal_drivers();

#ifndef NDEBUG
        CPLPushErrorHandler(CPLQuietErrorHandler);
#endif

        GDALDatasetPtr dataset_ptr{(GDALDataset *)::GDALOpen(name.c_str(), open_mode), gdal_close};

#ifndef NDEBUG
        CPLPopErrorHandler();
#endif

        if(!dataset_ptr)
        {
            throw std::runtime_error("Raster " + name + " cannot be opened");
        }

        return dataset_ptr;
    }


    GDALDriverPtr driver(
        std::string const& name)
    {
        GDALDriverPtr driver_ptr{GetGDALDriverManager()->GetDriverByName(name.c_str())};

        if(!driver_ptr)
        {
            throw std::runtime_error("Driver " + name + " not available");
        }

        return driver_ptr;
    }


    GDALDatasetPtr create_copy(
        std::string const& name,
        GDALDatasetPtr& clone_dataset)
    {
        register_gdal_drivers();

#ifndef NDEBUG
        CPLPushErrorHandler(CPLQuietErrorHandler);
#endif

        // TODO let GDAL pick the driver and/or use extension(?)
        GDALDriverPtr driver{lue::driver("GTiff")};

        GDALDatasetPtr dataset_ptr{
                driver->CreateCopy(name.c_str(), clone_dataset.get(), FALSE, nullptr, nullptr, nullptr),
                gdal_close
            };

#ifndef NDEBUG
        CPLPopErrorHandler();
#endif

        if(!dataset_ptr)
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


    GDALDatasetPtr create(
        std::string const& name,
        Shape<Count, 2> const& shape,
        Count const nr_bands,
        GDALDataType const data_type)
    {
        register_gdal_drivers();

#ifndef NDEBUG
        CPLPushErrorHandler(CPLQuietErrorHandler);
#endif

        // TODO let GDAL pick the driver and/or use extension(?)
        GDALDriverPtr driver{lue::driver("GTiff")};

        GDALDatasetPtr dataset_ptr{
                driver->Create(name.c_str(), shape[1], shape[0], nr_bands, data_type, nullptr),
                gdal_close
            };

#ifndef NDEBUG
        CPLPopErrorHandler();
#endif

        if(!dataset_ptr)
        {
            throw std::runtime_error("Raster " + name + " cannot be created");
        }

        return dataset_ptr;
    }


    GDALBandPtr get_raster_band(
        GDALDataset& dataset)
    {
        GDALBandPtr band_ptr{dataset.GetRasterBand(1)};

        if(!band_ptr)
        {
            throw std::runtime_error("First band cannot be obtained");
        }

        return band_ptr;
    }


    GDALDataType data_type(
        GDALRasterBand& band)
    {
        return band.GetRasterDataType();
    }


    GDALDataType data_type(
        GDALDataset& dataset)
    {
        GDALBandPtr band_ptr{get_raster_band(dataset)};

        return data_type(*band_ptr);
    }


    GDALDataType data_type(
        std::string const& name)
    {
        GDALDatasetPtr dataset_ptr{open_dataset(name, GDALAccess::GA_ReadOnly)};

        return data_type(*dataset_ptr);
    }

}  // namespace lue
