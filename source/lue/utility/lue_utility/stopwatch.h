#pragma once
#include <chrono>
#include <string>


namespace lue {

class Stopwatch
{

public:

    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

                   Stopwatch           ();

                   Stopwatch           (Stopwatch const&)=delete;

                   Stopwatch           (Stopwatch&&)=delete;

                   ~Stopwatch          ()=default;

    Stopwatch&     operator=           (Stopwatch const&)=delete;

    Stopwatch&     operator=           (Stopwatch&&)=delete;

    void           start               ();

    void           stop                ();

    TimePoint const& start             () const;

    TimePoint const& end               () const;

    double         elapsed_seconds     () const;

private:

    TimePoint      _start;

    TimePoint      _end;

};


std::string        to_string           (Stopwatch::TimePoint const& time_point);

}  // namespace lue
