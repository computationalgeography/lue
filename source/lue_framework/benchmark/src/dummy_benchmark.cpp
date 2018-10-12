#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/main.hpp"
#include <thread>


void dummy()
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
}


auto setup_benchmark(
        int /* argc */,
        char* /* argv */[])
{
    std::string const description = "Dummy benchmark";
    // auto const environment = runtime_environment();
    std::size_t const count = 3;
    auto callable = dummy;

    return lue::benchmark::Benchmark{std::move(callable), description, count};
}


LUE_CONFIGURE_BENCHMARK()
