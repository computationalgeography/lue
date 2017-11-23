#include "lue/hl/time_series_discretization.hpp"


namespace lue {
namespace hl {

TimeSeriesDiscretization::TimeSeriesDiscretization(
    Shape&& shape)

    : _shape{std::forward<Shape>(shape)}

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


bool operator==(
    TimeSeriesDiscretization const& lhs,
    TimeSeriesDiscretization const& rhs)
{
    return lhs.shape() == rhs.shape();
}


bool operator<(
    TimeSeriesDiscretization const& lhs,
    TimeSeriesDiscretization const& rhs)
{
    return lhs.shape() < rhs.shape();
}

}  // namespace hl
}  // namespace lue
