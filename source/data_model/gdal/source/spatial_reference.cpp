#include "lue/gdal/spatial_reference.hpp"


namespace lue::gdal {

    auto SpatialReference::from_wkt(std::string const& wkt) -> SpatialReference
    {
        OGRSpatialReference srs{};

        auto const ogrerr = srs.importFromWkt(wkt.c_str());

        if (ogrerr != OGRERR_NONE)
        {
            throw std::runtime_error("Error while importing from WKT string");
        }

        return srs;
    }


    auto SpatialReference::from_epsg(int const code) -> SpatialReference
    {
        OGRSpatialReference srs{};

        auto const ogrerr = srs.importFromEPSG(code);

        if (ogrerr != OGRERR_NONE)
        {
            throw std::runtime_error("Error while importing from EPSG code");
        }

        return srs;
    }


    SpatialReference::SpatialReference(OGRSpatialReference&& srs):

        _srs{std::move(srs)}

    {
    }


    auto SpatialReference::is_projected() const -> bool
    {
        return _srs.IsProjected() != 0;
    }


    auto SpatialReference::to_wkt() const -> std::string
    {
        // TODO: Use a type which frees in destructor
        char* wkt_ptr{nullptr};
        auto const ogrerr = _srs.exportToWkt(&wkt_ptr);

        if (ogrerr != OGRERR_NONE)
        {
            throw std::runtime_error("Cannot export spatial reference to WKT");
        }

        std::string wkt(wkt_ptr);
        CPLFree(wkt_ptr);

        return wkt;
    }

}  // namespace lue::gdal
