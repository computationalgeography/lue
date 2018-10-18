#pragma once
#include "lue/framework/benchmark/time_interval.hpp"


namespace lue {
namespace benchmark {

/*!
    @brief      Class for timing things
*/
class Stopwatch
{

public:

    using TimePoint = TimeInterval::TimePoint;
    using Duration = TimeInterval::Duration;

    static TimePoint now               ();

                   Stopwatch           ();

                   Stopwatch           (Stopwatch const&)=default;

                   Stopwatch           (Stopwatch&&)=default;

                   ~Stopwatch          ()=default;

    Stopwatch&     operator=           (Stopwatch const&)=default;

    Stopwatch&     operator=           (Stopwatch&&)=default;

    void           start               ();

    void           stop                ();

    TimeInterval const& time_interval  () const;

    Duration       elapsed             () const;

    template<
        typename ToDuration>
    ToDuration     elapsed             () const;

private:

    TimeInterval   _time_interval;

};


/*!
    @brief      Return the current value of the high resolution clock
*/
inline Stopwatch::TimePoint Stopwatch::now()
{
    return std::chrono::high_resolution_clock::now();
}


inline void Stopwatch::start()
{
    auto const now = Stopwatch::now();
    _time_interval = TimeInterval{now, now};
}


inline void Stopwatch::stop()
{
    auto const now = Stopwatch::now();
    _time_interval.set_stop(now);
}


/*!
    @brief      Return the duration casted to @a ToDuration

    Use this function to convert the high resolution duration to some
    courser unit, like seconds.
*/
template<
    typename ToDuration>
inline ToDuration Stopwatch::elapsed() const
{
    return std::chrono::duration_cast<ToDuration>(elapsed());
}

}  // namespace benchmark
}  // namespace lue
