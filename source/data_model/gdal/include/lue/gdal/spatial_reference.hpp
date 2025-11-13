#pragma once
#include "lue/gdal/export.hpp"
#include <gdal/ogr_spatialref.h>
#include <string>


namespace lue::gdal {

    class LUE_GDAL_EXPORT SpatialReference
    {

        public:

            static auto from_wkt(std::string const& wkt) -> SpatialReference;

            static auto from_epsg(int code) -> SpatialReference;

            auto is_projected() const -> bool;

            auto to_wkt() const -> std::string;

        private:

            SpatialReference(OGRSpatialReference&& srs);

            OGRSpatialReference _srs;
    };

}  // namespace lue::gdal
