#pragma once
#include "lue/framework/benchmark/environment.hpp"
#include "lue/framework/benchmark/format.hpp"
#include <docopt/docopt.h>
// #include <boost/asio/ip/host_name.hpp>
#include <fmt/format.h>
#include <fstream>


namespace lue {
namespace benchmark {
namespace detail {

std::string const usage = R"(
Run a performance benchmark

usage:
    {0} --count=<count> --nr_workers=<nr_workers>
        [--max_tree_depth=<max_tree_depth>] [<output>]
    {0} (-h | --help) | --version

options:
    --count=<count>  Number of times the benchmark must be run
    --nr_workers=<nr_workers>  Number of workers used
    --max_tree_depth=<max_tree_depth>  Maximum depth of task tree
    -h --help        Show this screen

Results will be written to the terminal if no output pathname is provided
)";


/*!
    @brief      Return an object representing the environment in which the
                benchmark is run
*/
Environment create_environment(
    std::map<std::string, docopt::value> const& arguments)
{

    // std::string const hostname = boost::asio::ip::host_name();
    std::size_t const count = arguments.at("--count").asLong();
    std::size_t const nr_workers = arguments.at("--nr_workers").asLong();

    std::optional<std::size_t> max_tree_depth;

    if(arguments.at("--max_tree_depth")) {
        max_tree_depth = arguments.at("--max_tree_depth").asLong();
    }

    // std::size_t const work_size = arguments.at("--work_size").asLong();

    return Environment{count, nr_workers, max_tree_depth};  // , work_size};
}


Task create_task(
    std::map<std::string, docopt::value> const& /* arguments */)
{
    lue_hpx_assert(false);

    // TODO
    std::uint64_t const nr_time_steps{0};
    Task::Shape array_shape{};
    Task::Shape partition_shape{};

    return Task{nr_time_steps, array_shape, partition_shape};
}

} // Namespace detail


template<
    typename Benchmark>
inline int run_benchmark(
    Benchmark& benchmark,
    std::ostream& stream)
{
    int status = benchmark.run();

    if(status != EXIT_SUCCESS) {
        std::cerr << "Running benchmark failed" << std::endl;
        return EXIT_FAILURE;
    }

    stream << format_as_json(benchmark) << "\n";

    return EXIT_SUCCESS;
}


#define LUE_CONFIGURE_BENCHMARK()                                              \
int main(                                                                      \
    int argc,                                                                  \
    char* argv[])                                                              \
{                                                                              \
    std::map<std::string, docopt::value> arguments =                           \
        docopt::docopt(                                                        \
            fmt::format(lue::benchmark::detail::usage, argv[0]),               \
            {argv + 1, argv + argc});                                          \
                                                                               \
    std::string const pathname = arguments["<output>"]                         \
        ? arguments.at("<output>").asString() : "";                            \
    auto const environment{                                                    \
        lue::benchmark::detail::create_environment(arguments)};                \
    auto const task{                                                           \
        lue::benchmark::detail::create_task(arguments)};                       \
                                                                               \
    auto benchmark = setup_benchmark(argc, argv, environment, task);           \
                                                                               \
    int status = EXIT_FAILURE;                                                 \
                                                                               \
    if(!pathname.empty()) {                                                    \
        std::fstream stream;                                                   \
        stream.open(pathname, std::ios::out);                                  \
        status = run_benchmark(benchmark, stream);                             \
    }                                                                          \
    else {                                                                     \
        status = run_benchmark(benchmark, std::cout);                          \
    }                                                                          \
                                                                               \
    return status;                                                             \
}

}  // namespace benchmark
}  // namespace lue
