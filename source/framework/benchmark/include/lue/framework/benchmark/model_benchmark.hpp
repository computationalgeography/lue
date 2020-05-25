#pragma once
#include "lue/framework/benchmark/benchmark.hpp"


namespace lue {
namespace benchmark {

template<
    typename Callable,
    typename Result>
class ModelBenchmark:
    public BenchmarkBase<Callable>
{

public:

    using Results = std::vector<Result>;

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

    Results const& results () const
    {
        return _results;
    }

private:

    void clear_results()
    {
        _results.clear();
    }

    void add_result(
        Result result)
    {
        _results.push_back(std::move(result));
    }

    Results _results;

};


template<
    typename Callable,
    typename Result>
inline ModelBenchmark<Callable, Result>::ModelBenchmark(
    Callable&& callable,
    Environment const& environment,
    Task const& task):

    BenchmarkBase<Callable>{
        std::forward<Callable>(callable), environment, task}

{
}


template<
    typename Callable,
    typename Result>
inline int ModelBenchmark<Callable, Result>::run()
{
    // Only time the simulate of the callable
    using namespace std::chrono_literals;

    auto& timings{this->timings()};
    auto& timing{this->timing()};
    auto const& environment{this->environment()};
    auto const& task{this->task()};
    auto& callable{this->callable()};

    timings.clear();
    this->clear_results();
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

            terminate(model);
        }

        Result result = model.result();

        stopwatch.stop();

        postprocess(model);

        timings.push_back(stopwatch);
        this->add_result(std::move(result));
    }
    timing.stop();

    return EXIT_SUCCESS;
}


template<
    typename Callable,
    typename Result>
class TypeTraits<ModelBenchmark<Callable, Result>>
{

public:

    using ResultT = Result;

};

}  // namespace benchmark
}  // namespace lue
