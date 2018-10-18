#pragma once
#include "lue/framework/benchmark/environment.hpp"
#include "lue/framework/benchmark/format.hpp"
#include <docopt.h>
#include <fmt/format.h>
#include <fstream>


namespace lue {
namespace benchmark {
namespace detail {

std::string const usage = R"(
Run a performance benchmark

usage:
    {0} --nr_threads=<nr_threads> [<output>]
    {0} (-h | --help) | --version

options:
    --nr_threads=<nr_threads>  Maximum number of OS threads to use
    -h --help      Show this screen

Results will be written to the terminal if no output pathname is provided
)";


/*!
    @brief      Return an object representing the environment in which the
                benchmark is run
*/
Environment create_environment(
    std::map<std::string, docopt::value> const& arguments)
{
    std::size_t const nr_threads = arguments.at("--nr_threads").asLong();

    return Environment{nr_threads};
}

} // Namespace detail


template<
    typename Benchmark>
inline int run_benchmark(
    Benchmark& benchmark,
    Environment const& environment,
    std::ostream& stream)
{
    int status = benchmark.run();

    if(status != EXIT_SUCCESS) {
        std::cerr << "Running benchmark failed" << std::endl;
        return EXIT_FAILURE;
    }

    stream << format_as_json(benchmark, environment) << "\n";

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
                                                                               \
    auto benchmark = setup_benchmark(argc, argv, environment);                 \
                                                                               \
    int status = EXIT_FAILURE;                                                 \
                                                                               \
    if(!pathname.empty()) {                                                    \
        std::fstream stream;                                                   \
        stream.open(pathname, std::ios::out);                                  \
        status = run_benchmark(benchmark, environment, stream);                \
    }                                                                          \
    else {                                                                     \
        status = run_benchmark(benchmark, environment, std::cout);             \
    }                                                                          \
                                                                               \
    return status;                                                             \
}

}  // namespace benchmark
}  // namespace lue
