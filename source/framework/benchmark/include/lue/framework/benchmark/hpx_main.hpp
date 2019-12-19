#pragma once
#include "lue/framework/benchmark/environment.hpp"
#include "lue/framework/benchmark/format.hpp"
#include "lue/framework/core/configuration_entry.hpp"
#include "lue/framework/core/debug.hpp"
#include <hpx/hpx_finalize.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/lcos.hpp>
#ifdef HPX_WITH_APEX
    #include <apex_api.hpp>
    #define register_apex_print_options() \
        hpx::register_startup_function(&apex::print_options);
#else
    #define register_apex_print_options()
#endif
#include <fstream>
#include <iostream>


namespace lue {
namespace benchmark {
namespace detail {

/*!
    @brief      Return an object representing the environment in which the
                benchmark is run
*/
Environment create_environment()
{
    // std::string const cluster_name =
    //     required_configuration_entry<std::string>("benchmark.cluster_name");
    std::uint64_t const count =
        required_configuration_entry<std::uint64_t>("benchmark.count");
    std::uint64_t const nr_localities =
        required_configuration_entry<std::uint64_t>("hpx", "localities");
    std::uint64_t const nr_threads =
        required_configuration_entry<std::uint64_t>("hpx", "os_threads");
    std::uint64_t const max_tree_depth =
        optional_configuration_entry<std::uint64_t>(
            "benchmark.max_tree_depth", 0);
    // std::uint64_t const work_size =
    //     required_configuration_entry<std::uint64_t>("benchmark.work_size");

    return Environment{
        count, nr_localities, nr_threads,
        max_tree_depth > 0
            ? std::optional<std::size_t>{max_tree_depth}
            : std::optional<std::size_t>{}
    };
}


Task create_task()
{
    std::uint64_t const nr_time_steps =
        required_configuration_entry<std::uint64_t>("nr_time_steps");
    Task::Shape const array_shape =
        required_configuration_entry<Task::Shape>("array_shape");
    Task::Shape const partition_shape =
        required_configuration_entry<Task::Shape>("partition_shape");

    return Task{nr_time_steps, array_shape, partition_shape};
}

} // Namespace detail


template<
    typename Benchmark>
inline void run_hpx_benchmark(
    Benchmark& benchmark,
    std::ostream& stream)
{
    int status = benchmark.run();

    if(status == EXIT_SUCCESS) {
        stream << format_as_json(benchmark) << "\n";
    }
}

}  // namespace benchmark
}  // namespace lue


// MS:
// Because of some changes in APEX the task names in the OTF file get the
// first name of the task. We don't have a good solution for this yet. As
// a hack ... → KDJ: see annotate_function and yield below

#define LUE_CONFIGURE_HPX_BENCHMARK(                                           \
    configuration)                                                             \
int hpx_main(                                                                  \
    int argc,                                                                  \
    char* argv[])                                                              \
{                                                                              \
    hpx::util::annotate_function annotation("hpx_main");                       \
    hpx::this_thread::yield();                                                 \
                                                                               \
    std::cout << lue::system_description().get() << std::endl;                 \
                                                                               \
    std::string const pathname =                                               \
        lue::optional_configuration_entry<std::string>(                        \
            "benchmark.output", "");                                           \
                                                                               \
    auto const environment{                                                    \
        lue::benchmark::detail::create_environment()};                         \
    auto const task{                                                           \
        lue::benchmark::detail::create_task()};                                \
                                                                               \
    auto benchmark = setup_benchmark(argc, argv, environment, task);           \
                                                                               \
    if(!pathname.empty()) {                                                    \
        std::fstream stream;                                                   \
        stream.open(pathname, std::ios::out);                                  \
        lue::benchmark::run_hpx_benchmark(benchmark, stream);                  \
    }                                                                          \
    else {                                                                     \
        lue::benchmark::run_hpx_benchmark(benchmark, std::cout);               \
    }                                                                          \
                                                                               \
    return hpx::finalize();                                                    \
}                                                                              \
                                                                               \
                                                                               \
int main(                                                                      \
    int argc,                                                                  \
    char* argv[])                                                              \
{                                                                              \
    std::vector<std::string> cfg = {                                           \
        configuration                                                          \
    };                                                                         \
                                                                               \
    register_apex_print_options() \
                                                                               \
    return hpx::init(argc, argv, cfg);                                         \
}
