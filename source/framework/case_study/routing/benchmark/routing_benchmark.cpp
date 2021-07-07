#include "routing_benchmark_model.hpp"
#include "lue/framework/benchmark/algorithm_benchmark_result.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include "lue/framework/benchmark/model_benchmark.hpp"
#include "lue/framework/benchmark/regex.hpp"
#include <docopt/docopt.h>
#include <regex>


namespace {

    std::string const usage = R"(
    usage:
        {} <flow_direction> [--center=<cell>]

    arguments:
        <flow_direction>  Pathname to array: dataset/phenomenon/property_set/layer

    options:
        --center=<cell>   Center of array to select: row_idx, col_idx

    Default center is the cell at the center of the array.
    )";

}  // Anonymous namespace


auto setup_benchmark(
        int argc,
        char* argv[],
        lue::benchmark::Environment const& environment,
        lue::benchmark::Task const& task)
{
    std::map<std::string, docopt::value> arguments{
        docopt::docopt(fmt::format(usage, argv[0]), {argv + 1, argv + argc}, true)};

    std::string const array_pathname{arguments.at("<flow_direction>").asString()};
    std::vector<lue::Index> center_cell;

    if(arguments.find("--center") != arguments.end())
    {
        center_cell = lue::benchmark::parse_idxs(arguments.at("--center").asString());
    }


    auto callable = [array_pathname, center_cell](
        lue::benchmark::Environment const& environment,
        lue::benchmark::Task const& task)
    {
        std::size_t const max_tree_depth = environment.max_tree_depth()
            ? *environment.max_tree_depth()
            : task.nr_time_steps();

        if(task.rank() != 2)
        {
            throw std::runtime_error("rank must be 2");
        }

        return lue::benchmark::RoutingBenchmarkModel{
            environment, task, max_tree_depth, array_pathname, center_cell};
    };

    return lue::benchmark::ModelBenchmark<decltype(callable), lue::benchmark::AlgorithmBenchmarkResult>{
        std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
