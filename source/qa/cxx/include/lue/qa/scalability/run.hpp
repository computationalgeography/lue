#pragma once
#include "lue/qa/scalability/stopwatch.hpp"


namespace lue::qa {

    class Run
    {

        public:

            Run();

            Run(Run const&) = default;

            Run(Run&&) = default;

            virtual ~Run() = default;

            Run& operator=(Run const&) = default;

            Run& operator=(Run&&) = default;

            void start();

            void stop();

            Stopwatch::SystemTimePoint const& start_time_point() const;

            template<typename ToDuration>
            ToDuration duration() const
            {
                return _stopwatch.duration<ToDuration>();
            }

        private:

            Stopwatch _stopwatch;
    };

}  // namespace lue::qa
