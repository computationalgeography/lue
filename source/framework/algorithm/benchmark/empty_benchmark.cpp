#include "empty.hpp"
#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"


auto setup_benchmark(
        int /* argc */,
        char* /* argv */[],
        lue::benchmark::Environment const& environment,
        lue::benchmark::Task const& task)
{
    auto callable = [](
        lue::benchmark::Environment const& environment,
        lue::benchmark::Task const& task)
    {
        assert(!environment.max_tree_depth());
        lue::benchmark::empty(task);
    };

    return lue::benchmark::Benchmark{std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
