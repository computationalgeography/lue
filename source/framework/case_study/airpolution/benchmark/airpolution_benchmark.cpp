#include "polute_air.hpp"
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

        using Shape = lue::Shape<lue::Count, 2>;

        Shape array_shape;
        Shape partition_shape;

        std::copy(task.array_shape().begin(), task.array_shape().end(),
            array_shape.begin());
        std::copy(task.partition_shape().begin(), task.partition_shape().end(),
            partition_shape.begin());

        assert(!environment.max_tree_depth());
        lue::polute_air(
            task.nr_time_steps(),
            array_shape, partition_shape);
    };

    return lue::benchmark::Benchmark{std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
