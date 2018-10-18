#pragma once
#include "lue/framework/benchmark/time_interval.hpp"


namespace lue {
namespace benchmark {

class Timing
{

public:

    explicit       Timing              (TimeInterval const& time_interval);

                   Timing              (Timing const&)=default;

                   Timing              (Timing&&)=default;

                   ~Timing             ()=default;

    Timing&        operator=           (Timing const&)=default;

    Timing&        operator=           (Timing&&)=default;

    TimeInterval const& time_interval  () const;

private:

    TimeInterval   _time_interval;

};

}  // namespace benchmark
}  // namespace lue
