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
      _coordinates(std::forward<Coordinates>(coordinates))

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


bool TimeSeriesDomain::operator==(
    TimeSeriesDomain const& other) const
{
    return
        _clock == other._clock &&
        _coordinates == other._coordinates
        ;
}


bool TimeSeriesDomain::operator<(
    TimeSeriesDomain const& other) const
{
    return
        _clock < other._clock ||
        _coordinates < other._coordinates
        ;
}

}  // namespace hl
}  // namespace lue
