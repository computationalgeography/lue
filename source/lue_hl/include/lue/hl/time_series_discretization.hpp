#pragma once
#include <hdf5.h>
#include <array>


namespace lue {
namespace hl {

class TimeSeriesDiscretization
{

public:

    using Shape = std::array<hsize_t, 1>;

                   TimeSeriesDiscretization()=default;

                   TimeSeriesDiscretization(
                                        Shape&& shape);

                   TimeSeriesDiscretization(
                                        hsize_t nr_steps);

                   TimeSeriesDiscretization(
                                        TimeSeriesDiscretization const&)
                                            =default;

                   TimeSeriesDiscretization(
                                        TimeSeriesDiscretization&&)=default;

                   ~TimeSeriesDiscretization()=default;

    TimeSeriesDiscretization& operator=(TimeSeriesDiscretization const&)
                                            =default;

    TimeSeriesDiscretization& operator=(TimeSeriesDiscretization&&)=default;

    Shape const&   shape               () const;

    hsize_t        nr_steps            () const;

private:

    Shape          _shape;

};


bool               operator==          (TimeSeriesDiscretization const& lhs,
                                        TimeSeriesDiscretization const& rhs);

bool               operator<           (TimeSeriesDiscretization const& lhs,
                                        TimeSeriesDiscretization const& rhs);

}  // namespace hl
}  // namespace lue
