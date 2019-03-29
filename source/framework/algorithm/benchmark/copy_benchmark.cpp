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

    // std::string const name = "copy";
    // std::string const description =
    //     "copy algorithm benchmark\n"
    //     "\n"
    //     "initial state: allocate a distributed array\n"
    //     "iteration    : copy current array to new array\n"
    //     ;

    // Function to benchmark
    auto callable = [](
        lue::benchmark::Environment const& environment,
        lue::benchmark::Task const& task)
    {
        // std::uint64_t const max_tree_depth =
        //     lue::required_configuration_entry<std::uint64_t>("max_tree_depth");
        // std::uint64_t const nr_time_steps =
        //     lue::required_configuration_entry<std::uint64_t>("nr_time_steps");
        // std::uint64_t const nr_rows =
        //     lue::required_configuration_entry<std::uint64_t>("nr_rows");
        // std::uint64_t const nr_cols =
        //     lue::required_configuration_entry<std::uint64_t>("nr_cols");
        // std::uint64_t const nr_rows_partition =
        //     lue::required_configuration_entry<std::uint64_t>(
        //         "nr_rows_partition");
        // std::uint64_t const nr_cols_partition =
        //     lue::required_configuration_entry<std::uint64_t>(
        //         "nr_cols_partition");

        lue::benchmark::copy(environment.max_tree_depth(), task);
    };

    return lue::benchmark::Benchmark{
        std::move(callable), environment, task};  // , name, description};
}


LUE_CONFIGURE_HPX_BENCHMARK()
