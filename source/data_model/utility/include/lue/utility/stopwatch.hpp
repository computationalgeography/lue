#pragma once
#include <chrono>
#include <string>


namespace lue {
    namespace utility {

        class Stopwatch
        {

            public:

                using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

                Stopwatch() = default;

                Stopwatch(Stopwatch const&) = default;

                Stopwatch(Stopwatch&&) = default;

                ~Stopwatch() = default;

                Stopwatch& operator=(Stopwatch const&) = default;

                Stopwatch& operator=(Stopwatch&&) = default;

                void start();

                void stop();

                TimePoint const& start() const;

                TimePoint const& end() const;

                double elapsed_seconds() const;

            private:

                TimePoint _start;

                TimePoint _end;
        };


        std::string to_string(Stopwatch::TimePoint const& time_point);

    }  // namespace utility
}  // namespace lue
