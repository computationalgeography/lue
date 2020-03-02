#include "change_land_use.hpp"
#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"

// #include "lue/framework/core/component/partitioned_array.hpp"
// #include "lue/framework/algorithm.hpp"
// #include <hpx/include/iostreams.hpp>



namespace lue {
namespace benchmark {
namespace detail {

// void land_use_change(
//     Task const& task,
//     std::size_t const max_tree_depth)
// {
//     assert(max_tree_depth > 0);
// 
//     using FloatRaster = PartitionedArray<double, 2>;
//     using IntegerRaster = PartitionedArray<int32_t, 2>;
//     using RasterShape = Shape<Count, 2>;
// 
//     RasterShape raster_shape;
//     std::copy(
//         task.array_shape().begin(), task.array_shape().end(),
//         raster_shape.begin());
// 
//     RasterShape partition_shape;
//     std::copy(
//         task.partition_shape().begin(), task.partition_shape().end(),
//         partition_shape.begin());
// 
//     IntegerRaster land_use{raster_shape, partition_shape};
//     hpx::cout << describe(land_use) << hpx::endl;
// 
//     assert(land_use.shape() == raster_shape);
// 
//     // Initialize land uses
//     land_use = uniform(land_use, 0, 10);
// 
//     // Store initial state in dataset
//     // FIXME
//     // write(land_use, property);
// 
// 
// 
//     hpx::lcos::local::sliding_semaphore semaphore{
//         static_cast<std::int64_t>(max_tree_depth)};
// 
//     for(std::size_t i = 0; i < task.nr_time_steps(); ++i) {
// 
//         semaphore.wait(i);
// 
// 
//         // No model yet...
//         land_use = copy(land_use);
// 
//         // Add temporal state to dataset
//         // FIXME
//         // write(land_use, dataset);
// 
//         hpx::cout << '.' << hpx::flush;
// 
//         // Attach a continuation to the state at the current time
//         // step. Once it is finished, signal the semaphore so it knowns
//         // that we can have another iteration in flight.
//         hpx::when_all_n(land_use.begin(), land_use.nr_partitions()).then(
//             hpx::launch::sync,
//             [&semaphore, i](
//                 auto const&)
//             {
//                 semaphore.signal(i);
//             });
//     }
// 
// 
// 
// 
// 
// 
// 
//     hpx::cout << "!" << hpx::flush;
// 
//     hpx::wait_all_n(land_use.begin(), land_use.nr_partitions());
// 
//     hpx::cout << hpx::endl;
// }

}  // namespace detail


// void land_use_change(
//     Task const& task,
//     std::size_t const max_tree_depth)
// {
//     if(task.rank() != 2) {
//         throw std::runtime_error("rank must be 2");
//     }
// 
//     detail::land_use_change(task, max_tree_depth);
// }

}  // namespace benchmark
}  // namespace lue


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
        std::size_t const max_tree_depth = environment.max_tree_depth()
            ? *environment.max_tree_depth()
            : task.nr_time_steps();

        if(task.rank() != 2) {
            throw std::runtime_error("rank must be 2");
        }

        lue::benchmark::change_land_use(task, max_tree_depth);
    };

    return lue::benchmark::Benchmark{std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
