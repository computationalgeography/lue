#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/copy.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/iterate_per_element.hpp"
#include "lue/framework/algorithm/uniform.hpp"
#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"

#include <hpx/include/iostreams.hpp>
#include <hpx/include/parallel_execution.hpp>


namespace lue {
namespace benchmark {
namespace detail {

template<
    typename Element,
    std::size_t rank>
void iterate_per_element(
    Task const& task,
    std::size_t max_tree_depth)
{
// hpx::cout
//     << "thread("
//         << hpx::this_thread::get_id() << '!'
//         << hpx::threads::get_thread_priority_name(
//             hpx::this_thread::get_priority())
//     << ")\n";

    // assert(
    //     hpx::threads::get_thread_priority() ==
    //     hpx::threads::thread_priority_high);

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

    // Fill array with random numbers
    // hpx::shared_future<Element> min_nr_iterations =
    //     hpx::make_ready_future<Element>(20);
    hpx::shared_future<Element> max_nr_iterations =
        hpx::make_ready_future<Element>(50);

    // uniform(state, min_nr_iterations, max_nr_iterations).wait();

    fill(state, max_nr_iterations).wait();

    // // TODO Create an array of states
    // // TODO Let algorithms spawn tasks in parallel? On high-priority
    // //     threads?
    // Array state1 = copy(state);
    // Array state2 = copy(state);
    // Array state3 = copy(state);
    // Array state4 = copy(state);

    // std::vector<Array> states;
    // states.push_back(state);

    if(max_tree_depth == 0) {
        max_tree_depth = task.nr_time_steps();
    }
    assert(max_tree_depth > 1);
    hpx::lcos::local::sliding_semaphore semaphore{
        static_cast<std::int64_t>(max_tree_depth - 1)};

    for(std::size_t i = 0; i < task.nr_time_steps(); ++i) {

        state = iterate_per_element(state);

        // state = iterate_per_element(state);
        // state = iterate_per_element(state);

        // states.push_back(iterate_per_element(states.back()));

        // state1 = iterate_per_element(state1);
        // state2 = iterate_per_element(state2);
        // state3 = iterate_per_element(state3);
        // state4 = iterate_per_element(state4);

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

        semaphore.wait(i);
    }

    hpx::cout << "!" << hpx::flush;

    hpx::wait_all_n(state.begin(), state.nr_partitions());

    // hpx::wait_all_n(states.back().begin(), states.back().nr_partitions());

    // hpx::wait_all_n(state1.begin(), state1.nr_partitions());
    // hpx::wait_all_n(state2.begin(), state2.nr_partitions());
    // hpx::wait_all_n(state3.begin(), state3.nr_partitions());
    // hpx::wait_all_n(state4.begin(), state4.nr_partitions());

    hpx::cout << hpx::endl;
}

}  // namespace detail


void iterate_per_element(
    Task const& task,
    std::size_t const max_tree_depth)
{
    // TODO Not sure if this helps
    hpx::parallel::execution::default_executor high_priority_executor{
        hpx::threads::thread_priority_high};

    using Element = std::int32_t;

    switch(task.rank()) {
        case 2: {
            // detail::iterate_per_element<Element, 2>(task, max_tree_depth);

            hpx::async(
                high_priority_executor,
                &detail::iterate_per_element<Element, 2>,
                task, max_tree_depth).wait();

            // hpx::async(
            //     high_priority_executor,
            //     &detail::iterate_per_element<Element, 2>,
            //     task).wait();

            // auto iteration1 = hpx::async(
            //     &detail::iterate_per_element<Element, 2>,
            //     task);
            // auto iteration2 = hpx::async(
            //     &detail::iterate_per_element<Element, 2>,
            //     task);
            // hpx::wait_all(iteration1, iteration2);

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
        // assert(!environment.max_tree_depth());

        std::size_t max_tree_depth = 0;

        if(environment.max_tree_depth()) {
            max_tree_depth = *environment.max_tree_depth();
        }

        lue::benchmark::iterate_per_element(task, max_tree_depth);

        // lue::benchmark::iterate_per_element(task);
    };

    return lue::benchmark::Benchmark{std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
