#include "lue/qa/scalability/run.hpp"


namespace lue::qa {

    Run::Run()
    {
    }


    void Run::start()
    {
        _stopwatch.start();
    }


    void Run::stop()
    {
        _stopwatch.stop();
    }


    Stopwatch::SystemTimePoint const& Run::start_time_point() const
    {
        return _stopwatch.start_time_point();
    }

}  // namespace lue::qa
