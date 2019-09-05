#include "empty.hpp"
#include "lue/framework/algorithm/empty.hpp"


namespace lue {
namespace benchmark {

void empty(
    Task const& task)
{
    // There is no state to manipulate
    hpx::future<void> state = hpx::make_ready_future<void>();

    for(std::size_t i = 0; i < task.nr_time_steps(); ++i) {

        // Ask for a new empty state, once the current empty state is
        // ready (which it already is).
        state = lue::empty(state);

    }
}

}  // namespace benchmark
}  // namespace lue
