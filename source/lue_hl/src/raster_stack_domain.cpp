#include "lue/hl/raster_stack_domain.hpp"


namespace lue {
namespace hl {

RasterStackDomain::RasterStackDomain(
    TimeSeriesDomain const& time_series_domain,
    RasterDomain const& raster_domain)

    : _time_series_domain{time_series_domain},
      _raster_domain{raster_domain}

{
}


TimeSeriesDomain const& RasterStackDomain::time_series_domain() const
{
    return _time_series_domain;
}


RasterDomain const& RasterStackDomain::raster_domain() const
{
    return _raster_domain;
}


bool operator==(
    RasterStackDomain const& lhs,
    RasterStackDomain const& rhs)
{
    return
        lhs.time_series_domain() == rhs.time_series_domain() &&
        lhs.raster_domain() == rhs.raster_domain()
        ;
}


bool operator<(
    RasterStackDomain const& lhs,
    RasterStackDomain const& rhs)
{
    return
        lhs.time_series_domain() < rhs.time_series_domain() ||
        lhs.raster_domain() < rhs.raster_domain()
        ;
}

}  // namespace hl
}  // namespace lue
