#include "land_use_change_benchmark_model.hpp"
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
        // std::size_t const max_tree_depth = environment.max_tree_depth()
        //     ? *environment.max_tree_depth()
        //     : task.nr_time_steps();

        if(task.rank() != 2) {
            throw std::runtime_error("rank must be 2");
        }

        // lue::benchmark::change_land_use(task, max_tree_depth);

        return lue::LandUseChangeBenchmarkModel{environment, task};
    };

    return lue::benchmark::Benchmark{std::move(callable), environment, task};
}


// #include "change_land_use.hpp"
// #include "lue/framework/benchmark/benchmark.hpp"
// #include "lue/framework/benchmark/hpx_main.hpp"
//
//
// auto setup_benchmark(
//         int /* argc */,
//         char* /* argv */[],
//         lue::benchmark::Environment const& environment,
//         lue::benchmark::Task const& task)
// {
//     auto callable = [](
//         lue::benchmark::Environment const& environment,
//         lue::benchmark::Task const& task)
//     {
//         std::size_t const max_tree_depth = environment.max_tree_depth()
//             ? *environment.max_tree_depth()
//             : task.nr_time_steps();
//
//         if(task.rank() != 2) {
//             throw std::runtime_error("rank must be 2");
//         }
//
//         lue::benchmark::change_land_use(task, max_tree_depth);
//     };
//
//     return lue::benchmark::Benchmark{std::move(callable), environment, task};
// }


LUE_CONFIGURE_HPX_BENCHMARK()
