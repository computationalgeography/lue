#include "lue/hl/time_series_domain.hpp"


namespace lue {
namespace hl {

TimeSeriesDomain::TimeSeriesDomain()

    : _clock{time::Unit::second, 1},
      _coordinates{{0, 0}}

{
}


TimeSeriesDomain::TimeSeriesDomain(
    Clock const& clock,
    Coordinate const start,
    Coordinate const end)

    : _clock{clock},
      _coordinates{{start, end}}

{
}


TimeSeriesDomain::TimeSeriesDomain(
    Clock const& clock,
    Coordinates&& coordinates)

    : _clock{clock},
      _coordinates{std::forward<Coordinates>(coordinates)}

{
}


Clock const& TimeSeriesDomain::clock() const
{
    return _clock;
}


TimeSeriesDomain::Coordinates const& TimeSeriesDomain::coordinates() const
{
    return _coordinates;
}


bool operator==(
    TimeSeriesDomain const& lhs,
    TimeSeriesDomain const& rhs)
{
    return
        lhs.clock() == rhs.clock() &&
        lhs.coordinates() == rhs.coordinates()
        ;
}


bool operator<(
    TimeSeriesDomain const& lhs,
    TimeSeriesDomain const& rhs)
{
    return
        lhs.clock() < rhs.clock() ||
        lhs.coordinates() < rhs.coordinates()
        ;
}

}  // namespace hl
}  // namespace lue
