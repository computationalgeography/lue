#pragma once
#include "lue/framework/benchmark/stopwatch.hpp"
#include <iostream>
#include <string>
#include <vector>


namespace lue {
namespace benchmark {

/*
Notes

Benchmark:
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
*/


/*!
    @brief      Class for managing a performance benchmark

    A performance benchmark is an experiment to see how fast a piece of
    code executeѕ. Benchmark instances will manage the timing of a
    single piece of code. Multiple timings can be collected in order to
    obtain information about the spread in the results.
*/
template<
    typename Callable>
class Benchmark
{

public:

    using Duration = Stopwatch::Duration;
    using Durations = std::vector<Duration>;

                   Benchmark           (Callable&& callable,
                                        std::string const& description,
                                        std::size_t count);

                   Benchmark           (Benchmark const&)=delete;

                   Benchmark           (Benchmark&&)=delete;

                   ~Benchmark          ()=default;

    Benchmark&     operator=           (Benchmark const&)=delete;

    Benchmark&     operator=           (Benchmark&&)=delete;

    int            run                 ();

    TimeInterval const& time_interval  () const;

    Durations const& durations         () const;

private:

    //! Callable representing the workload to time
    Callable       _callable;

    //! Description of the benchmark
    std::string    _description;

    //! Number of times a measurement must be taken
    std::size_t    _count;

    //! Interval in which the benchmark ran
    TimeInterval   _time_interval;

    //! Durations of the @a count benchmark runs
    Durations      _durations;

};


/*!
    @brief      Construct an instance

    In case of weird errors, verify that the callable is moved in.
*/
template<
    typename Callable>
inline Benchmark<Callable>::Benchmark(
    Callable&& callable,
    std::string const& description,
    std::size_t const count)
:
    _callable{std::forward<Callable>(callable)},
    _description{description},
    _count{count},
    _time_interval{},
    _durations{}
{
}


/*!
    @brief      Run the benchmarks
    @return     EXIT_SUCCESS or EXIT_FAILURE

    You can call this function multiple times, but any previous results
    will be overwritten in that case.
*/
template<
    typename Callable>
inline int Benchmark<Callable>::run()
{
    _durations.clear();
    Stopwatch stopwatch;

    std::cout << _description << ": " << std::flush;

    auto const start = Stopwatch::now();
    for(std::size_t i = 0; i < _count; ++i) {
        stopwatch.start();
        _callable();
        stopwatch.stop();
        _durations.push_back(stopwatch.elapsed());
        std::cout << "." << std::flush;
    }
    std::cout << std::endl;
    auto const stop = Stopwatch::now();

    _time_interval = TimeInterval{start, stop};

    return EXIT_SUCCESS;
}


/*!
    @brief      Return the time interval the experiment ran
*/
template<
    typename Callable>
inline TimeInterval const& Benchmark<Callable>::time_interval() const
{
    return _time_interval;
}


/*!
    @brief      Return the durations of each timing
*/
template<
    typename Callable>
inline typename Benchmark<Callable>::Durations const&
    Benchmark<Callable>::durations() const
{
    return _durations;
}

}  // namespace benchmark
}  // namespace lue
