#include "lue/hl/time_series_discretization.hpp"


namespace lue {
namespace hl {

TimeSeriesDiscretization::TimeSeriesDiscretization(
    Shape&& shape)

    : _shape(std::forward<Shape>(shape))

{
}


TimeSeriesDiscretization::TimeSeriesDiscretization(
    hsize_t const nr_steps)

    : _shape{{nr_steps}}

{
}


TimeSeriesDiscretization::Shape const& TimeSeriesDiscretization::shape() const
{
    return _shape;
}


hsize_t TimeSeriesDiscretization::nr_steps() const
{
    return _shape[0];
}


bool TimeSeriesDiscretization::operator==(
    TimeSeriesDiscretization const& other) const
{
    return _shape == other._shape;
}


bool TimeSeriesDiscretization::operator<(
    TimeSeriesDiscretization const& other) const
{
    return _shape < other._shape;
}

}  // namespace hl
}  // namespace lue
