#pragma once
#include "lue/framework/benchmark/write_results.hpp"
#include <hpx/hpx_finalize.hpp>
#include <hpx/hpx_init.hpp>


namespace lue {
namespace benchmark {

template<
    typename Benchmark>
inline int run_hpx_benchmark(
    Benchmark& benchmark)
{
    int status = benchmark.run();

    if(status != EXIT_SUCCESS) {
        std::cerr << "Running benchmark failed" << std::endl;
        return EXIT_FAILURE;
    }

    status = hpx::finalize();

    if(status != 0) {
        std::cerr << "Stopping HPX failed" << std::endl;
        return EXIT_FAILURE;
    }

    write_results(benchmark);

    return EXIT_SUCCESS;
}


#define LUE_CONFIGURE_HPX_BENCHMARK()              \
int hpx_main(                                      \
    int argc,                                      \
    char* argv[])                                  \
{                                                  \
    auto benchmark = setup_benchmark(argc, argv);  \
                                                   \
    return run_hpx_benchmark(benchmark);           \
}                                                  \
                                                   \
                                                   \
int main(                                          \
    int argc,                                      \
    char* argv[])                                  \
{                                                  \
    auto status = hpx::init(argc, argv);           \
                                                   \
    return status;                                 \
}

}  // namespace benchmark
}  // namespace lue
