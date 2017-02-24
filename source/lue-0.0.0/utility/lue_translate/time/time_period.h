#pragma once


namespace lue {

template<
    typename TimePoint,
    typename TimeDuration>
class TimePeriod
{

public:

                   TimePeriod          ()=default;

                   TimePeriod          (TimePoint const& time_point,
                                        TimeDuration const& time_duration);

    TimePoint      time_point          () const;

    TimeDuration   time_duration       () const;

private:

    TimePoint      _time_point;

    TimeDuration   _time_duration;

};


template<
    typename TimePoint,
    typename TimeDuration>
inline TimePeriod<TimePoint, TimeDuration>::TimePeriod(
    TimePoint const& time_point,
    TimeDuration const& time_duration)

    : _time_point{time_point},
      _time_duration{time_duration}

{
}


template<
    typename TimePoint,
    typename TimeDuration>
inline TimePoint TimePeriod<TimePoint, TimeDuration>::time_point() const
{
    return _time_point;
}


template<
    typename TimePoint,
    typename TimeDuration>
inline TimeDuration TimePeriod<TimePoint, TimeDuration>::time_duration() const
{
    return _time_duration;
}

}  // namespace lue
