#include "lue/hl/raster_domain.hpp"


namespace lue {
namespace hl {

RasterDomain::RasterDomain()

    : _crs{},
      _coordinates{}

{
}


RasterDomain::RasterDomain(
    std::string const& crs,
    Coordinate const west,
    Coordinate const south,
    Coordinate const east,
    Coordinate const north)

    : _crs{crs},
      _coordinates{{west, south, east, north}}

{
}


RasterDomain::RasterDomain(
    std::string const& crs,
    Coordinates&& coordinates)

    : _crs{crs},
      _coordinates{std::forward<Coordinates>(coordinates)}

{
}


std::string const& RasterDomain::crs() const
{
    return _crs;
}


RasterDomain::Coordinate RasterDomain::west() const
{
    return _coordinates[0];
}


RasterDomain::Coordinate RasterDomain::south() const
{
    return _coordinates[1];
}


RasterDomain::Coordinate RasterDomain::east() const
{
    return _coordinates[2];
}


RasterDomain::Coordinate RasterDomain::north() const
{
    return _coordinates[3];
}


RasterDomain::Coordinates const& RasterDomain::coordinates() const
{
    return _coordinates;
}


bool operator==(
    RasterDomain const& lhs,
    RasterDomain const& rhs)
{
    return
        lhs.crs() == rhs.crs() &&
        lhs.coordinates() == rhs.coordinates()
        ;
}


bool operator<(
    RasterDomain const& lhs,
    RasterDomain const& rhs)
{
    return
        lhs.crs() < rhs.crs() ||
        lhs.coordinates() < rhs.coordinates()
        ;
}

}  // namespace hl
}  // namespace lue
