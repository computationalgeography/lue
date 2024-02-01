#pragma once
#include <gdal_priv.h>
#include <string>


namespace lue::gdal {

    /*!
        @brief      Type for pointers to GDAL drivers

        Driver pointers are owned by the GDAL library and must not be deleted.
    */
    using DriverPtr = GDALDriver*;


    auto register_gdal_drivers() -> void;

    auto driver(std::string const& name) -> DriverPtr;

}  // namespace lue::gdal
