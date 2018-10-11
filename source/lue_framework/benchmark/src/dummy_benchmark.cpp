// Single benchmark
// - Meta information:
//     - Properties of node / nodes
//     - Time point at start
//     - Time point at finish

/*
Benchmark
- Measure performance of some solution (exe, parameters, system)
- Benchmark is an exe
    - Work to benchmark must be a callable in a lib
    - Each run must be a call to this function, timed by the benchmark
        instance
- Description
- Meta-information
    - exe
    - parameters
    - system
    - ...
- Timings (multiple equivalent runs, capture spread)


Benchmark
    description
    system
    exe
        path
        build_configuration
    parameters
    timings
        timing
            start
            end


HPXBenchmark
    system
        localities
        ...
    hpx
        build_configuration


ScalingBenchmark
- Series of benchmarks keeping most things fixed except for
    - Number of cores (local scaling)
    - Number of nodes (distributed scaling)
    - Job size (weak scaling)


BenchmarkSuite
- Set of, possibly not-related, benchmarks


- Output to JSON
- Import to LUE and work from there, but keep the JSONs
*/


#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/main.hpp"
#include <thread>


void dummy()
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    // std::cout << "dummy is running!" << std::endl;
}


// User-supplied benchmark construction logic ----------------------------------
auto setup_benchmark(
        int /* argc */,
        char* /* argv */[])
{
    std::string const description = "Dummy benchmark";
    // auto const environment = runtime_environment();
    std::size_t const count = 3;
    auto callable = dummy;

    return lue::benchmark::Benchmark{description, count, std::move(callable)};
}


LUE_CONFIGURE_BENCHMARK()
