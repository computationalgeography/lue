#pragma once
#include "lue/qa/scalability/instrument/stopwatch.hpp"


namespace lue::qa {

    /*!
        @brief      The Run class represents runs: information about how much time executing
                    a piece of code takes
        @sa         Stopwatch

        Run instances can be started (@a start()) and stopped (@a stop()), after which the
        duration between the start and stop can be queried (@a duration()).
    */
    class Run
    {

        public:

            Run() = default;

            Run(Run const&) = default;

            Run(Run&&) = default;

            virtual ~Run() = default;

            auto operator=(Run const&) -> Run& = default;

            auto operator=(Run&&) -> Run& = default;

            void start();

            void stop();

            [[nodiscard]] auto start_time_point() const -> Stopwatch::SystemTimePoint const&;

            /*!
                @brief      Return the duration between the start and end time points
                @tparam     ToDuration Units to cast the underlying high resolution duration to,
                            like seconds
            */
            template<typename ToDuration>
            auto duration() const -> ToDuration
            {
                return _stopwatch.duration<ToDuration>();
            }

        private:

            //! Stopwatch used to keep track of the start and stop time points
            Stopwatch _stopwatch;
    };

}  // namespace lue::qa
