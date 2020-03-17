#pragma once
#include "lue/framework/benchmark/benchmark.hpp"


namespace lue {
namespace benchmark {

template<
    typename Callable>
class ModelBenchmark:
    public Benchmark<Callable>
{

public:

                   ModelBenchmark      (Callable&& callable,
                                        Environment const& environment,
                                        Task const& task);

                   ModelBenchmark      (ModelBenchmark const&)=delete;

                   ModelBenchmark      (ModelBenchmark&&)=delete;

                   ~ModelBenchmark     ()=default;

    ModelBenchmark&
                   operator=           (ModelBenchmark const&)=delete;

    ModelBenchmark&
                   operator=           (ModelBenchmark&&)=delete;

    int            run                 () final;

private:

};


template<
    typename Callable>
inline ModelBenchmark<Callable>::ModelBenchmark(
    Callable&& callable,
    Environment const& environment,
    Task const& task):

    Benchmark<Callable>{std::forward<Callable>(callable), environment, task}

{
}


template<
    typename Callable>
inline int ModelBenchmark<Callable>::run()
{
    // Only time the simulate of the callable
    using namespace std::chrono_literals;

    auto& timings{this->timings()};
    auto& timing{this->timing()};
    auto const& environment{this->environment()};
    auto const& task{this->task()};
    auto& callable{this->callable()};

    timings.clear();
    Stopwatch stopwatch;

    auto const nr_time_steps = task.nr_time_steps();

    timing.start();
    for(std::size_t i = 0; i < environment.count(); ++i) {

        // Only measure the time it takes to simulate the state for all
        // time steps
        std::this_thread::sleep_for(2s);

        auto model = callable(environment, task);

        preprocess(model);

        stopwatch.start();

        {
            initialize(model);

            for(std::uint64_t t = 0; t < nr_time_steps; ++t) {
                simulate(model, t);
            }
        }

        stopwatch.stop();

        postprocess(model);

        timings.push_back(stopwatch);
    }
    timing.stop();

    return EXIT_SUCCESS;
}

}  // namespace benchmark
}  // namespace lue
