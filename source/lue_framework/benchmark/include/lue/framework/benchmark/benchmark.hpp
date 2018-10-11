#pragma once
#include "lue/framework/benchmark/stopwatch.hpp"
#include <iostream>
#include <string>
#include <vector>


namespace lue {
namespace benchmark {

/*!
    Class for managing a performance benchmark

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

                   Benchmark           (std::string const& description,
                                        std::size_t count,
                                        Callable&& callable);

                   Benchmark           (Benchmark const&)=delete;

                   Benchmark           (Benchmark&&)=delete;

                   ~Benchmark          ()=default;

    Benchmark&     operator=           (Benchmark const&)=delete;

    Benchmark&     operator=           (Benchmark&&)=delete;

    int            run                 ();

    Durations const& durations         () const;

private:

    std::string    _description;

    std::size_t    _count;

    Callable       _callable;

    Durations      _durations;

};


template<
    typename Callable>
inline Benchmark<Callable>::Benchmark(
    std::string const& description,
    std::size_t const count,
    Callable&& callable)
:
    _description{description},
    _count{count},
    _callable{std::forward<Callable>(callable)}
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

    for(std::size_t i = 0; i < _count; ++i) {
        stopwatch.start();
        _callable();
        stopwatch.stop();
        _durations.push_back(stopwatch.elapsed());
        std::cout << "." << std::flush;
    }

    std::cout << std::endl;

    return EXIT_SUCCESS;
}


template<
    typename Callable>
inline typename Benchmark<Callable>::Durations const&
    Benchmark<Callable>::durations() const
{
    return _durations;
}

}  // namespace benchmark
}  // namespace lue
