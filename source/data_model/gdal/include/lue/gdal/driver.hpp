#pragma once
#include "lue/gdal/export.hpp"
#include <gdal_priv.h>
#include <string>


namespace lue::gdal {

    /*!
        @brief      Type for pointers to GDAL drivers

        Driver pointers are owned by the GDAL library and must not be deleted.
    */
    using DriverPtr = GDALDriver*;


    LUE_GDAL_EXPORT auto register_gdal_drivers() -> void;

    LUE_GDAL_EXPORT auto driver(std::string const& name) -> DriverPtr;

    LUE_GDAL_EXPORT auto driver_name(std::string const& dataset_name) -> std::string;

}  // namespace lue::gdal
