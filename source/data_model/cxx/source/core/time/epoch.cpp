#include "lue/core/time/epoch.hpp"
#include <cassert>


namespace lue::data_model::time {

    /*!
        @brief      Default construct an instance

        The epoch kind is set to Kind::common_era, the calendar to Calendar::gregorian, and the offset to
        midnight, Jan 1st 1970. Time points are assumed to be offset by this origin. So, a time point of one
        year means midnight, Jan 1st 1971.

        This default constructed epoch is the same as used in Unix time.
    */
    Epoch::Epoch():

        // TODO: Create a time point instance representing Unix time epoch and convert it to a string using a
        //       utility function read converting between time points and strings.

        Epoch{Kind::common_era, "1970-01-01T00:00:00+00:00", Calendar::gregorian}

    {
    }


    /*!
        @brief      Construct an instance
        @param      kind Epoch kind

        The calendar and offset are not set.
    */
    Epoch::Epoch(Kind kind):

        _kind{kind},
        _origin{},
        _calendar{}

    {
        assert(!_origin && !_calendar);
    }


    /*!
        @brief      Construct an instance
        @param      kind Epoch kind
        @param      origin Offset from epoch kind's epoch

        The calendar is not set.
    */
    Epoch::Epoch(Kind const kind, std::string const& origin):

        _kind{kind},
        _origin{origin},
        _calendar{}

    {
        assert(_origin && !_calendar);
    }


    /*!
        @brief      Construct an instance
        @param      kind Epoch kind
        @param      origin Offset from epoch kind's epoch
        @param      calendar Calendar of time points
    */
    Epoch::Epoch(Kind const kind, std::string const& origin, Calendar const calendar):

        _kind{kind},
        _origin{origin},
        _calendar{calendar}

    {
        assert(_origin && _calendar);
    }


    auto Epoch::kind() const -> Epoch::Kind
    {
        return _kind;
    }


    auto Epoch::origin() const -> std::optional<std::string> const&
    {
        return _origin;
    }


    auto Epoch::calendar() const -> std::optional<Calendar> const&
    {
        return _calendar;
    }

}  // namespace lue::data_model::time
