#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include <hpx/hpx.hpp>
#include <thread>


void dummy()
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
}


HPX_PLAIN_ACTION(dummy, DummyAction);


auto setup_benchmark(
        int /* argc */,
        char* /* argv */[])
{
    std::string const description = "Dummy HPX benchmark";
    // auto const environment = hpx_runtime_environment();
    std::size_t const count = 3;
    DummyAction dummy;
    auto node = hpx::find_here();

    // Function to benchmark
    auto callable = [dummy, node]() {
        std::vector<hpx::future<void>> futures;

        futures.emplace_back(hpx::async(dummy, node));
        futures.emplace_back(hpx::async(dummy, node));
        futures.emplace_back(hpx::async(dummy, node));

        hpx::wait_all(futures);
    };

    return lue::benchmark::Benchmark{description, count, std::move(callable)};
}


LUE_CONFIGURE_HPX_BENCHMARK()
