#include "lue/hl/raster_stack_discretization.hpp"
// #include <algorithm>
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
    auto const& time_series_shape = _time_series_discretization.shape();
    auto const& raster_shape = _raster_discretization.shape();

    assert(time_series_shape.size() == 1);
    assert(raster_shape.size() == 2);
    assert(_shape.size() == 3);

    _shape[0] = time_series_shape[0];
    _shape[1] = raster_shape[0];
    _shape[2] = raster_shape[1];

    // auto destination = std::copy(
    //     time_series_shape.begin(), time_series_shape.end(), _shape.begin());
    // std::copy(raster_shape.begin(), raster_shape.end(), destination);
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


bool RasterStackDiscretization::operator==(
    RasterStackDiscretization const& other) const
{
    return
        _time_series_discretization == other._time_series_discretization &&
        _raster_discretization == other._raster_discretization
        ;
}


bool RasterStackDiscretization::operator<(
    RasterStackDiscretization const& other) const
{
    return
        _time_series_discretization < other._time_series_discretization ||
        _raster_discretization < other._raster_discretization
        ;
}

}  // namespace hl
}  // namespace lue
