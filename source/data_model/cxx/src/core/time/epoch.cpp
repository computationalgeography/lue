#include "lue/core/time/epoch.hpp"
#include <cassert>


namespace lue {
namespace time {

/*!
    @brief      Default construct an instance

    The epoch kind is set to Kind::anno_domini, the calendar to
    Calendar::gregorian, and the offset to midnight, Jan 1st 1970. Time
    points are assumed to be offset by this origin. So, a time point of
    one year means midnight, Jan 1st 1971.

    This default constructed epoch is the same as used in Unix time.
*/
Epoch::Epoch():

    // TODO Create a time point instance representing Unix time epoch
    //      and convert it to a string using a utility function read
    //      converting between time points and strings.
    Epoch{Kind::anno_domini, "1970-01-01T00:00:00+00:00", Calendar::gregorian}

{
}


Epoch::Epoch(
    Kind kind):

    _kind{kind},
    _origin{},
    _calendar{}

{
    assert(!_origin && !_calendar);
}


Epoch::Epoch(
    Kind const kind,
    std::string const& origin):

    _kind{kind},
    _origin{origin},
    _calendar{}

{
    assert(_origin && !_calendar);
}


Epoch::Epoch(
    Kind const kind,
    std::string const& origin,
    Calendar const calendar):

    _kind{kind},
    _origin{origin},
    _calendar{calendar}

{
    assert(_origin && _calendar);
}


Epoch::Kind Epoch::kind() const
{
    return _kind;
}


std::optional<std::string> const& Epoch::origin() const
{
    return _origin;
}


std::optional<Calendar> const& Epoch::calendar() const
{
    return _calendar;
}

}  // namespace time
}  // namespace lue
