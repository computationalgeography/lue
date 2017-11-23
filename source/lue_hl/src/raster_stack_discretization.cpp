#include "lue/hl/raster_stack_discretization.hpp"
#include <algorithm>
#include <cassert>


namespace lue {
namespace hl {

RasterStackDiscretization::RasterStackDiscretization(
    TimeSeriesDiscretization const& time_series_discretization,
    RasterDiscretization const& raster_discretization)

    : _time_series_discretization{time_series_discretization},
      _raster_discretization{raster_discretization},
      _shape(3)

{
    auto const& time_series_shape{_time_series_discretization.shape()};
    auto const& raster_shape{_raster_discretization.shape()};

    assert(time_series_shape.size() == 1);
    assert(raster_shape.size() == 2);
    assert(_shape.size() == 3);

    auto destination = std::copy(
        time_series_shape.begin(), time_series_shape.end(), _shape.begin());
    std::copy(raster_shape.begin(), raster_shape.end(), destination);
}


TimeSeriesDiscretization const& RasterStackDiscretization::
    time_series_discretization() const
{
    return _time_series_discretization;
}


RasterDiscretization const& RasterStackDiscretization::raster_discretization()
    const
{
    return _raster_discretization;
}


hdf5::Shape const& RasterStackDiscretization::shape() const
{
    return _shape;
}


bool operator==(
    RasterStackDiscretization const& lhs,
    RasterStackDiscretization const& rhs)
{
    return
        lhs.time_series_discretization() == rhs.time_series_discretization() &&
        lhs.raster_discretization() == rhs.raster_discretization()
        ;
}


bool operator<(
    RasterStackDiscretization const& lhs,
    RasterStackDiscretization const& rhs)
{
    return
        lhs.time_series_discretization() < rhs.time_series_discretization() ||
        lhs.raster_discretization() < rhs.raster_discretization()
        ;
}

}  // namespace hl
}  // namespace lue
