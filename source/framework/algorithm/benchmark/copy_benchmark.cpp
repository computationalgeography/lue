#include "copy.hpp"
#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"


auto setup_benchmark(
        int /* argc */,
        char* /* argv */[],
        lue::benchmark::Environment const& environment,
        lue::benchmark::Task const& task)
{
    // This function is called on the main locality. Return a callable that
    // performs the work to benchmark.

    // Function to benchmark
    auto callable = [](
        lue::benchmark::Environment const& environment,
        lue::benchmark::Task const& task)
    {
        assert(!environment.max_tree_depth());
        lue::benchmark::copy(task);
    };

    return lue::benchmark::Benchmark{std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
