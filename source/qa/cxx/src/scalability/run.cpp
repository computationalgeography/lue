#include "lue/qa/scalability/instrument/run.hpp"


namespace lue::qa {

    /*!
        @brief      Start the run, which means storing the current point in time
    */
    void Run::start()
    {
        _stopwatch.start();
    }


    /*!
        @brief      Stop the run, which means storing the current point in time
    */
    void Run::stop()
    {
        _stopwatch.stop();
    }


    /*!
        @brief      Return the time point when @a start() was called
    */
    auto Run::start_time_point() const -> Stopwatch::SystemTimePoint const&
    {
        return _stopwatch.start_time_point();
    }

}  // namespace lue::qa
