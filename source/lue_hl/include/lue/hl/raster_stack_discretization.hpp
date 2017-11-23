#pragma once
#include "lue/hl/raster_discretization.hpp"
#include "lue/hl/time_series_discretization.hpp"
#include "lue/hdf5/shape.hpp"


namespace lue {
namespace hl {

class RasterStackDiscretization
{

public:

    // // [nr_time_steps, nr_rows, nr_cols]
    // using Shape = std::array<hsize_t, 3>;

                   RasterStackDiscretization()=default;

                   RasterStackDiscretization(
                    TimeSeriesDiscretization const& time_series_discretization,
                    RasterDiscretization const& raster_discretization);

                   RasterStackDiscretization(
                                    RasterStackDiscretization const&)=default;

                   RasterStackDiscretization(
                                    RasterStackDiscretization&&)=default;

                   ~RasterStackDiscretization()=default;

    RasterStackDiscretization& operator=(
                                    RasterStackDiscretization const&)=default;

    RasterStackDiscretization& operator=(
                                    RasterStackDiscretization&&)=default;

    bool           operator==          (RasterStackDiscretization const& other)
                                            const;

    bool           operator<           (RasterStackDiscretization const& other)
                                            const;

    TimeSeriesDiscretization const& time_series_discretization() const;

    RasterDiscretization const& raster_discretization() const;

    hdf5::Shape const& shape           () const;

private:

    TimeSeriesDiscretization _time_series_discretization;

    RasterDiscretization _raster_discretization;

    hdf5::Shape    _shape;

};

}  // namespace hl
}  // namespace lue
