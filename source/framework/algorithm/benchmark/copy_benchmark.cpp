#include "lue/framework/algorithm/copy.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include <hpx/iostream.hpp>


namespace lue {
namespace benchmark {
namespace detail {

template<
    typename Element,
    std::size_t rank>
void copy(
    Task const& task,
    std::size_t const max_tree_depth)
{
    lue_assert(max_tree_depth > 0);

    using Array = PartitionedArray<Element, rank>;
    using Shape = typename Array::Shape;

    // Allocate array for initial state. The array is uninitialized,
    // which is fine.
    Shape shape;
    std::copy(
        task.array_shape().begin(), task.array_shape().end(),
        shape.begin());

    Shape partition_shape;
    std::copy(
        task.partition_shape().begin(), task.partition_shape().end(),
        partition_shape.begin());

    // → Create initial array
    Array state{create_partitioned_array<Element>(shape, partition_shape)};
    hpx::cout << describe(state) << std::endl;

    lue_assert(state.shape() == shape);

    hpx::sliding_semaphore semaphore{
        static_cast<std::int64_t>(max_tree_depth)};

    for(std::size_t i = 0; i < task.nr_time_steps(); ++i) {

        semaphore.wait(i);

        // A very simple model: the new state is a copy of the current
        // state...
        state = copy(state);

        hpx::cout << '.' << std::flush;

        hpx::when_all_n(state.begin(), state.nr_partitions()).then(
            hpx::launch::sync,
            [&semaphore, i](
                auto const&)
            {
                semaphore.signal(i);
            });
    }

    hpx::cout << "!" << std::flush;

    hpx::wait_all_n(state.begin(), state.nr_partitions());

    hpx::cout << std::endl;
}

}  // namespace detail


void copy(
    Task const& task,
    std::size_t const max_tree_depth)
{
    using Element = std::int32_t;

    switch(task.rank()) {
        case 2: {
            detail::copy<Element, 2>(task, max_tree_depth);
            break;
        }
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
    // This function is called on the main locality. Return a callable that
    // performs the work to benchmark.

    // Function to benchmark
    auto callable = [](
        lue::benchmark::Environment const& environment,
        lue::benchmark::Task const& task)
    {
        std::size_t const max_tree_depth = environment.max_tree_depth()
            ? *environment.max_tree_depth()
            : task.nr_time_steps();

        lue::benchmark::copy(task, max_tree_depth);
    };

    return lue::benchmark::Benchmark{std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
