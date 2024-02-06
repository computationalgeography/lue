#include "lue/gdal/driver.hpp"
#include <fmt/format.h>
#include <stdexcept>


namespace lue::gdal {

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


    auto delete_dataset(GDALDriver& driver, std::string const& dataset_name) -> void
    {
        CPLErr const status = driver.Delete(dataset_name.c_str());

        if (status != CE_None)
        {
            throw std::runtime_error(fmt::format("Cannot delete dataset {}", dataset_name));
        }
    }

}  // namespace lue::gdal
