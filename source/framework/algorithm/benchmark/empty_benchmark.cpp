#include "lue/framework/algorithm/empty.hpp"
#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include <hpx/synchronization/sliding_semaphore.hpp>


namespace lue {
    namespace benchmark {

        void empty(Task const& task, std::size_t const max_tree_depth)
        {
            lue_assert(max_tree_depth > 0);

            // There is no state to manipulate
            hpx::future<void> state = hpx::make_ready_future<void>();

            hpx::sliding_semaphore semaphore{static_cast<std::int64_t>(max_tree_depth)};

            for (std::size_t i = 0; i < task.nr_time_steps(); ++i)
            {

                semaphore.wait(i);

                // Ask for a new empty state, once the current empty state is
                // ready (which it already is).
                state = lue::empty(state);

                state.then(hpx::launch::sync, [&semaphore, i](auto const&) { semaphore.signal(i); });
            }
        }

    }  // namespace benchmark
}  // namespace lue


auto setup_benchmark(
    int /* argc */,
    char* /* argv */[],
    lue::benchmark::Environment const& environment,
    lue::benchmark::Task const& task)
{
    auto callable = [](lue::benchmark::Environment const& environment, lue::benchmark::Task const& task)
    {
        std::size_t const max_tree_depth =
            environment.max_tree_depth() ? *environment.max_tree_depth() : task.nr_time_steps();

        lue::benchmark::empty(task, max_tree_depth);
    };

    return lue::benchmark::Benchmark{std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
