#pragma once
#include "lue/framework/benchmark/write_results.hpp"


namespace lue {
namespace benchmark {

template<
    typename Benchmark>
inline int run_benchmark(
    Benchmark& benchmark)
{
    int status = benchmark.run();

    if(status != EXIT_SUCCESS) {
        std::cerr << "Running benchmark failed" << std::endl;
        return EXIT_FAILURE;
    }

    write_results(benchmark);

    return EXIT_SUCCESS;
}


#define LUE_CONFIGURE_BENCHMARK()                  \
int main(                                          \
    int argc,                                      \
    char* argv[])                                  \
{                                                  \
    auto benchmark = setup_benchmark(argc, argv);  \
                                                   \
    return run_benchmark(benchmark);               \
}

}  // namespace benchmark
}  // namespace lue
