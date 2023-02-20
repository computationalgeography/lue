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
    lue::benchmark::Environment const& environment,
    lue::benchmark::Task const& task)
{
    DummyAction dummy_;

    // std::string const name = "dummy_hpx";

    // Possible root to cluster name:
    // - HPX
    //     - find_root_locality (is this useful?)
    //     - Figure out how to obtain the hostname given a locality
    //       id. See implementation of get_error_host_name, which does
    //       something similar.
    // - SLURM
    //     - ???

    // std::string const description = "Dummy HPX benchmark";
    auto node = hpx::find_here();

    // Function to benchmark
    auto callable =
        [dummy_,
         node](lue::benchmark::Environment const& /* environment */, lue::benchmark::Task const& /* task */)
    {
        std::vector<hpx::future<void>> futures;

        futures.emplace_back(hpx::async(dummy_, node /* , environment, task */));
        futures.emplace_back(hpx::async(dummy_, node /* , environment, task */));
        futures.emplace_back(hpx::async(dummy_, node /* , environment, task */));

        hpx::wait_all(futures);
    };

    return lue::benchmark::Benchmark{std::move(callable), environment, task};  // , name, description};
}


LUE_CONFIGURE_HPX_BENCHMARK()
