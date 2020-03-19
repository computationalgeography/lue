#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/focal_mean.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/algorithm/uniform.hpp"
#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include <hpx/include/iostreams.hpp>


namespace lue {
namespace benchmark {
namespace detail {

template<
    typename Element,
    std::size_t rank>
void focal_mean(
    Task const& task,
    std::size_t const max_tree_depth)
{
    assert(max_tree_depth > 0);

    using Array = PartitionedArray<Element, rank>;
    using Shape = typename Array::Shape;

    Shape shape;
    std::copy(
        task.array_shape().begin(), task.array_shape().end(),
        shape.begin());

    Shape partition_shape;
    std::copy(
        task.partition_shape().begin(), task.partition_shape().end(),
        partition_shape.begin());

    // → Create initial array
    Array state{shape, partition_shape};
    hpx::cout << describe(state) << hpx::endl;

    assert(state.shape() == shape);

    state = uniform(state, Element{0}, std::numeric_limits<Element>::max());

    auto const kernel = lue::box_kernel<bool, rank>(1, true);

    hpx::lcos::local::sliding_semaphore semaphore{
        static_cast<std::int64_t>(max_tree_depth)};

    for(std::size_t i = 0; i < task.nr_time_steps(); ++i) {

        // Wait if there are more than max_tree_depth iterations in flight
        semaphore.wait(i);

        state = focal_mean(state, kernel);

        hpx::cout << '.' << hpx::flush;

        // Attach a continuation to the state at the current time
        // step. Once it is finished, signal the semaphore so it knowns
        // that we can have another iteration in flight.
        hpx::when_all_n(state.begin(), state.nr_partitions()).then(
            hpx::launch::sync,
            [&semaphore, i](
                auto const&)
            {
                semaphore.signal(i);
            });
    }

    hpx::cout << "!" << hpx::flush;

    hpx::wait_all_n(state.begin(), state.nr_partitions());

    hpx::cout << hpx::endl;
}

}  // namespace detail


void focal_mean(
    Task const& task,
    std::size_t const max_tree_depth)
{
    using Element = double;

    switch(task.rank()) {
        case 2: {
            detail::focal_mean<Element, 2>(task, max_tree_depth);
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

        lue::benchmark::focal_mean(task, max_tree_depth);
    };

    return lue::benchmark::Benchmark{std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
