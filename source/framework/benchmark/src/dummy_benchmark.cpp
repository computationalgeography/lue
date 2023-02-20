#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/main.hpp"
#include <iostream>
#include <random>
#include <thread>


static void dummy(lue::benchmark::Environment const& environment, lue::benchmark::Task const& task)
{
    using namespace std::chrono_literals;

    auto const nr_workers{environment.nr_workers()};
    auto const work_size = task.nr_elements();

    std::random_device device;
    std::default_random_engine engine(device());
    std::uniform_real_distribution<double> distribution(0.0, 2000.0);
    auto const noise = distribution(engine);

    auto const parallelization_overhead = nr_workers * noise;

    // in ms → 10s
    auto const duration_per_work_item = 10000.0 + parallelization_overhead;

    auto const duration_of_work_size = work_size * duration_per_work_item;
    auto const duration_in_case_of_perfect_scaling = duration_of_work_size / nr_workers;

    std::chrono::duration<double, std::milli> duration{duration_in_case_of_perfect_scaling};

    std::this_thread::sleep_for(duration);
}


auto setup_benchmark(
    int /* argc */,
    char* /* argv */[],
    lue::benchmark::Environment const& environment,
    lue::benchmark::Task const& task)
{
    auto callable = dummy;

    return lue::benchmark::Benchmark{std::move(callable), environment, task};
}


LUE_CONFIGURE_BENCHMARK()
