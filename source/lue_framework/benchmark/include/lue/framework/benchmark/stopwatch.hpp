#pragma once
#include <chrono>


namespace lue {
namespace benchmark {

class Stopwatch
{

public:

    using TimePoint = std::chrono::high_resolution_clock::time_point;
    using Duration = std::chrono::high_resolution_clock::duration;

                   Stopwatch           ();

                   Stopwatch           (Stopwatch const&)=default;

                   Stopwatch           (Stopwatch&&)=default;

                   ~Stopwatch          ()=default;

    Stopwatch&     operator=           (Stopwatch const&)=default;

    Stopwatch&     operator=           (Stopwatch&&)=default;

    void           start               ();

    void           stop                ();

    Duration       elapsed             () const;

    template<
        typename ToDuration>
    ToDuration     elapsed             () const;

private:

    TimePoint      _start;

    TimePoint      _stop;

};


template<
    typename ToDuration>
inline ToDuration Stopwatch::elapsed() const
{
    return std::chrono::duration_cast<ToDuration>(elapsed());
}

}  // namespace benchmark
}  // namespace lue
