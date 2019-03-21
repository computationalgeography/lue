#include "copy.hpp"
#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"


auto setup_benchmark(
        int /* argc */,
        char* /* argv */[],
        lue::benchmark::Environment const& environment)
{
    // This function is called on the main locality. Return a callable that
    // performs the work to benchmark.

    std::string const name = "copy";
    std::string const cluster_name = "eejit";
    std::string const description = "copy algorithm benchmark";

    // Function to benchmark
    auto callable = [](
        lue::benchmark::Environment const& /* environment */)
    {
        std::uint64_t const max_tree_depth =
            lue::required_configuration_entry<std::uint64_t>("max_tree_depth");
        std::uint64_t const nr_time_steps =
            lue::required_configuration_entry<std::uint64_t>("nr_time_steps");
        std::uint64_t const nr_rows =
            lue::required_configuration_entry<std::uint64_t>("nr_rows");
        std::uint64_t const nr_cols =
            lue::required_configuration_entry<std::uint64_t>("nr_cols");
        std::uint64_t const nr_rows_partition =
            lue::required_configuration_entry<std::uint64_t>(
                "nr_rows_partition");
        std::uint64_t const nr_cols_partition =
            lue::required_configuration_entry<std::uint64_t>(
                "nr_cols_partition");

        lue::benchmark::copy(
            max_tree_depth,
            nr_time_steps, nr_rows, nr_cols,
            nr_rows_partition, nr_cols_partition);
    };

    return lue::benchmark::Benchmark{
        std::move(callable), cluster_name, environment, name, description};
}


LUE_CONFIGURE_HPX_BENCHMARK()
