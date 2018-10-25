#include "lue/framework/benchmark/benchmark.hpp"
#include "lue/framework/benchmark/main.hpp"
#include <iostream>
#include <random>
#include <thread>


static void dummy(
    lue::benchmark::Environment const& environment)
{
    using namespace std::chrono_literals;

    auto const nr_localities = environment.nr_localities();
    auto const nr_threads = environment.nr_threads();
    auto const work_size = environment.work_size();

    std::random_device device;
    std::default_random_engine engine(device());
    std::uniform_real_distribution<double> distribution(0.0, 2000.0);
    auto const noise = distribution(engine);

    auto const parallelization_overhead = nr_threads * noise;

    // in ms → 10s
    auto const duration_per_work_item = 10000.0 + parallelization_overhead;

    auto const duration_of_work_size = work_size * duration_per_work_item;
    auto const nr_workers = nr_localities * nr_threads;
    auto const duration_in_case_of_perfect_scaling =
        duration_of_work_size / nr_workers;

    std::chrono::duration<double, std::milli> duration{
        duration_in_case_of_perfect_scaling};

    std::this_thread::sleep_for(duration);
}


auto setup_benchmark(
        int /* argc */,
        char* /* argv */[],
        lue::benchmark::Environment const& environment)
{
    auto callable = dummy;
    std::string const name = "dummy";
    std::string const description = "Dummy benchmark";

    return lue::benchmark::Benchmark{
        std::move(callable), environment, name, description};
}


LUE_CONFIGURE_BENCHMARK()
