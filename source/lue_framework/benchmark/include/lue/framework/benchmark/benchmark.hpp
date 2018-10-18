#pragma once
#include "lue/framework/benchmark/environment.hpp"
#include "lue/framework/benchmark/stopwatch.hpp"
#include "lue/framework/benchmark/timing.hpp"
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
    using Timings = std::vector<Timing>;

                   Benchmark           (Callable&& callable,
                                        Environment const& environment,
                                        std::string const& description,
                                        std::size_t count);

                   Benchmark           (Benchmark const&)=delete;

                   Benchmark           (Benchmark&&)=delete;

                   ~Benchmark          ()=default;

    Benchmark&     operator=           (Benchmark const&)=delete;

    Benchmark&     operator=           (Benchmark&&)=delete;

    Environment const& environment     () const;

    int            run                 ();

    TimeInterval const& time_interval  () const;

    Timings const& timings             () const;

private:

    //! Callable representing the workload to time
    Callable       _callable;

    //! Environment the benchmark runs in
    Environment    _environment;

    //! Description of the benchmark
    std::string    _description;

    //! Number of times a measurement must be taken
    std::size_t    _count;

    //! Interval in which the benchmark ran
    TimeInterval   _time_interval;

    //! Timings of the @a count benchmark runs
    Timings        _timings;

};


/*!
    @brief      Construct an instance

    In case of weird errors, verify that the callable is moved in.
*/
template<
    typename Callable>
inline Benchmark<Callable>::Benchmark(
    Callable&& callable,
    Environment const& environment,
    std::string const& description,
    std::size_t const count)
:
    _callable{std::forward<Callable>(callable)},
    _environment{environment},
    _description{description},
    _count{count},
    _time_interval{},
    _timings{}
{
}


template<
    typename Callable>
Environment const& Benchmark<Callable>::environment() const
{
    return _environment;
}


/*!
    @brief      Run the benchmarks @a count times
    @return     EXIT_SUCCESS or EXIT_FAILURE

    You can call this function multiple times, but any previous results
    will be overwritten in that case.

    After calling this function, @a count timings will be available.
*/
template<
    typename Callable>
inline int Benchmark<Callable>::run()
{
    _timings.clear();
    Stopwatch stopwatch;

    std::cout << _description << ": " << std::flush;

    auto const start = Stopwatch::now();
    for(std::size_t i = 0; i < _count; ++i) {
        stopwatch.start();
        _callable();
        stopwatch.stop();
        _timings.push_back(Timing{stopwatch.time_interval()});
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
    @brief      Return the timings of each run
*/
template<
    typename Callable>
inline typename Benchmark<Callable>::Timings const&
    Benchmark<Callable>::timings() const
{
    return _timings;
}

}  // namespace benchmark
}  // namespace lue
