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

    bool           operator==          (TimeSeriesDomain const& other) const;

    bool           operator<           (TimeSeriesDomain const& other) const;

    Clock const&   clock               () const;

    Coordinates const&
                   coordinates         () const;

private:

    Clock          _clock;

    Coordinates    _coordinates;

};

}  // namespace hl
}  // namespace lue
