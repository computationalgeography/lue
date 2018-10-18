#include "lue/framework/benchmark/timing.hpp"


namespace lue {
namespace benchmark {

Timing::Timing(
    TimeInterval const& time_interval):

    _time_interval{time_interval}

{
}


TimeInterval const& Timing::time_interval() const
{
    return _time_interval;
}

}  // namespace benchmark
}  // namespace lue
