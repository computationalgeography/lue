#pragma once
#include "lue/clock.hpp"
#include <array>


namespace lue {
namespace hl {

class TimeSeriesDomain
{

public:

    using Coordinate = time::DurationCount;
    using Coordinates = std::array<Coordinate, 2>;

                   TimeSeriesDomain    ();

                   TimeSeriesDomain    (TimeSeriesDomain const&)=default;

                   TimeSeriesDomain    (TimeSeriesDomain&&)=default;

                   TimeSeriesDomain    (Clock const& clock,
                                        Coordinate start,
                                        Coordinate end);

                   TimeSeriesDomain    (Clock const& clock,
                                        Coordinates&& coordinates);

                   ~TimeSeriesDomain   ()=default;

    TimeSeriesDomain& operator=        (TimeSeriesDomain const&)=default;

    TimeSeriesDomain& operator=        (TimeSeriesDomain&&)=default;

    Clock const&   clock               () const;

    Coordinates const&
                   coordinates         () const;

private:

    Clock          _clock;

    Coordinates    _coordinates;

};


bool               operator==          (TimeSeriesDomain const& lhs,
                                        TimeSeriesDomain const& rhs);

bool               operator<           (TimeSeriesDomain const& lhs,
                                        TimeSeriesDomain const& rhs);

}  // namespace hl
}  // namespace lue
