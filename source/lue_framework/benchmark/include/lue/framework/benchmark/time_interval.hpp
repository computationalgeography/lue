#pragma once
#include <chrono>


namespace lue {
namespace benchmark {

/*!
    @brief      Class for representing time intervals
*/
class TimeInterval
{

public:

    using TimePoint = std::chrono::high_resolution_clock::time_point;
    using Duration = std::chrono::high_resolution_clock::duration;

                   TimeInterval        ();

                   TimeInterval        (TimePoint const& start,
                                        TimePoint const& stop);

                   TimeInterval        (TimeInterval const&)=default;

                   TimeInterval        (TimeInterval&&)=default;

                   ~TimeInterval       ()=default;

    TimeInterval&  operator=           (TimeInterval const&)=default;

    TimeInterval&  operator=           (TimeInterval&&)=default;

    TimePoint const& start             () const;

    TimePoint const& stop              () const;

    Duration       duration            () const;

    template<
        typename ToDuration>
    ToDuration     durationf            () const;

private:

    TimePoint      _start;

    TimePoint      _stop;

};


/*!
    @brief      Return the duration casted to @a ToDuration

    Use this function to convert the high resolution duration to some
    courser unit, like seconds.
*/
template<
    typename ToDuration>
inline ToDuration TimeInterval::durationf() const
{
    return std::chrono::duration_cast<ToDuration>(duration());
}

}  // namespace benchmark
}  // namespace lue
