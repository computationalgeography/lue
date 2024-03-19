#include "lue/gdal/driver.hpp"
#include <fmt/format.h>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <map>
#include <stdexcept>


namespace lue::gdal {
    namespace {

        // GDAL driver name mapped by lower-cased file extension. Add to it whenever useful.
        const std::map<std::string, std::string> driver_name_by_extension{
            {".tif", "GTiff"},
            {".hdf4", "HDF4"},
            {".h4", "HDF4"},
            {".hdf5", "HDF5"},
            {".h5", "HDF5"},
            {".nc", "netCDF"},
            {".map", "PCRaster"},
            {".csf", "PCRaster"},
            {".img", "HFA"},  // Erdas Imagine
        };

    }  // namespace

    /*!
        @brief      Register all GDAL drivers

        It is safe to call this function multiple times. The drivers will only be registered
        the first time.
    */
    void register_gdal_drivers()
    {
        static bool drivers_registered{false};

        if (!drivers_registered)
        {
            ::GDALAllRegister();
            drivers_registered = true;
        }
    }


    /*!
        @brief      Return a pointer to the driver with the @name passed in
        @exception  std::runtime_error In case the requested driver is not available
    */
    auto driver(std::string const& name) -> DriverPtr
    {
        DriverPtr driver_ptr{GetGDALDriverManager()->GetDriverByName(name.c_str())};

        if (driver_ptr == nullptr)
        {
            throw std::runtime_error(fmt::format("Driver {} is not available", name));
        }

        return driver_ptr;
    }


    /*!
        @brief      Return the name of the driver that is (likely) able to handle the dataset
    */
    auto driver_name(std::string const& dataset_name) -> std::string
    {
        std::string extension{std::filesystem::path(dataset_name).extension().string()};

        std::transform(
            extension.begin(),
            extension.end(),
            extension.begin(),
            [](unsigned char const character) { return std::tolower(character); });

        std::string result{"Unknown"};

        if (auto it = driver_name_by_extension.find(extension); it != driver_name_by_extension.end())
        {
            result = (*it).second;
        }

        return result;
    }

}  // namespace lue::gdal
