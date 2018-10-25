#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include <hpx/hpx.hpp>
#include <thread>


static void dummy()
    // lue::benchmark::Environment const& /* environment */)
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
}


HPX_PLAIN_ACTION(dummy, DummyAction);


auto setup_benchmark(
        int /* argc */,
        char* /* argv */[],
        lue::benchmark::Environment const& environment)
{
    DummyAction dummy;
    std::string const name = "dummy_hpx";
    std::string const description = "Dummy HPX benchmark";
    auto node = hpx::find_here();

    // Function to benchmark
    auto callable = [dummy, node](
        lue::benchmark::Environment const& /* environment */)
    {
        std::vector<hpx::future<void>> futures;

        futures.emplace_back(hpx::async(dummy, node /* , environment */));
        futures.emplace_back(hpx::async(dummy, node /* , environment */));
        futures.emplace_back(hpx::async(dummy, node /* , environment */));

        hpx::wait_all(futures);
    };

    return lue::benchmark::Benchmark{
        std::move(callable), environment, name, description};
}


LUE_CONFIGURE_HPX_BENCHMARK()
