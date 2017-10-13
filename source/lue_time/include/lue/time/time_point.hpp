#pragma once


namespace lue {
namespace time {

template<
    typename Clock>
class TimePoint
{

public:

    using Duration = typename Clock::Duration;

                   TimePoint           (Duration const& duration);

                   TimePoint           (TimePoint const&)=default;

                   TimePoint           (TimePoint&&)=default;

                   ~TimePoint          ()=default;

    TimePoint&     operator=           (TimePoint const&)=default;

    TimePoint&     operator=           (TimePoint&&)=default;

    Duration const& duration           () const;

private:

    //! Duration since epoch
    Duration       _duration;

};


template<
    typename Clock>
inline TimePoint<Clock>::TimePoint(
    Duration const& duration)

    : _duration{duration}

{
}


template<
    typename Clock>
inline typename TimePoint<Clock>::Duration const&
    TimePoint<Clock>::duration() const
{
    return _duration;
}

}  // namespace time
}  // namespace lue
