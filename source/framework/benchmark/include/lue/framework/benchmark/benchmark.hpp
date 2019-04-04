#pragma once
#include "lue/framework/benchmark/environment.hpp"
#include "lue/framework/benchmark/stopwatch.hpp"
#include "lue/framework/benchmark/task.hpp"
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
    using Timing = Stopwatch;
    using Timings = std::vector<Timing>;

                   Benchmark           (Callable&& callable,
                                        Environment const& environment,
                                        Task const& task);

                   Benchmark           (Benchmark const&)=delete;

                   Benchmark           (Benchmark&&)=delete;

                   ~Benchmark          ()=default;

    Benchmark&     operator=           (Benchmark const&)=delete;

    Benchmark&     operator=           (Benchmark&&)=delete;

    Environment const& environment     () const;

    Task const&    task                () const;

    int            run                 ();

    Timing const&  timing              () const;

    Timings const& timings             () const;

private:

    //! Callable representing the workload to time
    Callable       _callable;

    //! Environment the benchmark runs in
    Environment const _environment;

    Task const     _task;

    //! Name of the benchmark
    std::string const _name;

    Timing         _timing;

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
    Task const& task):

    _callable{std::forward<Callable>(callable)},
    _environment{environment},
    _task{task},
    _timing{},
    _timings{}

{
    // assert(!_array_shape.empty());
    // assert(std::size(_array_shape) == std::size(_partition_shape));
}


template<
    typename Callable>
Environment const& Benchmark<Callable>::environment() const
{
    return _environment;
}


template<
    typename Callable>
Task const& Benchmark<Callable>::task() const
{
    return _task;
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

    // Stream << _name << ": ";  // << std::flush;

    _timing.start();
    for(std::size_t i = 0; i < _environment.count(); ++i) {
        stopwatch.start();
        _callable(_environment, _task);
        stopwatch.stop();
        _timings.push_back(stopwatch);
        // Stream << ".";  // << std::flush;
    }
    // Stream << "\n";  // std::endl;
    _timing.stop();

    return EXIT_SUCCESS;
}


template<
    typename Callable>
inline typename Benchmark<Callable>::Timing const&
    Benchmark<Callable>::timing() const
{
    return _timing;
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
