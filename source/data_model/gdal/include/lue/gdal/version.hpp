#pragma once
#include <string>


namespace lue::gdal {

    /*!
        @brief      Return the version of the GDAL library
    */
    auto version() -> std::string;

}  // namespace lue::gdal
